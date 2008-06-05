/*! \file
    \brief Class to handle the bt_peersrc_utpex_t

\par Brief Description
\rev bt_peersrc_utpex_t handle the utpex, aka using other peers as peersrc.
- it registers as bt_peersrc_vapi_t to notify the peers learned
- it registers as bt_utmsg_vapi_t to recv/xmit utpex packets

- TODO some issue when the other peers doesnt have a tcp_listen_addr
  - it may not be provided in the utmsg handshake
  - this cause some sanity check error here, to be handled
  - i dunno where to handle it, likely in moment to add/del in the log

\par TODO renaming
- note about renaming
  - bt_protoflag_t::UT_MSGPROTO
  - bt_cmdtype_t::UTMSG_PAYL
  - bt_utmsgtype_t::HANDSHAKE
  - bt_swarm_full_utmsg_t
- test with ut extension
  - test of what it would give
    bt_protoflag_t::UT_EXTPROTO
    bt_cmdtype_t::UTEXT_PAYL
    bt_utextpayl_t::HANDSHAKE
    bt_swarm_full_utext_t
  - the consistency is good
  - the issue is that 'utext' may be read 'u-text' instead of 'ut-ext'
- utpkt <- avoid the double read and so confusion but it is not really a packet
- utmsg <- the current but it is not really a msg either
- utext <- confusion due to double read but it IS the 'utorrent extension'

*/

/* system include */
/* local include */
#include "neoip_bt_peersrc_utpex.hpp"
#include "neoip_bt_peersrc_utpex_cnx.hpp"
#include "neoip_bt_peersrc_event.hpp"
#include "neoip_bt_peersrc_helper.hpp"
#include "neoip_bt_peersrc_peer_arr.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_utmsg_event.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_bencode.hpp"
#include "neoip_dvar.hpp"
#include "neoip_pkt.hpp"
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
bt_peersrc_utpex_t::bt_peersrc_utpex_t()		throw()
{
	// zero some fields
	bt_swarm	= NULL;
	peersrc_cb	= NULL;
	utmsg_cb	= NULL;
	next_seqnb	= 1;
}

/** \brief Destructor
 */
