/*! \file
    \brief Class to handle the bt_httpo_full_t
  
\par TODO About piece deleting
- bt_httpo_full_t may handle the piece deletion or not
  - it is tuned by the 'maydel_piece' variable.
- there are several piece deletion strategies
  - one is 'delete the piece as soon as it has been delivered'
    - this one requires to know what has been delivered, so need to be encoded here
    - NOTE: if the first piece available is always the one to deliver, no piece
      will ever be kept available... so no piece to trade with other... an issue
      for bt.
  - one is 'delete the unneeded piece periodically'
    - coded via bt_io_pfile_asyncdel_t
    - this one is interesting in the case of oload, because unneeded piece are kept
      available for a while even when the http client is a video/audio player which 
      are seeking. so small steps forward or backward. thus the piece already downloaded
      before the seeking are not redownloaded
      - ultra shitty explanation
  - one is 'try to delete the piece which is slide_curs()->nb_piece() before the newly avail
    pieceidx'
    - this weird algo ensure to always keep at least slide_curs piece available
    - no matter when the piece are delivered
 
\par Usage
- this is used in neoip-casto and neoip-oload
  - in neoip-casto, this is an endless delivery with range_tosend.end() == file_size_t::MAX
  - in neoip-oload, this is an ended delivery with rend2deliver.end() <= totfile_len

\par About the stopping criteria and has_circularidx
- Depending on the caller, bt_httpo_full_t may or may not have a stopping criteria
  - it is handled by the file_range_t passed in bt_httpo_full_t::start()
  - e.g. if bt_httpo_full_t output a fully qualified range for outputing
    a file, the file_range_t provide the range to deliver and MUST be fully
    included in the bt_mfile_t::totfile_size()
  - e.g. if bt_httpo_full_t output a endless stream, the file_range.end() is 
    file_size_t::MAX. so it will never be reached, bt_httpo_full_t will be endless
- if bt_pselect_slide_arg_t::has_circularidx(), the pieceidx will be circular

\par Note about the delayed start()
- the start() may be done in the same iteration of the event loop as the contructor. 
  or in a different one. This feature is called 'delayed start()'.
- the socket_event_t on the socket_full_t will be handled and if any occurs before
  the start(), the socket_full_t will be deleted and the start() will return an
  error.
- the alternative solution of notifying event is not possible as the callback
  is known only at the start() time

\par About rate limiting
- it is possible to limit the rate at which a bt_httpo_full_t will deliver data
- by default, there are no rate limiting
- this is done by bt_httpo_full_t::xmit_maxrate(maxrate)
- use case: prefetching in neoip-oload
  - create a prefetch bt_httpo_full_t aka a bt_httpo_full_t with xmit_maxrate to 0
    - thus it creates a bt_httpo_full_t which wont deliver any data
  - but the data will be wished so will be downloaded and stored
  - thus when the new bt_httpo_full_t which want the data will get them immediatly
    as they have been prefetched by the prefetch bt_httpo_full_t
- use case: delivering flash video
  - the flash plugin download the video as fast as it can. instead of the usual
    download up to say 100k after the current video position.
  - if the client got a fast connection, it may download the whole movie even 
    if the current video position is at 5% of the movies. 
  - this creates 2 issues:
    1. it is a waste of bandwidth for everybody if the client dont watch the whole movie
    2. with neoip-oload tv, it desynch the bt_httpo_full_t position between the 
       clients with fast connections, and the ones with slower connections.
  - google video uses a bandwidth limiter to reduce the effect of this 'feature'
    in the flash plugin. 
  - bt_httpo_full_t::xmit_maxrate(maxrate) makes it possible to use the same workaround
    while using neoip-oload.

\par About rate limiting - special case of xmit_maxrate == 0 as prefetching tool 
- this is typically used in case of of prefetching in neoip-oload
- it is needed to handle a special case because the rate_limit_t is done at socket level
  - so bt_httpo_full_t will actually write data to the socket
  - the socket will buffer them and deliver them according to rate_limit_t
    - with xmit_maxrate == 0, they are buffered indefinitly
  - so no data is actually delivered during xmit_maxrate == 0... as it is supposed to happen
  - nevertheless bt_httpo_full_t is convinced those data has been delivered and so
    as bt_pieceprec_t::NOTNEEDED in bt_pselect_slide_curs_t
  - consequently those data may be deleted!!!
- as the goal of prefething is to keep the data buffered, to delete them is a bug
- conclusion: a special case is needed in bt_httpo_full_t 
  - to ensure that no data is written in the socket when xmit_maxrate == 0 

\par About supporting http_method_t::HEAD
- http_method_t::GET and http_method_t::HEAD are transparently supported by bt_httpo_*_t 
- algo to support http_method_t::HEAD in bt_httpo_full_t
  - in ::start() dont send any http body after the head and trigger a zerotimer_t
    to notify a bt_httpo_event_t::CNX_CLOSED
- thus the bt_httpo_*_t do not have to be aware of the method 

*/

