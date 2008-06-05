/*! \file
    \brief Class to handle the bt_http_ecnx_cnx_t

\par Note about having multiple simultaneous bt_http_ecnx_iov_t
- this allows to have a http request queueing directly handled by the 
  http_client_pool_t
  - thus eliminate the issue of request space fragmentation due to the bt_pselect_t
    choosing randomly the piece.
- 2 part about bt_http_ecnx_iov_t
  1. launching it
  2. delivering the result
- bt_http_ecnx_iov_t handled in a list ecnx_iov_db
  - bt_http_ecnx_iov_t linking itself to the bt_http_ecnx_cnx_t
- this implies to know which bt_http_ecnx_req_t bt_iov_t is currently being 
  processed or not.
  - this state could be stored in bt_http_ecnx_req_t itself ?
  - this state could be deduced from the current bt_http_ecnx_iov_t
  - the 'state in http_ecnx_req_t' break the unicity rules as the state is already
    in the bt_http_ecnx_iov_t
- delivering the result seems very similar
  - in fact i dont even see what to change
- launching it
  - i should not launch bt_iov_t which are already processed by another ecnx_iov
  - how to do that ?
  - bt_http_ecnx_iov_t::subfile_range() 
  - sanity check - in bt_http_ecnx_iov_t check that all other queued 

\par absence of http request queuing considered harmfull
- at first i though i would able to get away with not actually queuing request
  at the http level.
  - aka i send a http request and dont send any new one before receiving the
    reply of the first request.
- i did a scater/gather to regroup the requests and reduce this effect
- but i got trouble when the bt_pselect start to send many small requests
  accross the whole thing.
- i got the multiple concurrent connections which can used simultaneously.
  - but this use more resource and especially some http server restricts the numbers
    of connections
    - e.g. like if there is already 2 connections being process, dont answer the other one ?
    - or something close to that
  - and the pselect selecting randomly fragment the space to request and increase
    significantly the cost of non queuing request
  - so i think one should do a multiple concurrent request on a single connection
- POSSIBLE SOLUTION:
  - remove the concurrent connection stuff at the bt_http_ecnx_pool_t level
  - handle multiple simultaneous bt_http_ecnx_iov_t ?
    - what about a subfile http_uri_t present several time  on the same hostport_str ?
    - currently one instance is handled by a given bt_http_ecnx_cnx_t instance
  - let the http_client_pool_t handle the matter
    - it will be modified latter to determine the number of simultaneous connection
  - at the bt_http_ecnx_pool_t level, i send as many bt_http_ecnx_iov_t as
    needed
    
\par ABOUT error handling
- 2 type of error
  - cnx error = quite frequent - occurs even in normal case 
  - file error = much less frequent
- any type of error cnx or file trigger a deconnection
- it reconnect immediatly if it is a file error 
- it reconnect after a while if it is a cnx error
- the file in error are handled in http_ecnx_pool thru a set<http_uri_t>
  - currently they never expires
  - LATER: will expire after a quite long timeout, e.g. 10min
- what is the name for the connection in error ?
  - bt_http_ecnx_cnxerr_t ?
- what about the identifier ? hostport_str alone ?
  - what about the concurrent_idx ?
  - is this still required to have multiple connection per hostport ?
  - what is the goal ? for the very specific case, multiple uri to the same hostport
    on a single subfile ?
  - this seems like very overkill
  - the original goal of this concurrent_idx was to get multiple connection at the 
    same time to increase the bandwidth
  - and now the actual number of http connection is handled within the http layer
    and is completly masked here
  - seems like one should remove this concurrent connection stuff
  - so the hostport_str is the identifier of the bt_http_ecnx_cnx_t
- when a cnx enter in error
  - if there are no cnxerr for this hostport, create one
  - notify the cnxerr that it now have a connection with an state indicating the error
  - delete the bt_http_ecnx_cnx_t
  - when the timer of cnxerr expire, it recreate the bt_http_ecnx_cnx_t for it
- as the connection issue occurs even in normal case im not sure on how to 
  handle such problem ?
  - well dunno for now
  - would require to actually understand how those error are determined

\par TODO do the error handling
- how to handle the error case within the http connection aka in bt_http_ecnx_iov_t
  - a connection should be able to deregister itself and reregister itself after a delay
  - when a issue occurs at the network level, do that
  - when a error occurs at the http level, disable the file and do that
  - when the file got reenable, what to do ?
    - is it possible to add file on the fly
    - one dont want to disable the whole connection everytime
- TODO to trigger an error at the connection level
  - put 10 simultaneous cnx
  - req_queue_delay = 1sec
  - download go-open
  - it create a error every time :)
    - it was before i fixed the race in the launch_new_iov
    - so maybe disable the fix to recreate the error
    
\par TODO 
- TODO it doesnt update the bt_swarm_stats_t::dloaded_datalen
  - this should be kept into account in the scheduler instead ???
- TODO put some statistic at the cnx level and the pool level
  - e.g. the average max queue fill ratio
  - handle the dloaded_size and overhead_size
    - same for bt_swarm_full_t


\par Brief description
- bt_http_ecnx_cnx_t handle external connection for bt_swarm_t. it simulates a 
  single http connection toward a given hostport_str.
- it declare all the pieces which as fully available from this hostport_str as available
  to the bt_swarm_t.

\par About the request grouping
- there is never more than one bt_http_ecnx_iov_t running 
- in order to limit the overhead of the http headers in the request/reply, 
  the bt_swarm_sched_request_t are regrouped together when they are contiguous.
  - thus if the scheduler makes contiguous requests, it will be much more efficient.
- when the recved data from the grouped http request are notified to each of the
  bt_swarm_sched_request_t, it is done thru within a single event loop iteration
  - it is made to allow playing with zerotimer within the scheduler and thus
    make it easier to create all the bt_swarm_sched_request_t in a row and thus
    easier to make them sequential.

\par Possible improvement - availability at the byte granularity
- NOTE: this is usefull only for multiple file .torrent with a particular http_uri_t per subfile
  - aka not very usefull... as it is a very particular case and require quite a lot of change
    - but i leave the note just in case it becomes usefull one day.
- handle a bt_pieceavail_t piecepartavail which contains all the pieces which are at least
  partially available
- keep a interval_t of the byte range available
- put a bt_ecnx_vapi_t virtual function range_avail(totfile_beg, totfile_end)
  - or something similar as this API doesnt seems to handle the need
- this would allow to modify the scheduler to handle the byte granularity
  - need some modification in the scheduler tho
- this would allow to get all data available via http
  - currently all piece which are not fully available via a single hostport_str
    cant be downloaded thru bt_http_ecnx_t

*/