bt_peersrc_utpex_t::~bt_peersrc_utpex_t()		throw()
{
	// unregister this object
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	if( peersrc_cb ){
		bool	tokeep	= notify_peersrc_cb( bt_peersrc_event_t::build_unregister() );
		DBG_ASSERT( tokeep == true );
	}
	// unregister this object
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	if( utmsg_cb ){
		bool	tokeep	= notify_utmsg_cb( bt_utmsg_event_t::build_unregister() );
		DBG_ASSERT( tokeep == true );
	}
	// close all pending bt_peersrc_utpex_cnx_t
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_peersrc_utpex_t &bt_peersrc_utpex_t::set_profile(const bt_peersrc_utpex_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_peersrc_utpex_t::start(bt_swarm_t *bt_swarm, bt_peersrc_cb_t *peersrc_cb
					, bt_utmsg_cb_t *utmsg_cb, void *userptr)	throw()
{
	bool	tokeep;
	// copy the parameter
	this->bt_swarm	= bt_swarm;
	this->peersrc_cb= peersrc_cb;
	this->utmsg_cb	= utmsg_cb;
	this->userptr	= userptr;
	// register this peersrc to the callback
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return dontkeep
	tokeep	= notify_peersrc_cb( bt_peersrc_event_t::build_doregister() );
	DBG_ASSERT( tokeep == true );
	// register this utmsg to the callback
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return dontkeep
	tokeep	= notify_utmsg_cb( bt_utmsg_event_t::build_doregister() );
	DBG_ASSERT( tokeep == true );
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_utmsg_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Called to notify a bt_swarm_full_t opened
 */
bt_utmsg_cnx_vapi_t *	bt_peersrc_utpex_t::cnx_ctor(bt_swarm_full_utmsg_t *full_utmsg)	throw()
{
	bt_err_t	bt_err;
	// create a bt_peersrc_utpex_cnx_t for this bt_swarm_full_utmsg_t
	bt_peersrc_utpex_cnx_t *utpex_cnx;
	utpex_cnx	= nipmem_new bt_peersrc_utpex_cnx_t();
	bt_err		= utpex_cnx->start(this, full_utmsg);
	DBG_ASSERT( bt_err.succeed() );
	// return the just built bt_utmsg_cnx_vapi_t
	return utpex_cnx;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_peersrc_utpex_cnx_t management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify a utmsg packet
 * 
 * @return a bt_err_t if an error occurs
 */
bool	bt_peersrc_utpex_t::parse_pkt(bt_peersrc_utpex_cnx_t *utpex_cnx
			, const bt_utmsgtype_t &bt_utmsgtype, const pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter bt_utmsgtype=" << bt_utmsgtype << " pkt=" << pkt);
	// sanity check - the bt_utmsgtype MUST be handlded
	DBG_ASSERT( utmsgtype() == bt_utmsgtype );

	// convert the utpex message body to a dvar_t
	dvar_t	utpex_dvar	= bencode_t::to_dvar(pkt.to_datum(datum_t::NOCOPY));

	// if the handshake_dvar failed to parse the bencoded data, return an error
	if( utpex_dvar.is_null() || utpex_dvar.type() != dvar_type_t::MAP )
		return parsing_error("unable to find utpex dvar_type_t::MAP");

	// get the "added" peers
	bt_peersrc_peer_arr_t	opened_peer_arr;
	if( utpex_dvar.map().contain("added", dvar_type_t::STRING) ){
		const std::string &	added_str = utpex_dvar.map()["added"].str().get();
		opened_peer_arr	= bt_peersrc_helper_t::peer_arr_from_compactfmt(added_str);
		// parse the "added.f" peerflag for the "added" peers
		if( utpex_dvar.map().contain("added.f", dvar_type_t::STRING) ){
			const std::string &	addedf_str = utpex_dvar.map()["added.f"].str().get();
			// update the opened_peer_arr with the peerflag
			bt_peersrc_helper_t::peer_arr_from_peerflag(opened_peer_arr, addedf_str);
		}
	}

	// get the "dropped" peers
	// - NOTE: currently nobody uses the dropped entry
	bt_peersrc_peer_arr_t	closed_peer_arr;
	if( utpex_dvar.map().contain("dropped", dvar_type_t::STRING) ){
		const std::string &	dropped_str = utpex_dvar.map()["dropped"].str().get();
		closed_peer_arr	= bt_peersrc_helper_t::peer_arr_from_compactfmt(dropped_str);
	}

	// log to debug
	KLOG_DBG("opened_peer_arr=" << opened_peer_arr);
	KLOG_DBG("closed_peer_arr=" << closed_peer_arr);

	// feed the bt_peersrc_cb_t with the opened_peer_arr
	for(size_t i = 0; i < opened_peer_arr.size(); i++){
		bt_peersrc_peer_t peersrc_peer	= opened_peer_arr[i];
		// notify the bt_peersrc_vapi_t of the new peers
		// - NOTE: so this tokeep MUST NOT be echoed to the bt_swarm_full_t
		bool	tokeep	= notify_peersrc_cb(bt_peersrc_event_t::build_new_peer(&peersrc_peer));
		DBG_ASSERT( tokeep );
	}

	// return noerror
	return bt_err_t::OK;
}

/** \brief function to call in case of error while parsing incoming payload
 */
bool	bt_peersrc_utpex_t::parsing_error(const std::string &reason)	throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR(reason);
	// return tokeep
	return true;
}

/** \brief Called to notify a bt_swarm_full_t opened
 */
void	bt_peersrc_utpex_t::cnx_dolink(bt_peersrc_utpex_cnx_t *cnx)	throw()
{
	bt_swarm_full_utmsg_t *	full_utmsg	= cnx->full_utmsg;
	// add it to the database
	cnx_db.push_back(cnx);
	// log to debug
	KLOG_WARN("enter OPENED bt_peersrc_peer_t=" << full_utmsg->to_peersrc_peer() );
	
	// log the event in log_opened
	if( full_utmsg->tcp_listen_ipport().is_fully_qualified() ){
		bt_peersrc_peer_t	peersrc_peer	= full_utmsg->to_peersrc_peer();		
		// insert it in log_opened
		bool 	succeed	= log_opened.insert(std::make_pair(next_seqnb, peersrc_peer)).second;
		DBG_ASSERT( succeed );
		// increment the next_seqnb
		next_seqnb++;
	}
}

/** \brief Called to notify a bt_swarm_full_t closed
 */
void	bt_peersrc_utpex_t::cnx_unlink(bt_peersrc_utpex_cnx_t *cnx)	throw()
{
	bt_swarm_full_utmsg_t *	full_utmsg	= cnx->full_utmsg;
	// log to debug
	KLOG_WARN("enter CLOSED bt_peersrc_peer_t=" << full_utmsg->to_peersrc_peer() );

	// log the event in log_closed
	if( full_utmsg->tcp_listen_ipport().is_fully_qualified() ){
		bt_peersrc_peer_t	peersrc_peer	= full_utmsg->to_peersrc_peer();		
		// insert it in log_closed
		bool 	succeed	= log_closed.insert(std::make_pair(next_seqnb, peersrc_peer)).second;
		DBG_ASSERT( succeed );
		// increment the next_seqnb
		next_seqnb++;
	}
	
	// remote it from the database
	cnx_db.remove(cnx);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      query the log
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the opened_arr/closed_arr from the 'from_seqnb'
 * 
 * @param opened_arr	this is a output parameter containing the opened ipport_addr_t from from_seqnb
 * @param closed_arr	this is a output parameter containing the closed ipport_addr_t from from_seqnb
 * 
 * @return the 'from_seqnb' for the next call
 */
uint32_t	bt_peersrc_utpex_t::get_log_main(uint32_t from_seqnb, bt_peersrc_peer_arr_t &opened_arr
						, bt_peersrc_peer_arr_t &closed_arr)	const throw()
{
	// if the dirst_seqnb == 0, this is a special case, report the initial bt_peersrc_peer_arr_t's
	if( from_seqnb == 0 )	return get_log_init(from_seqnb, opened_arr, closed_arr);
	// else return the difference
	return get_log_diff(from_seqnb, opened_arr, closed_arr);
}

/** \brief return the opened_arr/closed_arr from the 'from_seqnb'
 * 
 * - it uses the bt_peersrc_utpex_cnx_t as a full set MUST be sent aka not the logged event
 */
uint32_t	bt_peersrc_utpex_t::get_log_init(uint32_t from_seqnb, bt_peersrc_peer_arr_t &opened_arr
						, bt_peersrc_peer_arr_t &closed_arr)	const throw()
{
	// sanity check - the from_seqnb MUST == 0 - else get_log_diff() is the one to be used
	DBG_ASSERT( from_seqnb == 0 );
	// fill the opened_arr with the tcp_listen_ipport of all the bt_peex_utpex_cnx_t
	std::list<bt_peersrc_utpex_cnx_t *>::const_iterator	iter;
	for(iter = cnx_db.begin(); iter != cnx_db.end(); iter++){
		const bt_peersrc_utpex_cnx_t *	utpex_cnx	= *iter;
		const bt_swarm_full_utmsg_t *	full_utmsg	= utpex_cnx->full_utmsg;
		bt_peersrc_peer_t		peersrc_peer	= full_utmsg->to_peersrc_peer();
		// if bt_peersrc_peer_t is null, goto the next
		if( peersrc_peer.is_null() )	continue;
		// add the bt_swarm_full_utmsg_t bt_peersrc_peer_t to the opened_arr
		opened_arr	+= peersrc_peer;
	}
	// log to debug
	KLOG_DBG("from_seqnb=" << from_seqnb << " opened_arr=" << opened_arr << " closed_arr=" << closed_arr);
	// return the next_seqnb
	return next_seqnb;
}

/** \brief return the opened_arr/closed_arr from the 'from_seqnb'
 * 
 * - it uses the log_opened/log_closed to provide the difference
 */
uint32_t	bt_peersrc_utpex_t::get_log_diff(uint32_t from_seqnb, bt_peersrc_peer_arr_t &opened_arr
						, bt_peersrc_peer_arr_t &closed_arr)	const throw()
{
	peer_log_t::const_iterator	iter;
	// sanity check - the from_seqnb MUST != 0  - else get_log_diff() is the one to be used
	DBG_ASSERT( from_seqnb != 0 );
	// populate the opened_arr with any entry of log_opened which has a seqnb >= from_seqnb
	for(iter = log_opened.lower_bound(from_seqnb); iter != log_opened.end(); iter++){
		const bt_peersrc_peer_t & peersrc_peer	= iter->second;
		// add this bt_peersrc_peer_t in the opened_arr
		opened_arr	+= peersrc_peer;
	}
	// populate the closed_arr with any entry of log_closed which has a seqnb >= from_seqnb
	for(iter = log_closed.lower_bound(from_seqnb); iter != log_closed.end(); iter++){
		const bt_peersrc_peer_t & peersrc_peer	= iter->second;
		// if this bt_peersrc_peer_t is already in opened_arr, this is a special case
		// - this means its has been opened and closed between the 2 utpex messages
		size_t	opened_arr_idx	= opened_arr.find(peersrc_peer);
		if( opened_arr_idx != std::numeric_limits<size_t>::max() ){
			// remove it from the opened_arr
			opened_arr.remove(opened_arr_idx);
			// dont put it in closed_arr
			continue;
		}
		// add this bt_peersrc_peer_t in the closed_arr
		closed_arr	+= peersrc_peer;
	}
	// log to debug
	KLOG_DBG("from_seqnb=" << from_seqnb << " opened_arr=" << opened_arr
				<< " closed_arr=" << closed_arr);
	// return the next_seqnb
	return next_seqnb;
}


/** \brief Purge the log if needed
 */
void	bt_peersrc_utpex_t::purge_log_if_needed()				throw()
{
	uint32_t min_seqnb	= std::numeric_limits<uint32_t>::max();

	// compute the minimal next_seqnb among all the the bt_peersrc_utpex_cnx_t
	std::list<bt_peersrc_utpex_cnx_t *>::iterator	iter;
	for(iter = cnx_db.begin(); iter != cnx_db.end(); iter++){
		bt_peersrc_utpex_cnx_t *utpex_cnx	= *iter;
		uint32_t		cnx_seqnb	= utpex_cnx->get_next_seqnb();
		// if cnx_seqnb is 0, dont take it into account
		if( cnx_seqnb == 0 )	continue;
		// update the min_seqnb
		min_seqnb	= std::min(min_seqnb, cnx_seqnb);
	}
	// remove in the log_opened all the events with seqnb < min_seqnb 
	while( !log_opened.empty() ){
		// if the seqnb of this event is >= to the min_seqnb, leave the loop
		if( log_opened.begin()->first >= min_seqnb )	break;
		// remove this event
		log_opened.erase(log_opened.begin());
	}
	
	// remove in the log_closed all the events with seqnb < min_seqnb 
	while( !log_closed.empty() ){
		// if the seqnb of this event is >= to the min_seqnb, leave the loop
		if( log_closed.begin()->first >= min_seqnb )	break;
		// remove this event
		log_closed.erase(log_closed.begin());
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the bt_peersrc_event_t
 */
bool bt_peersrc_utpex_t::notify_peersrc_cb(const bt_peersrc_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( peersrc_cb );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*peersrc_cb);
	// notify the caller
	bool tokeep = peersrc_cb->neoip_bt_peersrc_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

/** \brief notify the callback with the bt_utmsg_event_t
 */
bool bt_peersrc_utpex_t::notify_utmsg_cb(const bt_utmsg_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( utmsg_cb );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*utmsg_cb);
	// notify the caller
	bool tokeep = utmsg_cb->neoip_bt_utmsg_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