/* system include */
/* local include */
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_resp.hpp"
#include "neoip_bt_httpo_event.hpp"
#include "neoip_bt_httpo_listener.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_helper.hpp"
#include "neoip_bt_pselect_slide_curs.hpp"
#include "neoip_bt_io_vapi.hpp"
#include "neoip_bt_io_read.hpp"
#include "neoip_bt_prange.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_rate_sched.hpp"
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
bt_httpo_full_t::bt_httpo_full_t(bt_httpo_resp_t *httpo_resp, socket_full_t *socket_full
					, const http_reqhd_t &m_http_reqhd)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->httpo_resp	= httpo_resp;
	this->socket_full	= socket_full;
	this->m_http_reqhd	= m_http_reqhd;
	// link this object to the bt_httpo_listener_t
	httpo_resp->httpo_listener()->full_dolink(this);
	// zero some field
	callback		= NULL;
	bt_swarm		= NULL;
	m_xmit_rsched		= NULL;
	bt_io_read		= NULL;
	m_slide_curs		= NULL;
	m_sent_length		= 0;
	piecedel_in_dtor	= false;
	piecedel_as_delivered	= false;
	piecedel_pre_newlyavail	= false;
	// set socket_full callback
	// - thus if a socket_event_t occurs before it is started, a bt_httpo_event_t will be generated
	// - required to support the delayed start()
	socket_full->set_callback(this, NULL);	
}

/** \brief Destructor
 */