/* system include */
/* local include */
#include "neoip_bt_http_ecnx_cnx.hpp"
#include "neoip_bt_http_ecnx_pool.hpp"
#include "neoip_bt_http_ecnx_req.hpp"
#include "neoip_bt_http_ecnx_iov.hpp"
#include "neoip_bt_http_ecnx_herr.hpp"
#include "neoip_bt_ecnx_event.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_sched_request.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_interval.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_http_ecnx_cnx_t::bt_http_ecnx_cnx_t()		throw()
{
	// zero some fields
	m_ecnx_pool	= NULL;
	m_ecnx_iov	= NULL;
	callback	= NULL;
}

/** \brief Destructor
 */
bt_http_ecnx_cnx_t::~bt_http_ecnx_cnx_t()		throw()
{
	// delete all the bt_http_ecnx_req_t
	while( !ecnx_req_db.empty() )	nipmem_delete ecnx_req_db.begin()->second->get_sched_req();
	// delete bt_http_ecnx_iov_t if needed
	nipmem_zdelete	m_ecnx_iov;
	
	// notify the callback
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	if( callback ){
		bool	tokeep	= notify_callback( bt_ecnx_event_t::build_cnx_closed() );
		DBG_ASSERT( tokeep == true );
	}
	// unlink this object from the bt_http_ecnx_pool_t if needed
	if( m_ecnx_pool )	m_ecnx_pool->ecnx_cnx_unlink(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_http_ecnx_cnx_t &bt_http_ecnx_cnx_t::set_profile(const bt_http_ecnx_cnx_profile_t &profile) throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t bt_http_ecnx_cnx_t::start(bt_http_ecnx_pool_t *ecnx_pool, const std::string &hostport_str
					, bt_ecnx_vapi_cb_t *callback, void *userptr)	throw()
{
	// log to debug 
	KLOG_DBG("enter hostport_str=" << hostport_str);
	// copy the parameter
	this->m_ecnx_pool	= ecnx_pool;
	this->m_hostport_str	= hostport_str;
	this->callback		= callback;
	this->userptr		= userptr;
	// link this object to the bt_http_ecnx_pool_t
	m_ecnx_pool->ecnx_cnx_dolink(this);

	// init the rate_estim_t
	m_recv_rate		= rate_estim_t<size_t>(m_profile.rate_estim_arg());

	// build the subfile_uri_arr
	build_subfile_uri_arr();
	// build the pieceavail
	build_pieceavail();

	// notify the callback
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	bool	tokeep	= notify_callback( bt_ecnx_event_t::build_cnx_opened() );
	DBG_ASSERT( tokeep == true );
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief build the subfile_uri_arr 
 * 
 * - the subfile_uri_arr is used to generate the bt_pieceavail_t and inside 
 *   bt_http_ecnx_iov_t to determine the http_uri_t of a given subfile_idx
 * - subfile_uri_arr is an array of http_uri_t of the same length as the bt_mfile_subfile_arr_t.
 *   - if the http_uri_t is not null, it is the chosen http_uri_t for this bt_mfile_subfile_t subfile_idx
 *   - else the http_uri_t is not available thru this bt_http_ecnx_cnx_t
 * - NOTE: if a given bt_mfile_subfile_t contains several http_uri_t for the 
 *   same hostport_str, the bt_http_ecnx_cnx_t choose one randomly
 */
void	bt_http_ecnx_cnx_t::build_subfile_uri_arr()	throw()
{
	const bt_mfile_t &		bt_mfile	= m_ecnx_pool->get_swarm()->get_mfile();
	const bt_mfile_subfile_arr_t &	subfile_arr	= bt_mfile.subfile_arr();
	bt_http_ecnx_herr_t *		ecnx_herr	= m_ecnx_pool->ecnx_herr_by_hostport_str(m_hostport_str);	
	std::set<http_uri_t>		uri_blacklist;
	bt_err_t			bt_err;

	// get the uri_blacklist from the ecnx_herr, if there is any
	if( ecnx_herr )	uri_blacklist	= ecnx_herr->get_uri_blacklist();
	
	// go thru the whole bt_mfile_t::subfile_arr()
	for(size_t subfile_idx = 0; subfile_idx < subfile_arr.size(); subfile_idx++){
		const http_uri_arr_t &	uri_arr		= subfile_arr[subfile_idx].uri_arr();
		http_uri_arr_t		filtered_arr;
		// build the filtered_arr containing only the http_uri_t with the same hostport_str
		for(size_t i = 0; i < uri_arr.size(); i++){
			const http_uri_t &	http_uri	= uri_arr[i];
			// if this http_uri_t doesnt match the local hostport_str, skip it
			if( hostport_str() != http_uri.hostport_str() )			continue;
			// if this http_uri is blacklisted, skip it
			if( uri_blacklist.find(http_uri) != uri_blacklist.end())	continue;
			// add it to the filtered_arr
			filtered_arr	+= http_uri;
		}
		// if the filtered_arr is empty, add a null http_uri_t for this subfile_idx
		if( filtered_arr.empty() ){
			subfile_uri_arr	+= http_uri_t();
		}else{	// else add a random http_uri_t of the filtered array
			subfile_uri_arr += filtered_arr[rand() % filtered_arr.size()];
		}
	}
}
/** \brief build the pieceavail_t
 */
void	bt_http_ecnx_cnx_t::build_pieceavail()		throw()
{
	const bt_mfile_t &	bt_mfile	= m_ecnx_pool->get_swarm()->get_mfile();
	// sanity check - subfile_uri_arr MUST BE already initialized
	DBG_ASSERT( subfile_uri_arr.size() == bt_mfile.subfile_arr().size() );

// build the interval of the zone available
	interval_t<file_size_t>		avail_intv;
	// go thru the whole subfile_uri_arr
	for(size_t subfile_idx = 0; subfile_idx < subfile_uri_arr.size(); subfile_idx++){
		const http_uri_t &		http_uri	= subfile_uri_arr[subfile_idx];
		const bt_mfile_subfile_t &	mfile_subfile	= bt_mfile.subfile_arr()[subfile_idx];
		const file_range_t &		totfile_range	= mfile_subfile.totfile_range();
		// if this http_uri_t is empty, skip it
		if( http_uri.is_null() )	continue;
		// add it to the avail_intv
		avail_intv	+= totfile_range.to_inval_item();
	}

	// init the local bt_pieceavail_t
	pieceavail	= bt_pieceavail_t(bt_mfile.nb_piece());
	
// set the piece as available in pieceavail_t IIF it is fully available in avail_intv
	// go thru each piece of the bt_mfile_t
	for(size_t piece_idx = 0; piece_idx < bt_mfile.nb_piece(); piece_idx++){
		file_range_t	piece_totrange	= bt_unit_t::pieceidx_to_totfile_range(piece_idx, bt_mfile);
		// if the piece_item is NOT fully included in the avail_intv, goto the next
		if( !avail_intv.fully_include(piece_totrange.to_inval_item()) )	continue;
		// mark the piece as available
		pieceavail.mark_isavail(piece_idx);
	}
}

/** \brief Return true if the bt_http_ecnx_cnx_t is sane, false otherwise
 * 
 * - being not sane implies a BUG!
 * - TODO currently this function is not used, use it
 */
bool	bt_http_ecnx_cnx_t::is_sane()						const throw()
{
	// TODO check that all request are distinct
	
	// return true;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     bt_http_ecnx_req_t dolink/unlink
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief DoLink a bt_http_ecnx_req_t to this bt_http_ecnx_cnx_t
 */
void	bt_http_ecnx_cnx_t::ecnx_req_dolink(bt_http_ecnx_req_t *ecnx_req)	throw()
{
	bool	succeed;
	// log to debug
	KLOG_DBG("enter");
	// add it to the ecnx_req_fifo
	ecnx_req_fifo.push_back(ecnx_req);
	// add it to the ecnx_req_db
	succeed = ecnx_req_db.insert(std::make_pair(ecnx_req->get_sched_req(), ecnx_req)).second;
	DBG_ASSERT( succeed );
	// add it to the ecnx_req_offs
	succeed = ecnx_req_offs.insert(std::make_pair(ecnx_req->orig_totfile_beg(), ecnx_req)).second;
	DBG_ASSERT( succeed );
}

/** \brief UnLink a bt_http_ecnx_req_t to this bt_http_ecnx_cnx_t
 */
void	bt_http_ecnx_cnx_t::ecnx_req_unlink(bt_http_ecnx_req_t *ecnx_req)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// remove it from the ecnx_req_fifo
	ecnx_req_fifo.remove(ecnx_req);
	// remove it from the ecnx_req_offs
	ecnx_req_offs.erase(ecnx_req->orig_totfile_beg());
	// remove it to the ecnx_req_db
	ecnx_req_db.erase(ecnx_req->get_sched_req());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  bt_swarm_sched_cnx_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief DoLink and init a new bt_swarm_sched_request_t to this connection
 */
void	bt_http_ecnx_cnx_t::sched_req_dolink_start(bt_swarm_sched_request_t *sched_req) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// create the bt_http_ecnx_req_t for this bt_swarm_sched_request_t 
	// - it links itself to the bt_http_ecnx_cnx_t
	nipmem_new bt_http_ecnx_req_t(this, sched_req);

	// launch the next_iov_zerotimer if not yet done
	// - to launch the bt_http_ecnx_iov_t which may gather several bt_http_ecnx_req_t
	//   on the next loop iteration.
	if( next_iov_zerotimer.empty() )	next_iov_zerotimer.append(this, NULL);
}

/** \brief UnLink a new bt_swarm_sched_request_t from this connection
 */
void	bt_http_ecnx_cnx_t::sched_req_unlink(bt_swarm_sched_request_t *sched_req)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - the bt_swarm_sched_request_t MUST be present 
	DBG_ASSERT( ecnx_req_db.find(sched_req) != ecnx_req_db.end() );
	// delete the bt_http_ecnx_req_t
	nipmem_delete	ecnx_req_db.find(sched_req)->second;

	// if no pending bt_http_ecnx_req_t and kill_iov_zerotimer is not started, launch it
	if( ecnx_req_db.empty() && kill_iov_zerotimer.empty())	kill_iov_zerotimer.append(this, NULL);
}

/** \brief Cancel a request made on this connection
 * 
 * - send a bt_cmdtype_t::BLOCK_DEL
 */
void	bt_http_ecnx_cnx_t::sched_req_cancel(bt_swarm_sched_request_t *sched_req)	throw()
{
	// TODO i dont think i have anything to do - to check
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the maximum number of request to make on this connection
 * 
 * - currently it is based on a pseudo bandwidth-delay product
 * - NOTE: it may be good to get various policy for this queue, tunnable from
 *   outside the bt_swarm_t
 * - bt_swarm_full_sched_t has the same function
 */
size_t	bt_http_ecnx_cnx_t::req_queue_maxlen()		const throw()
{
	double	recv_rate	= recv_rate_avg();
	
	// compute a pseudo bandwidth-delay product - it is pseudo because:
	// - the latency is hardcoded in the profile as it is hard/impossible
	//   to measure in this context
	// - a request BLOCK_REQ, may not be sent immediatly e.g. if a BLOCK_REP is in progress
	size_t	req_queue_len	= size_t(m_profile.req_queue_delay().to_sec_double() * recv_rate);
	
	// compute the actual req_queue_minlen
	size_t req_queue_minlen	= m_profile.req_queue_minlen();
	// if m_is_cnx_close, then multiply req_queue_ming len by req_queue_minlen_cnxclose_mult
	// - it allows to absorb the overhead of establishing the tcp connection at every req
	if( m_is_cnx_close ) 	req_queue_minlen *= m_profile.req_queue_minlen_cnxclose_mult();

	// clamp the req_queue_len based on the profile minlen/maxlen
	req_queue_len	= std::max(req_queue_len, req_queue_minlen);
	req_queue_len	= std::min(req_queue_len, m_profile.req_queue_maxlen());

	// return the computed result
	return req_queue_len;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	bt_http_ecnx_cnx_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// just forward to the proper handler
	if( &next_iov_zerotimer == &cb_zerotimer )	return next_iov_zerotimer_expire_cb(cb_zerotimer, userptr);
	if( &kill_iov_zerotimer == &cb_zerotimer )	return kill_iov_zerotimer_expire_cb(cb_zerotimer, userptr);
	// sanity check - this point MUST NEVER be reached
	DBG_ASSERT(false);
	// return tokeep
	return true;
}

/** \brief callback called when the next_iov_zerotimer expire
 */
bool	bt_http_ecnx_cnx_t::next_iov_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// launch the next bt_http_ecnx_iov_t
	launch_next_iov();
	// return tokeep
	return true;
}

/** \brief callback called when the kill_iov_zerotimer expire
 * 
 * - it is required to do that in zerotimer_t in order to avoid deletion 
 *   during a bt_http_ecnx_iov_t callback or other weird cases like that.
 * - kill_iov_zerotimer is used only to delete bt_http_ecnx_iov_t when there
 *   are no more bt_http_ecnx_req_t
 *   - this may happen if the bt_pieceprec_t for the piece of this req changed
 *     to make it unneeded.
 *     - e.g. when a bt_httpo_full_t is established thru neoip-oload, requests
 *       are issued, but if bt_httpo_full_t is closed before they are delivered
 *       all the requests are on bt_pieceprec_t::NOTNEEDED
 *   - this may happen if the request has been satified by another connection
 */
bool	bt_http_ecnx_cnx_t::kill_iov_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// if some bt_http_ecnx_req_t have been inserted since kill_iov_zerotimer started
	// then do nothing
	if( !ecnx_req_db.empty() )	return true;
	
	// delete m_ecnx_iov
	nipmem_zdelete	m_ecnx_iov;
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    launch_next_iov
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the next bt_http_ecnx_cnx_t
 */
void	bt_http_ecnx_cnx_t::launch_next_iov()				throw()
{
	bt_err_t		bt_err;
	// if there is no request to process, return now
	if( ecnx_req_db.empty() )	return;
	// if there is already a bt_http_ecnx_iov_t, return now
	if( m_ecnx_iov )		return;

	// get the gathered bt_iov_t
	bt_iov_t bt_iov	= build_gathered_iov();
	// if no bt_iov is to process, return now
	if( bt_iov.is_null() )		return;
	
	// create and start the bt_http_ecnx_iov_t
	m_ecnx_iov	= nipmem_new bt_http_ecnx_iov_t();
	bt_err		= m_ecnx_iov->start(this, bt_iov, this, NULL);
	DBG_ASSERT( bt_err.succeed() );	// TODO poor error management - what to do in case of error
					// - should i return a tokeep for this function ?
					// - disable it forever ?
}

/** \brief gather bt_iov_t from the pending bt_http_ecnx_req_t, or null if none is to process
 * 
 * brief description of the algo:
 * -# take the first bt_iov_t not yet processed
 *    - scanning the bt_http_ecnx_req_t in ecnx_req_fifo order to avoid a starvation.
 * -# merge it with all the contiguous bt_iov_t's from the other requests
 *    - first in decreasing order of ecnx_req_offs
 *    - then  in increasing order of ecnx_req_offs
 * 
 * - NOTE: any bt_iov_t is added in its entirity. aka complete or not at all
 *   - aka no bt_iov_t is splitted in several ones
 */
bt_iov_t	bt_http_ecnx_cnx_t::build_gathered_iov()			const throw()
{
	std::map<file_size_t, bt_http_ecnx_req_t *>::const_iterator	iter;

	// sanity check - m_ecnx_iov MUST be null at this point
	DBG_ASSERT( !m_ecnx_iov );
	
	// sanity check - some bt_http_ecnx_req_t MUST exists
	DBG_ASSERT( !ecnx_req_db.empty() );

	
	// get the first unprocessed bt_iov_t - called the base_ecnx_req
	// - NOTE: it is taken from the ecnx_req_fifo. - it is important in order to process 
	//   all the request in the same order they have been created, to avoid 
	//   the starvation where a given request is delayed for ever.
	const bt_http_ecnx_req_t *	base_ecnx_req	= ecnx_req_fifo.front();
	bt_iov_t			gathered_iov	= base_ecnx_req->get_iov_arr()[0];

	// if the loop ended without finding unprocessed bt_iov_t, return a null one
	// - TODO from the old multiplte bt_http_ecnx_iov_t version. is it still needed
	if( gathered_iov.is_null() )	return bt_iov_t();

	// sanity check - the base_ecnx_req MUST be present in the ecnx_req_offs
	DBG_ASSERT( ecnx_req_offs.find(base_ecnx_req->orig_totfile_beg()) != ecnx_req_offs.end() );
	
	/* 
	 * NOTE: here start the gathering part
	 * - aka trying to merge it with previous and following request
	 * - NOTE: as the bt_iov_arr_t within a ecnx_req is reduced, it is useless 
	 *   to try merging inside it.
	 */

	// loop to gather contiguous bt_iov_t in the decreasing direction within ecnx_req_offs
	iter	= ecnx_req_offs.find(base_ecnx_req->orig_totfile_beg());
	while( true ){
		// if there are no previous element, leave the loop
		if( iter == ecnx_req_offs.begin() )						break;
		// update the iter to get the previous element
		iter--;
		// get the last bt_iov_t from the current bt_
		// - NOTE: as the bt_iov_arr_t is sorted by totfile offset, only test the last bt_iov_t
		const bt_iov_t &other_iov	= iter->second->get_last_iov();
		// if the other_iov IS NOT contiguous to the bt_iov, leave the loop
		if( !other_iov.is_contiguous(gathered_iov) )					break;
		// if the other_iov length would make the outter_req too large, leave the loop
		if( gathered_iov.length() + other_iov.length() > m_profile.outter_req_maxlen())	break;
		// merge the other_iov into the gathered_iov
		gathered_iov.subfile_beg( other_iov.subfile_beg() );
	}

	// loop to gather contiguous bt_iov_t in the increasing direction within ecnx_req_offs
	iter	= ecnx_req_offs.find(base_ecnx_req->orig_totfile_beg());
	while( true ){
		// update the iter to get the next element
		iter++;
		// if the end of the ecnx_req_offs, leave the loop
		if( iter == ecnx_req_offs.end() )						break;
		// get the first bt_iov_t from the current bt_http_ecnx_req_t
		// - NOTE: as the bt_iov_arr_t is sorted by totfile offset, only test the first bt_iov_t
		const bt_iov_t &other_iov	= iter->second->get_first_iov();
		// if the other_iov IS NOT contiguous to the bt_iov, leave the loop
		if( !other_iov.is_contiguous(gathered_iov) )					break;
		// if the other_iov length would make the outter_req too large, leave the loop
		if( gathered_iov.length() + other_iov.length() > m_profile.outter_req_maxlen())	break;
		// merge the other_iov into the bt_iov
		gathered_iov.subfile_end( other_iov.subfile_end() );
	}
	
	// return the result
	return gathered_iov;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  bt_http_ecnx_iov_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_http_ecnx_iov_t when to notify an event
 */
bool	bt_http_ecnx_cnx_t::neoip_bt_http_ecnx_iov_cb(void *cb_userptr, bt_http_ecnx_iov_t &cb_ecnx_iov
					, const bt_err_t &bt_err, const datum_t &recved_data
					, bool cb_is_cnx_close)	throw()
{
	// log to debug
	KLOG_DBG("enter bt_err=" << bt_err);
	// sanity check - the notified bt_http_ecnx_iov_t MUST be the local one
	DBG_ASSERT( m_ecnx_iov == &cb_ecnx_iov );

	// launch the next_iov_zerotimer if not yet done
	// - to launch the bt_http_ecnx_iov_t which may gather several bt_http_ecnx_req_t
	// - NOTE: this is needed if some bt_http_ecnx_req_t remains to be handled and 
	//   if no new ones are inserted
	if( next_iov_zerotimer.empty())	next_iov_zerotimer.append(this, NULL);

	// if the error is at the http level, handle it as such
	if(bt_err==bt_err_t::FROM_HTTP)	return handle_iov_http_error(bt_err);
	// any other type of error (typically at the network level), handle it as a cnx error
	if( bt_err.failed() )		return handle_iov_cnx_error(bt_err);

	// update m_is_cnx_close
	m_is_cnx_close	= cb_is_cnx_close;
	
	// handle the recved_data
	return handle_iov_recved_data(recved_data);
}

bool	bt_http_ecnx_cnx_t::neoip_bt_http_ecnx_iov_progress_chunk_cb(void *cb_userptr
			, bt_http_ecnx_iov_t &cb_ecnx_iov
			, const bt_iov_t &bt_iov, const datum_t &recved_data)	throw()
{
	// log to debug
	KLOG_ERR("enter bt_iov=" << bt_iov);

	// process the data
	bool tokeep	= process_iov_data(bt_iov, recved_data);
	if( !tokeep )	return false;
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  handle result from bt_http_ecnx_iov_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called by the bt_http_ecnx_cnx_t when an error occurs at the connection level
 * 
 * ALGO:
 * - try to find a error handler for the hostport_str
 * - if it doesnt exist yet, create it
 * - notify a cnx error
 * - autodelete this bt_http_ecnx_cnx_t
 * 
 * @return tokeep for the bt_http_ecnx_iov_t
 */
bool	bt_http_ecnx_cnx_t::handle_iov_cnx_error(const bt_err_t &bt_err)	throw()
{
	bt_http_ecnx_herr_t *	ecnx_herr;
	// log to debug
	KLOG_DBG("enter bt_err=" << bt_err);
	
	// try to find a bt_http_ecnx_herr_t for this hostport_str
	ecnx_herr	= m_ecnx_pool->ecnx_herr_by_hostport_str(m_hostport_str);
	// if none exists, create one
	if(!ecnx_herr)	ecnx_herr = nipmem_new bt_http_ecnx_herr_t(m_ecnx_pool, m_hostport_str);

	// autodelete this object
	nipmem_delete	this;

	// notify the error to the ecnx_herr 
	ecnx_herr->notify_cnx_failed(bt_err);

	// return dontkeep
	return false;
}

/** \brief called by the bt_http_ecnx_cnx_t when an error occurs at the http level
 * 
 * ALGO:
 * - try to find a error handler for the hostport_str
 * - if it doesnt exist yet, create it
 * - notify a http error
 * - autodelete this bt_http_ecnx_cnx_t
 * 
 * @return tokeep for the bt_http_ecnx_iov_t
 */
bool	bt_http_ecnx_cnx_t::handle_iov_http_error(const bt_err_t &bt_err)	throw()
{
	const bt_iov_t &recved_iov	= m_ecnx_iov->bt_iov();	
	http_uri_t	http_uri	= subfile_uri_arr[recved_iov.subfile_idx()];
	bt_http_ecnx_herr_t * ecnx_herr;
	// log to debug
	KLOG_DBG("enter bt_err=" << bt_err);

	// sanity check - the http_uri MUST be non null
	DBG_ASSERT( !http_uri.is_null() );

	// try to find a bt_http_ecnx_herr_t for this hostport_str
	ecnx_herr	= m_ecnx_pool->ecnx_herr_by_hostport_str(m_hostport_str);
	// if none exists, create one
	if(!ecnx_herr)	ecnx_herr = nipmem_new bt_http_ecnx_herr_t(m_ecnx_pool, m_hostport_str);

	// autodelete this object
	nipmem_delete	this;

	// notify the error to the ecnx_herr 
	ecnx_herr->notify_http_failed(http_uri, bt_err);

	// return dontkeep
	return false;
}

/** \brief called by the bt_http_ecnx_cnx_t when an error occurs at the file level
 * 
 * @return tokeep for the bt_http_ecnx_iov_t
 */
bool	bt_http_ecnx_cnx_t::handle_iov_recved_data(const datum_t &recved_data)		throw()
{
	// notify the ecnx_herr of that a cnx_succeed
	bt_http_ecnx_herr_t *	ecnx_herr	= m_ecnx_pool->ecnx_herr_by_hostport_str(m_hostport_str);
	if( ecnx_herr )		ecnx_herr->notify_cnx_succeed();

	// update the recv_rate
	// TODO it would be better to be in the process_iov_data because it would
	//      be smoother and thus more accurate over time
	m_recv_rate.update(recved_data.size());
	
	// process the data
	bool tokeep	= process_iov_data(m_ecnx_iov->bt_iov(), recved_data);
	if( !tokeep )	return false;

	// notify an empty BLOCK_REP to trigger the sending of new bt_http_ecnx_req_t
	tokeep		= notify_callback(bt_ecnx_event_t::build_block_rep(NULL, NULL));
	if( !tokeep )	return false;
	// delete the m_ecnx_iov
	nipmem_zdelete	m_ecnx_iov;
	// return dontkeep
	return false;
}

/** \brief Process the data from bt_http_ecnx_iov_t (main callback and progress_chunk)
 */
bool	bt_http_ecnx_cnx_t::process_iov_data(const bt_iov_t &recved_iov
						, const datum_t &recved_data)	throw()
{
	const bt_mfile_t & bt_mfile	= m_ecnx_pool->get_swarm()->get_mfile();
	// sanity check - the recved_data MUST be at long as the bt_iov_t
	KLOG_DBG("recved_data.size() == " << recved_data.size());
	KLOG_DBG("recved_iov=" << recved_iov);
	DBG_ASSERT( recved_data.size() == recved_iov.length().to_size_t() );
	
	// if the ecnx_req_offs is empty at this point, return now
	if( ecnx_req_offs.empty() )	return true;

	// init some variables
	std::map<file_size_t, bt_http_ecnx_req_t *>::iterator	iter_offs;
	std::list<bt_http_ecnx_req_t *>				matching_req_list;
	file_range_t	recved_totfile_range	= recved_iov.totfile_range(bt_mfile);
	// get the first bt_http_ecnx_req_t which may matches recved_data
	iter_offs	= ecnx_req_offs.lower_bound(recved_totfile_range.beg());
	if( iter_offs != ecnx_req_offs.begin() )	iter_offs--;

	// go thru the ecnx_req_offs to build a list of all the bt_http_ecnx_req_t which matches ecnx_iov
	// - NOTE: it is copied in a local list as the bt_http_ecnx_req_t may be deleted 
	//   during the notification (usual walk and delete issue).
	while( iter_offs != ecnx_req_offs.end() ){
		bt_http_ecnx_req_t *	ecnx_req		= iter_offs->second;
		file_range_t		req_totfile_range	= ecnx_req->curr_totfile_range();
		// update the iterator
		iter_offs++;
		// if this bt_http_ecnx_req_t is *after* recved_totfile_range, leave the loop
		if(  req_totfile_range.fully_after(recved_totfile_range) )	break;
		// if this ecnx_req is not fully included in recved_totfile_beg, goto the next
		if( !req_totfile_range.fully_included_in(recved_totfile_range))	continue;
		// put this ecnx_req in the matching_req_list
		matching_req_list.push_back(ecnx_req);
		
		// TODO hand the recv_rate here
		// - will make the estimation smoother and thus more accurate
	}

	// go thru the whole list of interested bt_http_ecnx_req_t to deliver the recved data
	std::list<bt_http_ecnx_req_t *>::iterator	iter_req;
	for(iter_req = matching_req_list.begin(); iter_req != matching_req_list.end(); iter_req++){
		bt_http_ecnx_req_t *	ecnx_req	= *iter_req;
		
		// send the recved_data to the bt_http_ecnx_req_t
		// - NOTE: the tokeep is for bt_http_ecnx_cnx_t as it is its callback which is used
		bool tokeep	= ecnx_req->process_ecnx_iov(recved_iov, recved_data);
		if( !tokeep )	return false;
	}
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_http_ecnx_cnx_t::notify_callback(const bt_ecnx_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_ecnx_vapi_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