bt_httpo_full_t::~bt_httpo_full_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");

	if( m_slide_curs ){
		// backup the bt_pselect_slide_curs_t pieceidx to be able to try a delete later
		size_t	pieceidx_beg	= slide_curs()->cursidx_to_pieceidx(0);
		size_t	pieceidx_end	= slide_curs()->cursidx_to_pieceidx(slide_curs()->nb_piece()-1);
		// delete the bt_pselect_slide_curs;
		nipmem_zdelete	m_slide_curs;
		// notify the bt_swarm_t of the pieceprec_change 
		// - TODO what about doing this notify_pieceprec_change automatically in 
		//   bt_pselect_vapi_t
		bt_swarm->notify_pieceprec_change();
		// Implement piecedel_in_dtor policy
		// - If maydel_piece is set, try to delete the pieces which were needed for slide_curs 
		if( piecedel_in_dtor ){
			// try to delete all the piece selected only for this bt_httpo_full_t
			for(size_t pieceidx = pieceidx_beg; pieceidx <= pieceidx_end; pieceidx++)
				try_del_piece(pieceidx);
		}
	}
	// unlink it from the bt_httpo_listener_t if needed
	if( httpo_resp )	httpo_resp->httpo_listener()->full_unlink(this);
	// delete the socket_full if needed
	nipmem_zdelete	socket_full;
	// delete the xmit_rsched if needed
	nipmem_zdelete	m_xmit_rsched;
	// delete the bt_io_read_t if needed
	nipmem_zdelete	bt_io_read;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  Setup start
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_httpo_full_t & bt_httpo_full_t::profile(const bt_httpo_full_profile_t &p_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 * 
 * - NOTE: if bt_swarm_t is NULL, it will only send the http_rephd and then close
 *   the connection
 * - WARNING: mutually exclusive between the normal start() and the error start()
 */
bt_err_t	bt_httpo_full_t::start(const bt_httpo_full_arg_t &arg
				, bt_httpo_full_cb_t *callback, void *userptr)	throw()
{	
	// log to debug
	KLOG_DBG("enter");
	KLOG_DBG("param arg.range_tosend=" << arg.range_tosend() << " http_rephd=" << arg.http_rephd());

	// sanity check - this object MUST NOT be already started
	DBG_ASSERT( !is_started() );
	
	// if socket_full is NULL at this point, so it returns an error
	// - NOTE: this is likely due to an socket_event_t occured before the start()
	if( !socket_full )	return bt_err_t::ERROR;
	
	// sanity check - the callback MUST NOT be null
	DBG_ASSERT( callback );

	// copy the parameter
	this->bt_swarm			= arg.bt_swarm();
	this->m_range_tosend		= arg.range_tosend();
	this->piecedel_in_dtor		= arg.piecedel_in_dtor();
	this->piecedel_as_delivered	= arg.piecedel_as_delivered();
	this->piecedel_pre_newlyavail	= arg.piecedel_pre_newlyavail();
	this->callback			= callback;
	this->m_userptr			= userptr;
	
	// sanity check - if has_circularidx, file_range.end() MUST < file_size_t::MAX
	// - as bt_httpo_full_t is supposed to deliver an endless stream
	if(arg.curs_arg().has_circularidx())	DBG_ASSERT( m_range_tosend.end() == file_size_t::MAX);
	// sanity check - if no has_circularidx, file_range.end() MUST be < bt_mfile.totfile_size 
	if(!arg.curs_arg().has_circularidx())	DBG_ASSERT( m_range_tosend.end() < bt_swarm->get_mfile().totfile_size() );
	
	// setup the sendbuf maxlen for the socket_full_t
	socket_full->xmitbuf_maxlen( 50*1024 );
	// set the socket_full->rcvdata_maxlen() to 0 to avoid any reading on the socket
	// - as it is a GET nothing should be read
	// - TODO may be good to put back the socket_full_t in the http_listener after
	//   - then it will be able to keep the connection open and receive multiple requests
#if 0	// TODO disable due to a bug in the error detection
	// - recvdata_maxlen(0) make it no more watch the INPUT and so make it miss
	//   the closing event 
	socket_full->rcvdata_maxlen(0);
#endif
	// set the maysend_threshold
	socket_full->maysend_tshold(socket_full->xmitbuf_maxlen()/2);

	// sanity check - the socket_full_t sendbuf MUST have enougth room to accept the http_rephd_t
	DBG_ASSERT( socket_full->xmitbuf_freelen() >= arg.http_rephd().to_http().size() );
	// send the http_rephd_t thru the socket_full_t
	socket_full->send( datum_t(arg.http_rephd().to_http()) );
	
	KLOG_ERR("http_reqhd=" << http_reqhd() );
	
	// honor the http_method_t::HEAD - see rfc2616.9.4
	// - "The HEAD method is identical to GET except that the server MUST 
	//    NOT return a message-body in the response." - rfc2616.9.4  
	if( m_http_reqhd.method() == http_method_t::HEAD ){
		// launch the zerotimer to delete this object 
		// - NOTE: impossible to do it here, as it is in ::start()
		//   - returning an error is no option, as the caller will interpret it 
		//     as such and may trigger false alarm
		head_autodel_zerotimer.append(this, NULL);
		// return no error
		return bt_err_t::OK; 
	}
	
	// send the header_datum iif present
	// - NOTE: this allows to push a prefix header to this bt_httpo_full_t
	if( arg.header_datum_present() )	socket_full->send( arg.header_datum() );

	// init the xmit_maxrate here if needed - special case for xmit_maxrate == 0
	if( m_profile.xmit_maxrate_thres() == 0 && m_profile.xmit_maxrate() == 0 )
		xmit_maxrate( m_profile.xmit_maxrate() ); 
	
	// set the cursor position
	m_current_pos	= range_tosend().beg();
	// create the bt_pselect_slide_curs_t
	slide_curs_ctor(arg.curs_arg());
	// try to fill the socket sendbuf
	try_fill_xmitbuf();

	// return no error
	return bt_err_t::OK;
}


/** \brief Start the bt_httpo_full_t BUT only to reply an error in the http_rephd_t
 * 
 * - typically used to reply a "404 not found" or similar before deleting bt_httpo_full_t
 * - WARNING: mutually exclusive between the normal start() and the error start()
 *   - aka it is NOT intended to be used to deliver anything after that
 * - NOTE: to ensure that it is not used later, the socket_full_t will be closed
 *   - leaving the 'linger' sending the http_rephd_t data to the remote peer
 */
bt_err_t bt_httpo_full_t::start_reply_error(const http_rephd_t &http_rephd)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - this object MUST NOT be already started
	DBG_ASSERT( !is_started() );
	// if socket_full is NULL at this point, so it returns an error
	// - NOTE: this is likely due to an socket_event_t occured before the start()
	if( !socket_full )	return bt_err_t::ERROR;

	// setup the sendbuf maxlen for the socket_full_t
	socket_full->xmitbuf_maxlen( 50*1024 );
	// sanity check - the socket_full_t sendbuf MUST have enougth room to accept the http_rephd_t
	DBG_ASSERT( socket_full->xmitbuf_freelen() >= http_rephd.to_http().size() );
	// send the http_rephd_t thru the socket_full_t
	socket_full->send( datum_t(http_rephd.to_http()) );
	
	// delete the socket_full to ensure 
	nipmem_zdelete socket_full;
	// sanity check - bt_io_read MUST be NULL at this point
	DBG_ASSERT( !bt_io_read );
	
	// return no error
	return bt_err_t::OK;	
}

/** \brief Shorter Helper on top of bt_httpo_full_t::start_reply_error()
 */
bt_err_t	bt_httpo_full_t::start_reply_error(const http_status_t &status_code
					, const std::string &reason_phrase)	throw()
{
	http_rephd_t	http_rephd; 
	// build the http_rephd_t
	http_rephd.version(http_version_t::V1_1).status_code(status_code.get_value())
				.reason_phrase(reason_phrase);
	// Start the bt_httpo_full_t but in reply error
	return start_reply_error( http_rephd );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			zerotimer callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	bt_httpo_full_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw()
{
	// sanity check - this zerotimer_t MUST be called IIF http_method_t::HEAD 
	DBG_ASSERT( m_http_reqhd.method() == http_method_t::HEAD );
	// notify the cnx_closed to the caller - up to it to delete the bt_httpo_full_t
	return notify_callback( bt_httpo_event_t::build_cnx_closed() );
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief reinit the range_tosend after being start() (IIF no data have been delivered yet)
 * 
 * - this is called *AFTER* the start() and *IIF* sent_length == 0
 *   and *IIF* has_circular_idx()
 * - USAGE: this is usefull to update the bt_httpo_full_t range_tosend at the 
 *   begining of a neoip-casto. 
 *   - as the range to send is always moving (the one from neoip-casti)
 *   - the time to initialize the bt_swarm_t (peersrc, actually connecting remote peer,  
 *     starting downloading data), may be significant.
 *   - so it may be needed to update the range_tosend
 * - NOTE: this is quite kludgy
 */
void	bt_httpo_full_t::reinit_range_tosend(const file_range_t &new_range)	throw()
{
	// sanity check - the sent_length MUST be 0
	DBG_ASSERT( sent_length() == 0 );
	// sanity check - has_circularidx() MUST be true
	DBG_ASSERT( has_circularidx() );
	// sanity check - new_range.end() MUST be file_size_t::MAX (as has_circularidx() is true)
	if( has_circularidx() )	DBG_ASSERT( new_range.end() == file_size_t::MAX);

	// delete the bt_io_read_t if needed
	// - this may happen during a short delay if the first read is in progress 
	nipmem_zdelete	bt_io_read;

	// copy the parameter
	this->m_range_tosend	= new_range;
	// set the cursor position
	m_current_pos		= range_tosend().beg();
	// update the bt_pselect_slide_curs_t offset with the new current_pos
	m_slide_curs->offset	( bt_unit_t::totfile_to_pieceidx(current_pos(), bt_swarm->get_mfile()));
	// notify the bt_swarm_t of the pieceprec change
	bt_swarm->notify_pieceprec_change();
	// try to fill the socket sendbuf
	try_fill_xmitbuf();
	
	// log to debug
	KLOG_ERR("new current_pos=" << current_pos());
	KLOG_ERR("new slide_curs offset=" << m_slide_curs->offset());
}

/** \brief Reinit the http_reqhd_t of this bt_httpo_full_t (valid IIF not yet started)
 * 
 * - this is used in neoip-oload to rewrite the http_reqhd_t depending on
 *   the oload module
 */
bt_httpo_full_t & bt_httpo_full_t::http_reqhd(const http_reqhd_t &new_reqhd)	throw()
{
	// sanity check - the bt_httpo_full_t MUST NOT be started 
	DBG_ASSERT( !is_started() );
	// copy the new value
	m_http_reqhd	= new_reqhd;
	// return the object itself
	return *this;
}

/** \brief Reinit the userptr of this bt_httpo_full_t (valid IIF not yet started)
 * 
 * - this is used in neoip-oload to rewrite the userptr depending on
 *   the oload module. userptr points to the oload_mod_vapi_t
 */
bt_httpo_full_t &	bt_httpo_full_t::userptr(void *new_val)			throw()
{
	// sanity check - the bt_httpo_full_t MUST NOT be started 
	DBG_ASSERT( !is_started() );
	// copy the new value
	m_userptr	= new_val;
	// return the object itself
	return *this;	
}

/** \brief Set the maximum rate at which this bt_httpo_full_t will deliver data
 * 
 * - if maxrate = rate_sched_t::INFINITE_RATE, no rate limit is done
 * - NOTE: maxrate may be 0 if no further data must be delivered
 * - this function MUST be called after the bt_httpo_full_t has been started
 */
bt_httpo_full_t &	bt_httpo_full_t::xmit_maxrate(double maxrate)		throw()
{
	// sanity check - the bt_httpo_full_t MUST be started 
	DBG_ASSERT( is_started() );
	// log to debug
	KLOG_ERR("maxrate=" << maxrate);

	// if maxrate is infinite, disable the rate scheduing
	if( maxrate == rate_sched_t::INFINITE_RATE ){
		// remove the xmit_limit_t from the socket_full if needed
		socket_full->xmit_limit(rate_limit_arg_t());
		// delete the m_xmit_rsched if needed
		nipmem_zdelete	m_xmit_rsched;
		// return the object itself
		return *this;
	}
	
	// create the m_xmit_rsched if needed
	if( !m_xmit_rsched ){
		// start the rate_sched_t with the maxrate parameter
		rate_err_t	rate_err;
		m_xmit_rsched	= nipmem_new rate_sched_t();
		rate_err	= m_xmit_rsched->start();
		DBG_ASSERT( rate_err.succeed() );
		// set the rate_limit_arg_t in the socket_full
		// - as rate_sched_t has only 1 rate_limit_t, the precendence is irrelevant.
		rate_limit_arg_t	rate_limit_arg;
		rate_limit_arg.rate_sched(m_xmit_rsched).rate_prec(rate_prec_t(50));
		socket_full->xmit_limit(rate_limit_arg);
	}
	
	// set the next maxrate
	m_xmit_rsched->max_rate(maxrate);

	// try to fill the socket sendbuf - in case the new maxrate allows new xmit
	try_fill_xmitbuf();

	// return the object itself
	return *this;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  Setup start
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Create the bt_selector_cursor_item_t
 */
bt_err_t bt_httpo_full_t::slide_curs_ctor(const bt_pselect_slide_curs_arg_t &arg)	throw()
{
	// sanity check - the bt_swarm MUST NOT nonnull
	DBG_ASSERT( bt_swarm );
	// sanity check - the arg.offset() MUST be the same pieceidx as the current_pos
	DBG_ASSERT(arg.offset() == bt_unit_t::totfile_to_pieceidx(current_pos(), bt_swarm->get_mfile()));

	// init the bt_pselect_cur_buff_t itself
	bt_pselect_slide_t *	pselect_slide;
	pselect_slide	= bt_swarm_helper_t::get_pselect_slide(bt_swarm);	
	m_slide_curs	= nipmem_new bt_pselect_slide_curs_t(pselect_slide, arg);
	// notify the bt_swarm_t of the pieceprec change
	bt_swarm->notify_pieceprec_change();

	// return no error
	return bt_err_t::OK;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Function to be called when new data are available from the bt_swarm_t
 */
void	bt_httpo_full_t::notify_newly_avail_piece(size_t pieceidx)	throw()
{
	// log to debug
	KLOG_ERR("enter pieceidx=" << pieceidx);

	// implement piecedel_pre_newlyavail policy 
	// - try_delete piece which is slide_curs()->nb_piece before pieceidx
	// - NOTE: this ensure to keep at least slide_curs()->nb_piece available.
	if( piecedel_pre_newlyavail ){
		const bt_mfile_t &bt_mfile	= bt_swarm->get_mfile();
		// compute the pieceidx of the piece to try to delete
		// - NOTE: this works even if has_circularidx()
		size_t		pieceidx_to_del	= (bt_mfile.nb_piece() + pieceidx 
					- slide_curs()->nb_piece()) % bt_mfile.nb_piece();
		try_del_piece( pieceidx_to_del );
	}

	// try to fill the socket sendbuf
	try_fill_xmitbuf();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t when a connection is established
 */
bool	bt_httpo_full_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );
	
	// if any socket_event_t occurs before is_started(), delete the socket_full
	// - NOTE: handled the delayed start() feature
	if( !is_started() ){
		KLOG_INFO("an socket_event_t " << socket_event << " occured before start(). closing the socket.");
		nipmem_zdelete	socket_full;
		// sanity check - bt_io_read MUST be NULL at this point
		DBG_ASSERT( !bt_io_read );
		return false;
	}

	// handle the fatal events
	// - TODO cnx_closed is not explicit way to say error. moreover no way to 
	//   know the error. well put a bt_err_t in the cnx_closed
	if( socket_event.is_fatal() )
		return notify_callback( bt_httpo_event_t::build_cnx_closed() );

	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:
			// this event should not happen as recv_max_len_set is 0
			KLOG_ERR("enter event=" << *socket_event.get_recved_data());
			//DBGNET_ASSERT( 0 );
			// TODO else do what ? auto delete ?
			break;
	case socket_event_t::MAYSEND_ON:
			// if MAYSEND_ON occurs and the maysend_threashold is == sendbuf maxlen, this is
			// the end of the connection, so notify a CNX_CLOSED
			// - this is a kludge to implement a weird linger... to fix
			if( socket_full->maysend_tshold() == socket_full->xmitbuf_maxlen() )
				return notify_callback( bt_httpo_event_t::build_cnx_closed() );
			// try to full the socket_full sendbuf
			try_fill_xmitbuf();
			break;
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Try to fill the send buf
 */
void	bt_httpo_full_t::try_fill_xmitbuf()	throw()
{
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	// sanity check - the socket_full MUST be init
	DBG_ASSERT( socket_full );
	// sanity check - the bt_swarm_t MUST be non null
	DBG_ASSERT( bt_swarm );
	// sanity check - if http_reqhd is http_method_t::HEAD, no 'body' should be written
	DBG_ASSERT( m_http_reqhd.method() != http_method_t::HEAD );
	// if there is already a io_pfile_read in progress, return now
	if( bt_io_read )			return;

	// if m_xmit_rsched->max_rate() is 0, no need to try adding data to the xmitbuf
	// - special case for xmit_maxrate == 0
	if( m_xmit_rsched && m_xmit_rsched->max_rate() == 0.0 )	return;

	// get the file_range_t to read
	file_range_t	totfile_range	= range_to_read();
	// if the returned totfile_range is null, return now
	if( totfile_range.is_null() )		return;
	
	// sanity check - totfile_range MUST be fully included in bt_mfile.totfile_range()
	DBG_ASSERT( totfile_range.fully_included_in(bt_mfile.totfile_range()) );
	// sanity check - totfile_range MUST be <= the socket xmitbuf_freelen
	DBG_ASSERT( totfile_range.length() <= socket_full->xmitbuf_freelen() );
	
	// log to debug
	KLOG_DBG("read totfile_range=" << totfile_range);
	
	// launch the bt_io_read for the chunk
	bt_io_vapi_t *	bt_io_vapi	= bt_swarm->io_vapi();
	bt_io_read	= bt_io_vapi->read_ctor(totfile_range, this, NULL);
}


/** \brief Return the file_range_t to read - may return null if no data are available
 * 
 * - even in case of has_circularidx, the returned file_range_t.beg is always < than end()
 */
file_range_t	bt_httpo_full_t::range_to_read()	const throw()
{
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	const bt_pieceavail_t &	pieceavail	= bt_swarm->local_pavail();
	file_size_t		chunk_beg	= current_pos();
	file_size_t		chunk_end	= m_range_tosend.end();
	// log to debug
	KLOG_ERR("enter chunk_beg=" << chunk_beg << " chunk_end=" << chunk_end);
	// sanity check - bt_io_read_t MUST NOT be in progress when this function is called
	DBG_ASSERT( bt_io_read == NULL );

	// if the socket_full_t xmitbuf has no freespace, return now
	if( socket_full->xmitbuf_freelen() == 0 )		return file_range_t();

	// if the chunk_beg is > than chunk_end, return now
	// - special case which means that all data has been delivered
	// - happen IIF not has_circularidx()
	if( chunk_beg > chunk_end )				return file_range_t();

	// compute pieceidx_beg of the chunk_beg
	size_t	pieceidx_beg	= bt_unit_t::totfile_to_pieceidx(chunk_beg, bt_mfile);
	// if pieceidx_beg is not available, return a null file_range_t
	if( !pieceavail.is_avail(pieceidx_beg) )		return file_range_t();
	
	// clamp the chunk_end with bt_mfile.totfile_size()
	// - usefull in case of has_circular_idx(), where m_range_tosend.end()==file_size_t::MAX
	chunk_end = std::min( chunk_end, bt_mfile.totfile_size()-1);

	// Compute pieceidx_end - the pieceidx of the last piece available
	size_t	pieceidx_max	= bt_unit_t::totfile_to_pieceidx(chunk_end, bt_mfile);
	size_t	pieceidx_end	= 0;
	for(size_t i = pieceidx_beg; i <= pieceidx_max; i++){
		if( pieceavail.is_unavail(i) )	break; 
		pieceidx_end	= i;
	}
	// clamp chunk_end to be the end of the last available pieceidx
	chunk_end	= std::min( chunk_end, bt_unit_t::pieceidx_to_pieceend(pieceidx_end, bt_mfile) );
	// clamp the chunk_end with socket_full->xmitbuf_freelen()
	if( chunk_end-chunk_beg+1 > socket_full->xmitbuf_freelen() )
		chunk_end	= chunk_beg + socket_full->xmitbuf_freelen() - 1;

	// log to debug
	KLOG_ERR("leave chunk_beg=" << chunk_beg << " chunk_end=" << chunk_end);

	// return the file_range
	return file_range_t(chunk_beg, chunk_end);
}

/** \brief Try delete a given piece (done synchronously)
 * 
 * - if this pieceidx is still needed, it wont delete the pieceidx
 *   - it may happen if it is still needed by another bt_pselect_slide_curs_t
 * 
 * @return a tokeep for the bt_httpo_full_t - this function may notify event
 *         in case of disk error
 */
void	bt_httpo_full_t::try_del_piece(size_t pieceidx)	throw()
{
	bt_pselect_vapi_t *	pselect_vapi	= bt_swarm->pselect_vapi();
	const bt_pieceavail_t &	pieceavail	= bt_swarm->local_pavail();
	// log to debug
	KLOG_DBG("enter pieceidx=" << pieceidx << " pieceprec=" << pselect_vapi->pieceprec(pieceidx));
	// if this pieceidx is still needed, do nothing
	if( pselect_vapi->pieceprec(pieceidx).is_needed() )	return;
	// if the pieceidx is not locally available, do nothing
	if( pieceavail.is_unavail(pieceidx) )			return;
	// notify the bt_swarm_t that this piece is no more available
	bt_swarm->declare_piece_nomore_avail(pieceidx);
}

/** \brief Return true if bt_httpo_full_t must handle has_circular_idx
 * 
 * - take it directly from bt_pselect_slide_curs_t
 */
bool	bt_httpo_full_t::has_circularidx()		const throw()
{
	// sanity check - bt_pselect_slide_curs_t MUST be non null
	DBG_ASSERT( slide_curs() );
	// return the value directory from slide_curs
	return slide_curs()->has_circularidx();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       bt_io_read_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_io_read_t when to notify the result of the operation
 * 
 * - TODO this function is a mess, reorganize it
 */
bool 	bt_httpo_full_t::neoip_bt_io_read_cb(void *cb_userptr, bt_io_read_t &cb_bt_io_read
				, const bt_err_t &read_err, const datum_t &read_data)	throw()
{
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	socket_err_t		socket_err;
	// log to debug
	KLOG_DBG("enter bt_err=" << read_err << " read_data.size=" << read_data.size() );
	KLOG_DBG("read_data=" << read_data );
	// sanity check - if http_reqhd is http_method_t::HEAD, no 'body' should be written
	DBG_ASSERT( m_http_reqhd.method() != http_method_t::HEAD );
	// sanity check - socket_full MUST NOT be NULL
	DBG_ASSERT( socket_full );
	// sanity check - the socket_full_t sendbuf MUST have enougth room to accept the read_data
	DBG_ASSERT( socket_full->xmitbuf_freelen() >= read_data.size() );
	
	// if the bt_io_read_t report an error, log the event and notify the caller
	if( read_err.failed() ){
		// log the event as this should not happen because the data where supposed to be available
		KLOG_ERR("Unable to read available content from bt_io_vapi_t. closing the connection.");
		// notify the caller
		// TODO this is improper, a read_err failed SHOULD NOT happen
		// - aka if it happen something went seriously wrong
		// - add an event DISK_ERROR in bt_httpo_event_t
		return notify_callback( bt_httpo_event_t::build_cnx_closed() );	
	}

	// init the xmit_maxrate here if needed
	// - if m_sent_length is going across the m_profile.xmit_maxrate_thres()
	// - if m_xmit_rsched is not yet initialized. thus if the caller already 
	//   set up its own before it gets precedence.
	const file_size_t &	maxrate_thres	= m_profile.xmit_maxrate_thres();
	if( m_sent_length <= maxrate_thres && m_sent_length+read_data.size() > maxrate_thres
			&& m_profile.xmit_maxrate() != rate_sched_t::INFINITE_RATE
			&& m_xmit_rsched == NULL ){
		this->xmit_maxrate( m_profile.xmit_maxrate() ); 
	}
	
	// update the sent_length
	m_sent_length	+= read_data.size();

	// send the data to the connection
	socket_err = socket_full->send( read_data );
	DBG_ASSERT( socket_err.succeed() );

	// compute the piece_idx for the current_pos *before* update
	size_t	old_pieceidx	= bt_unit_t::totfile_to_pieceidx(current_pos(), bt_mfile);
	// sanity check - the slide_curs()->offset MUST be equal of the pieceidx for current_pos()
	DBG_ASSERT( slide_curs()->offset() == old_pieceidx );

	// update the current_pos with the data which has just been sent
	m_current_pos	+= read_data.size();	
	// if has_circularidx(), modularize current_pos with bt_mfile.totfile_size
	// - thus it can warparound
	if( has_circularidx() )	m_current_pos	%= bt_mfile.totfile_size(); 
	// if this bt_httpo_full_t has reached the end, init a weird linger (FIXME)
	// - set the maysend threshold to be notified when the sendbuf is empty
	// - NOTE: if file_range.end() is file_size_t::MAX, it will never be reached
	//   and so bt_httpo_full_t will never stop.
	//   - this is typically the case with has_circularidx()
	if( current_pos() > m_range_tosend.end() )
		socket_full->maysend_tshold(socket_full->xmitbuf_maxlen());

	// compute the piece_idx for the current_pos *after* update
	size_t	new_pieceidx	= bt_unit_t::totfile_to_pieceidx(current_pos(), bt_mfile);

	// if the piece_idx has changed and not all data have been recved, update the slide_curs
	if( old_pieceidx != new_pieceidx && current_pos() <= m_range_tosend.end()){
		// update the slide_curs
		slide_curs()->offset( new_pieceidx );
		// notify the bt_swarm_t of the pieceprec change
		bt_swarm->notify_pieceprec_change();

		// implement piecedel_as_delivered policy
		// - try to delete all pieceidx between old_pieceidx and new_pieceidx
		// - MUST be done after the slide_curs()->offset update
		// - IIF maydel_piece is set
		if( piecedel_as_delivered ){
			for(size_t i = old_pieceidx; i < new_pieceidx; i++)
				try_del_piece(i);
		}
		// log to debug
		KLOG_ERR("old_pieceidx=" << old_pieceidx << " new_pieceidx=" << new_pieceidx);
		KLOG_DBG("slide_curs=" << *slide_curs);
	}

	// delete the bt_io_read
	nipmem_zdelete	bt_io_read;
	// try again to fill the sendbuf
	try_fill_xmitbuf();
	// return dontkeep - as it has just been deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_httpo_full_t::notify_callback(const bt_httpo_event_t &cnx_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_httpo_full_cb(userptr(), *this, cnx_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





