/*! \file
    \brief Class to handle the bt_swarm_full_t

- TODO i think there is a bug in the local_dowant_req
  - it is computed only on the local pieceavail, assuming we want to download 
    all pieces... but the caller may not be interested in some piece.
    like download of only part of the file
  - fix this
  - it requires to know if we want to download the piece aka to know its precedence
    - so it needs to be updated when the the pieceprec_arr is changed


*/

/* system include */
/* local include */
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_prec.hpp"
#include "neoip_bt_swarm_full_sched.hpp"
#include "neoip_bt_swarm_full_sendq.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_swarm_full_event.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_pieceavail.hpp"
#include "neoip_bt_handshake.hpp"
#include "neoip_bt_cmdtype.hpp"
#include "neoip_bt_utmsgtype.hpp"
#include "neoip_bt_utmsg_punavail_cnx.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_bt_swarm_sched.hpp"		// only to get the bt_swarm_full_cb_t callback
#include "neoip_bt_swarm_sched_full.hpp"	// only to get the bt_swarm_full_cb_t callback
#include "neoip_serial_stat.hpp"


NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_swarm_full_t::bt_swarm_full_t()		throw()
{
	// zero some fields
	bt_swarm	= NULL;
	socket_full	= NULL;
	sendq		= NULL;
	callback	= NULL;
	m_full_prec	= NULL;
	m_full_sched	= NULL;
	m_full_utmsg	= NULL;
	// set some fields
	m_local_doauth_req	= false;
	m_other_doauth_req	= false;
	m_local_dowant_req	= false;
	m_other_dowant_req	= false;
	notified_as_open	= false;
}

/** \brief Destructor
 */
bt_swarm_full_t::~bt_swarm_full_t()		throw()
{
#if 1	// NOTE: move to dtor - but afraid of the nested notification
	// - so i leave it there 
	// if notified_as_open, notify the event bt_swarm_full_event_t::CNX_CLOSED
	if( notified_as_open ){
		bool	tokeep	= notify_callback(bt_swarm_full_event_t::build_cnx_closed());
		DBG_ASSERT( tokeep );
	}
#endif
	// unlink this object from the bt_swarm
	if( bt_swarm )	bt_swarm->full_unlink(this);
	// delete the bt_swarm_full_prec_t if needed
	nipmem_zdelete	m_full_prec;
	// delete the bt_swarm_full_sched_t if needed
	nipmem_zdelete	m_full_sched;
	// delete the bt_swarm_full_utmsg_t if needed
	nipmem_zdelete	m_full_utmsg;
	// delete the sendq_t if needed
	nipmem_zdelete	sendq;
	// delete the socket_full_t if needed
	nipmem_zdelete	socket_full;
}

/** \brief Autodelete this object - aka notify CNX_CLOSED to the scheduler and then delete the object
 */
bool	bt_swarm_full_t::autodelete()	throw()
{
#if 0	// NOTE: move to dtor - but afraid of the nested notification
	// - so i leave it there 
	// if notified_as_open, notify the event bt_swarm_full_event_t::CNX_CLOSED
	if( notified_as_open ){
		bool	tokeep	= notify_callback(bt_swarm_full_event_t::build_cnx_closed());
		if( !tokeep )	return false;
	}
#endif
	// autodelete itself
	nipmem_delete	this;
	// return false - to 'simulate' a dontkeep
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_swarm_full_t::start(bt_swarm_t *bt_swarm, socket_full_t *socket_full
					, const bytearray_t &recved_data
					, const bt_handshake_t &remote_handshake)	throw()
{
	const bt_swarm_profile_t &profile	= bt_swarm->profile();
	const bt_mfile_t &	bt_mfile	= bt_swarm->bt_mfile;
	// log to debug
	KLOG_DBG("enter");
	// sanity check - no other bt_swarm_full_t MUST have the same peerid
	DBG_ASSERT( !bt_swarm->full_by_remote_peerid(remote_handshake.peerid()) );
	// copy the parameter
	this->recved_data	= recved_data;
	this->bt_swarm		= bt_swarm;
	this->m_remote_peerid	= remote_handshake.peerid();
	this->remote_protoflag	= remote_handshake.protoflag();
	// link this object to the bt_swarm
	bt_swarm->full_dolink(this);
	// init the recv_rate/xmit_rate estimator
	m_recv_rate		= rate_estim_t<size_t>(profile.full_rate_estim_arg());
	m_xmit_rate		= rate_estim_t<size_t>(profile.full_rate_estim_arg());
	// set up the socket_full_t
	this->socket_full	= socket_full;
	socket_full->set_callback(this, NULL);
	if( profile.xmit_limit_arg().is_valid() )	socket_full->xmit_limit(profile.xmit_limit_arg());
	if( profile.recv_limit_arg().is_valid() )	socket_full->recv_limit(profile.recv_limit_arg());
	// set the zerotimer to parse the just recved_data
	first_parse_zerotimer.append(this, NULL);
	// initialize the bt_pieceavail_t - full of zero
	m_remote_pavail		= bt_pieceavail_t(bt_mfile.nb_piece());
	// build the m_remote_pwish bitfield_t - the opposite of pieceavail
	m_remote_pwish		= ~remote_pavail();
	// initialize the sendq_t
	sendq			= nipmem_new bt_swarm_full_sendq_t(this);
	// initialize the bt_swarm_full_prec_t
	m_full_prec		= nipmem_new bt_swarm_full_prec_t(this);
	// initialize the bt_swarm_full_utmsg_t
	m_full_utmsg		= nipmem_new bt_swarm_full_utmsg_t(this);
	// start the idle_timeout
	idle_timeout.start(profile.full_idle_timeout(), this, NULL);
	// hardcode the callback to the bt_swarm_sched_full_t in bt_swarm_t
	callback		= bt_swarm->swarm_sched()->sched_full();
	userptr			= NULL;
	// initialize the bt_swarm_full_sched_t
	m_full_sched		= nipmem_new bt_swarm_full_sched_t(this);
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the remote socket_addr_t of this bt_swarm_full_t
 */
const socket_addr_t &	bt_swarm_full_t::remote_addr()	const throw()
{
	return socket_full->remote_addr();
}

/** \brief Return the bt_peersrc_peer_t from this bt_swarm_full_t
 */
bt_peersrc_peer_t	bt_swarm_full_t::peersrc_peer()	const throw()
{
	return bt_peersrc_peer_t(remote_peerid(), remote_addr());
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief set the local doauth_req value
 * 
 * - likely to be used by the bt_swarm_sched_t
 */
bt_swarm_full_t &	bt_swarm_full_t::set_doauth_req(bool new_value)		throw()
{
	// if the new_value is equal to the current one, do nothing
	if( local_doauth_req() == new_value )	return *this;
	// set the new_value as the current one
	m_local_doauth_req	= new_value;
	// compute the command according to the new value
	bt_cmd_t	bt_cmd;
	if( new_value )	bt_cmd	= bt_cmd_t::build_doauth_req();
	else		bt_cmd	= bt_cmd_t::build_unauth_req();	
	// send the command to the remote peer to warn it
	sendq->queue_cmd( bt_cmd );
	// if the new value mean UNAUTH_REQ, remove all the pending BLOCK_REP of the sendq
	if( !local_doauth_req() )	sendq->remove_all_block_rep();	
	// return the object itself
	return *this;
}

/** \brief Send a bt_cmd_t thru this bt_swarm_full_t
 */
void	bt_swarm_full_t::send_cmd(const bt_cmd_t &bt_cmd)	throw()
{
	sendq->queue_cmd( bt_cmd );	
}

/** \brief to be called when a piece_idx is newly marked available
 */
void	bt_swarm_full_t::declare_piece_newly_avail(size_t piece_idx)	throw()
{
	const bt_swarm_profile_t &profile	= bt_swarm->profile();
	// if no_redundant_have is set and the piece is remotly available, dont send the cmd
	if(profile.no_redundant_have() && remote_pavail().is_avail(piece_idx))	return;
	// send the bt_cmd
	send_cmd( bt_cmd_t::build_piece_isavail(piece_idx) );
}

/** \brief to be called when a piece_idx is newly marked unavailable
 * 
 * - WARNING: this function MAY delete the bt_swarm_full_t. take this into account.
 */
void	bt_swarm_full_t::declare_piece_nomore_avail(size_t piece_idx)	throw()
{
	// autodelete the connection if there is pending BLOCK_REP for this piece_idx in the sendq
	// - NOTE: autodelete is the only possible option as it is impossible to notify the
	//   remote peer that a particular request have been canceled (limitation in 
	//   bt protocol)
	//   - maybe some stuff about the 'fast' extension (not implemented)
	const std::list<bt_cmd_t> &		cmd_queue	= sendq->cmd_queue;
	std::list<bt_cmd_t>::const_iterator	iter;
	for(iter = cmd_queue.begin(); iter != cmd_queue.end(); iter++){
		const bt_cmd_t &	bt_cmd = *iter;
		// if this bt_cmd_t is NOT a BLOCK_REP, skip it
		if( bt_cmd.cmdtype() != bt_cmdtype_t::BLOCK_REP )	continue;
		// if this bt_cmd_t is NOT for this piece_idx, skip it
		if( bt_cmd.prange().piece_idx() != piece_idx )		continue;
		// else autodelete the bt_swarm_full_t and return immediatly
		autodelete();
		return;
	}

	// if the remote peer support bt_utmsgtype_t::PUNAVAIL, notify it 
	if( full_utmsg()->do_support(bt_utmsgtype_t::PUNAVAIL) ){
		// TODO may be put a helper to get the it directly without putting the dynamic_cast here
		// - see what has been done in the bt_swarm_utmsg_t::utmsg_piecewish()
		bt_utmsg_cnx_vapi_t * 	cnx_vapi	= full_utmsg()->cnx_vapi(bt_utmsgtype_t::PUNAVAIL);
		bt_utmsg_punavail_cnx_t*punavail_cnx	= dynamic_cast<bt_utmsg_punavail_cnx_t*>(cnx_vapi);
		DBG_ASSERT( punavail_cnx );
		punavail_cnx->xmit_punavail(piece_idx);
	}
}

/** \brief Function used to update the listen_pview *DURING* bt_session_t run
 */
void	bt_swarm_full_t::update_listen_pview(const ipport_addr_t &new_listen_pview)	throw()
{
	// log to debug
	KLOG_ERR("enter NOT YET IMPLEMENTED new_listen_pview=" << new_listen_pview);

	// ask bt_swarm_full_utmsg_t to send an handshake IIF remote peer 
	// has bt_protoflag_t::UT_MSGPROTO
	// - this bt_utmsg_handshake_t contains the listen port
	// - TODO once again IIF the port has changed...
	if( remote_protoflag.fully_include(bt_protoflag_t::UT_MSGPROTO) )
		full_utmsg()->send_handshake();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool 	bt_swarm_full_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// autodelete
	return autodelete();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	bt_swarm_full_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// sanity check - the zerotimer_t MUST be first_parse_zerotimer
	DBG_ASSERT( &cb_zerotimer == &first_parse_zerotimer );
	// parse the receved_data
	return parse_recved_data();
}

/** \brief notify bt_swarm_full_event_t::cnx_opened and initial commands send
 * 
 * - it MUST be called before doing any other bt_swarm_full_event_t::notification
 */
bool	bt_swarm_full_t::notify_cnx_open()	throw()
{
	// queue a bt_cmd_t::PIECE_BFIELD
	sendq->queue_cmd( bt_cmd_t::build_piece_bfield() );

	// ask bt_swarm_full_utmsg_t to send an handshake IIF remote peer has bt_protoflag_t::UT_MSGPROTO
	if( remote_protoflag.fully_include(bt_protoflag_t::UT_MSGPROTO) )
		full_utmsg()->send_handshake();

	// mark this bt_swarm_full_t as notified_as_open
	notified_as_open	= true;

	// notify the event bt_swarm_full_event_t::cnx_opened
	bool	tokeep	= notify_callback(bt_swarm_full_event_t::build_cnx_opened());
	if( !tokeep )	return false;
	
	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t to provide event
 */
bool	bt_swarm_full_t::neoip_socket_full_event_cb(void *userptr
			, socket_full_t &cb_socket_full, const socket_event_t &socket_event) throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// if the socket_event_t is fatal, autodelete this router_resp_cnx_t
	if( socket_event.is_fatal() )	return autodelete();
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:{
			pkt_t *	pkt	= socket_event.get_recved_data();
			// log to debug
			KLOG_DBG("data len=" << pkt->length());
			// queue the received data to the one already received
			recved_data.append(pkt->void_ptr(), pkt->length());
			// parse the recved command
			return parse_recved_data();}
	case socket_event_t::MAYSEND_ON:
			// to warn the sendq that it can send
			sendq->notify_maysend();
			return true;	
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         generic command parsing
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief parse the received commands
 * 
 * - it read a whole command out of this->recved_data and then pass it to parse_recved_cmd()
 * 
 * @return a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_recved_data()	throw()
{
	const bt_swarm_profile_t &profile	= bt_swarm->profile();
	uint32_t		cmd_len;
	// if bt_swarm_full_t is not yet notified_as_open, notify it now
	if( !notified_as_open ){
		bool	tokeep = notify_cnx_open();
		if( !tokeep )	return false;
	}

	// parse as many commands as possible
	while( recved_data.size() ){
		// read a command length
		try {
			recved_data.unserial_peek( cmd_len );
		}catch(serial_except_t &e){
			// if unserialization of the cmd_len failed, it is not yet fully received, return tokeep
			return true;
		}
		// if the cmd_len is >= than the profile.recv_cmd_maxlen, autodelete
		if( cmd_len >  profile.recv_cmd_maxlen() )		return autodelete();
		// if recved_data are not long enougth to contain the whole command, return tokeep
		if( recved_data.size() < sizeof(uint32_t) + cmd_len )	return true;
		// extract the whole command from the recved_data WITHOUT copy
		pkt_t	cmd_pkt;
		cmd_pkt.work_on_data_nocopy(recved_data.char_ptr() + sizeof(uint32_t), cmd_len);

		// update the dloaded_fulllen in bt_swarm_stats_t
		bt_swarm_stats_t & swarm_stats	= bt_swarm->swarm_stats();	
		swarm_stats.dloaded_fulllen(swarm_stats.dloaded_fulllen()+cmd_pkt.size());

		// return tokeep
		bool	tokeep	= parse_recved_cmd(cmd_pkt);
		if( !tokeep )	return false;
		// free just parsed command from the recved_data
		recved_data.head_free(sizeof(uint32_t) + cmd_len);
	}
	
	// return tokeep
	return true;
}

/** \brief parse a received command
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_recved_cmd(pkt_t &pkt)			throw()
{
	const bt_swarm_profile_t &profile	= bt_swarm->profile();
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);
	// restart the idle_timeout at eached receved command
	idle_timeout.start(profile.full_idle_timeout(), this, NULL);
	
	// handle the KEEP_ALIVE 'command' - the idle_timeout is restarted above for each command
	// - it has a length of 0 with no packet type
	if( pkt.size() == 0 )	return true;

	// parse the packet type
	bt_cmdtype_t	cmdtype;
	try {
		pkt >> cmdtype;
	}catch(serial_except_t &e){
		// if the unserialization failed, it is a bug in the protocol, autodelete
		return autodelete();
	}
	// log to debug
	KLOG_DBG("cmdtype=" << cmdtype << "pkt.size=" << pkt.size());

	// parse the command depending on the cmdtype
	switch(cmdtype.get_value()){
	case bt_cmdtype_t::DOAUTH_REQ:		return parse_doauth_req_cmd(pkt);
	case bt_cmdtype_t::UNAUTH_REQ:		return parse_unauth_req_cmd(pkt);
	case bt_cmdtype_t::DOWANT_REQ:		return parse_dowant_req_cmd(pkt);
	case bt_cmdtype_t::UNWANT_REQ:		return parse_unwant_req_cmd(pkt);
	case bt_cmdtype_t::PIECE_ISAVAIL:	return parse_piece_isavail_cmd(pkt);
	case bt_cmdtype_t::PIECE_BFIELD:	return parse_piece_bfield_cmd(pkt);
	case bt_cmdtype_t::BLOCK_REQ:		return parse_block_req(pkt);
	case bt_cmdtype_t::BLOCK_REP:		return parse_block_rep(pkt);
	case bt_cmdtype_t::BLOCK_DEL:		return parse_block_del(pkt);
	case bt_cmdtype_t::UTMSG_PAYL:		return parse_utmsg_payl(pkt);
	default:	// unknown command type are simply ignored
			KLOG_ERR("recved unknown command " << cmdtype.get_value()
					<< " from " << remote_peerid().peerid_progfull());
			return true;
	}

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        command parsing specific to each command
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief parse a bt_cmdtype_t::DOAUTH_REQ command
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_doauth_req_cmd(pkt_t &pkt)			throw()
{
	// set the local variable
	m_other_doauth_req	= true;

	// notify the callback of this event
	bool	tokeep	= notify_callback( bt_swarm_full_event_t::build_doauth_req() );
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

/** \brief parse a bt_cmdtype_t::UNAUTH_REQ command
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_unauth_req_cmd(pkt_t &pkt)			throw()
{
	// set the local variable
	m_other_doauth_req	= false;
	
	// notify the callback of this event
	// - it is up to the scheduler to delete all pending sched_request
	bool	tokeep	= notify_callback( bt_swarm_full_event_t::build_unauth_req() );
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

/** \brief parse a bt_cmdtype_t::DOWANT_REQ command
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_dowant_req_cmd(pkt_t &pkt)			throw()
{
	// set the local variable
	m_other_dowant_req	= true;

	// notify the callback of this event
	bool	tokeep	= notify_callback( bt_swarm_full_event_t::build_dowant_req() );
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

/** \brief parse a bt_cmdtype_t::UNWANT_REQ command
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_unwant_req_cmd(pkt_t &pkt)			throw()
{
	// set the local variable
	m_other_dowant_req	= false;
	
	// notify the callback of this event
	bool	tokeep	= notify_callback( bt_swarm_full_event_t::build_unwant_req() );
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

/** \brief parse a bt_cmdtype_t::PIECE_ISAVAIL command
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_piece_isavail_cmd(pkt_t &pkt)			throw()
{
	uint32_t	piece_idx;
	try {
		pkt >> piece_idx;
	}catch(serial_except_t &e){
		// if the unserialization failed, it is a bug in the protocol, autodelete
		return autodelete();
	}
	// log to debug
	KLOG_DBG("piece_idx=" << piece_idx);
	// if the piece_idx is not in the proper range, autodelete
	if( piece_idx >= remote_pavail().nb_piece() )	return autodelete();	

	// if piece_idx piece is already marked as avail, do nothing 
	if( remote_pavail().is_avail(piece_idx) )	return true;

	// if local peer was not interested and this piece_idx is not locally available, make it interested
	if( bt_swarm->local_pavail().is_unavail(piece_idx) && !local_dowant_req() ){
		m_local_dowant_req	= true;
		sendq->queue_cmd( bt_cmd_t::build_dowant_req() );
	}

	// if bt_swarm_full_t DO NOT support bt_utmsgtype_t::PIECEWISH, update piecewish
	if( full_utmsg()->no_support(bt_utmsgtype_t::PIECEWISH) ){
		bool	tokeep	= notify_utmsg_nowish_index(piece_idx);
		if( !tokeep )	return false;
	}

	// mark the piece as available in the remote_pavail
	m_remote_pavail.mark_isavail(piece_idx);
	// notify the callback of this event
	bool	tokeep	= notify_callback( bt_swarm_full_event_t::build_piece_isavail(piece_idx) );
	if( !tokeep )	return false;

	// if the local peer is seed and the remote one too, close the connection
	// - NOTE: it MUST be done after the notification to have the full and sched in sync when deleting
	if( bt_swarm->is_seed() && remote_pavail().is_fully_avail() )	return autodelete();	

	// return tokeep
	return true;
}

/** \brief parse a bt_cmdtype_t::PIECE_BFIELD command
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_piece_bfield_cmd(pkt_t &pkt)			throw()
{
	const bt_mfile_t &	bt_mfile	= bt_swarm->bt_mfile;
	// log to debug
	KLOG_DBG("enter pkt="<< pkt);
	// try to parse it
	try {
		m_remote_pavail	= bt_pieceavail_t::btformat_from_pkt(pkt, bt_mfile.nb_piece());
	}catch(serial_except_t &e){
		// if the unserialization failed, it is a bug in the protocol, autodelete
		return autodelete();
	}
	// if the local peer was not interested and this pieceavail contains pieces not locally avaiable
	// => switch to interested
	if( !local_dowant_req() && !bt_swarm->local_pavail().fully_contain(remote_pavail()) ){
		m_local_dowant_req	= true;
		sendq->queue_cmd( bt_cmd_t::build_dowant_req() );
	}

	// notify the callback of this event
	bool	tokeep	= notify_callback(bt_swarm_full_event_t::build_piece_bfield());
	if( !tokeep )	return false;

	// if bt_swarm_full_t DO NOT support bt_utmsgtype_t::PIECEWISH, update piecewish
	if( full_utmsg()->no_support(bt_utmsgtype_t::PIECEWISH) ){
		bool	tokeep	= notify_utmsg_dowish_field(~remote_pavail());
		if( !tokeep )	return false;
	}

	// if the local peer is seed and the remote one too, close the connection
	// - NOTE: it MUST be done after the notification to have the full and sched in sync when deleting
	if( bt_swarm->is_seed() && remote_pavail().is_fully_avail() )	return autodelete();	

	// return tokeep
	return true;
}

/** \brief parse a bt_cmdtype_t::BLOCK_REQ command
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_block_req(pkt_t &pkt)			throw()
{
	const bt_swarm_profile_t &	swarm_profile	= bt_swarm->profile();
	const bt_mfile_t &		bt_mfile	= bt_swarm->get_mfile();
	uint32_t			piece_idx;
	uint32_t			data_offset;
	uint32_t			data_len;
	// log to debug
	KLOG_DBG("enter pkt="<< pkt);
	try {
		pkt >> piece_idx;
		pkt >> data_offset;
		pkt >> data_len;
	}catch(serial_except_t &e){
		// if the unserialization failed, it is a bug in the protocol, autodelete
		return autodelete();
	}
	
	// if the piece_idx is greater than or equal to the number of piece, autodelete
	if( piece_idx >= bt_mfile.nb_piece() )			return autodelete();
	// if the block_req goes across several pieces, autodelete
	if( data_offset + data_len > bt_mfile.piecelen() )	return autodelete();
	// if the data_len is larger that the swarm_profile.recv_req_maxlen, autodelete
	if( data_len > swarm_profile.recv_req_maxlen() )	return autodelete();
	// if the piece_idx is not locally available, autodelete
	// - NOTE: this is actually important for bt_io_vapi_t to http
	if( bt_swarm->local_pavail().is_unavail(piece_idx) ){
		KLOG_ERR("received a request from peerid " << remote_peerid().peerid_progfull() 
				<< " for piece_idx " << piece_idx << " so autodelete");
		return autodelete();
	}

	// if BLOCK_REQ are no authorized, discard it
	if( !local_doauth_req() )	return true;
	
	// queue the BLOCK_REP command
	bt_prange_t bt_prange	= bt_prange_t(piece_idx, data_offset, data_len);
	sendq->queue_cmd( bt_cmd_t::build_block_rep(bt_prange)  );
	// return tokeep
	return true;
}

/** \brief parse a bt_cmdtype_t::BLOCK_REP command
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_block_rep(pkt_t &pkt)			throw()
{
	bt_swarm_stats_t&	swarm_stats	= bt_swarm->swarm_stats();	
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	uint32_t		piece_idx;
	uint32_t		data_offset;
	bt_err_t		bt_err;
	// log to debug
	KLOG_DBG("enter pkt="<< pkt);
	try {
		pkt >> piece_idx;
		pkt >> data_offset;
		// NOTE: the data replied are the remaining of pkt
	}catch(serial_except_t &e){
		// if the unserialization failed, it is a bug in the protocol, autodelete
		return autodelete();
	}

	// if the piece_idx is greater than or equal to the number of piece, autodelete
	if( piece_idx >= bt_mfile.nb_piece() )			return autodelete();
	// if the block_rep goes across several pieces, autodelete
	if( data_offset + pkt.size() > bt_mfile.piecelen() )	return autodelete();
	// if the block_rep has a size of 0, autodelete
	if( pkt.size() == 0 )					return autodelete();

	// update the recv_rate
	m_recv_rate.update(pkt.size());

	// update the dloaded_datalen in bt_swarm_stats_t
	// TODO should it be updated here ? and not in the scheduler ?
	// - do i update it properly in the ecnx case too ?
	// - where should i update it ? in the block notify complete ?
	// - this is not updated for ecnx
	swarm_stats.dloaded_datalen	( swarm_stats.dloaded_datalen() + pkt.size() );
	
	// if no pending bt_swarm_sched_request_t matches this block_rep, discard it
	// - NOTE: it may happen in a race such as (i) local peer requests the block to peer alice 
	//   and bob, (ii) receives its from bob, (iii) send a BLOCK_DEL to alice, (iv) alice already
	//   replied the data before getting the BLOCK_DEL, (v) local peer received BLOCK_REP without
	//   matching bt_+swarm_sched_request_t.
	bt_prange_t	bt_prange	= bt_prange_t(piece_idx, data_offset, pkt.size());
	file_range_t	totfile_range	= bt_prange.to_totfile_range(bt_mfile);
	if( !full_sched()->has_matching_request(totfile_range) ){
		// update the bt_swarm_stats_t
		swarm_stats.dup_rep_nb	( swarm_stats.dup_rep_nb() + 1 );
		swarm_stats.dup_rep_len	( swarm_stats.dup_rep_len() + pkt.size() );
		// return tokeep
		return true;
	}
		
	// notify the callback of this event - up to the scheduler to write the data
	bt_cmd_t bt_cmd	= bt_cmd_t::build_block_rep(bt_prange);
	bool	tokeep	= notify_callback( bt_swarm_full_event_t::build_block_rep(bt_cmd, &pkt) );
	if( !tokeep )	return false;

	// return tokeep
	return true;
}

/** \brief parse a bt_cmdtype_t::BLOCK_DEL command
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_block_del(pkt_t &pkt)			throw()
{
	uint32_t	piece_idx;
	uint32_t	data_offset;
	uint32_t	data_len;
	// log to debug
	KLOG_DBG("enter pkt="<< pkt);
	try {
		pkt >> piece_idx;
		pkt >> data_offset;
		pkt >> data_len;
	}catch(serial_except_t &e){
		// if the unserialization failed, it is a bug in the protocol, autodelete
		return autodelete();
	}

	// ask the sendq to remove this BLOCK_REP
	bt_prange_t	bt_prange	= bt_prange_t(piece_idx, data_offset, data_len);
	sendq->remove_one_block_rep( bt_cmd_t::build_block_rep(bt_prange) );
	// return tokeep
	return true;
}


/** \brief parse a bt_cmdtype_t::UTMSG_PAYL command
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::parse_utmsg_payl(pkt_t &pkt)			throw()
{
	// TODO should i check if the local bt_swarm_t has the bt_protoflag_t::UT_MSGPROTO ?
	// - or if the remove peer has it too

	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// forward the packet to the bt_swarm_full_utmsg_t
	bool	tokeep	= full_utmsg()->parse_utmsg_payl(pkt);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	if( !tokeep )	return	false;

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parser specific to bt_utmsg_cnx_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to notify a piece_unavail(pieceidx) from the bt_utmsgtype_t::PUNAVAIL
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::notify_utmsg_punavail(size_t pieceidx)		throw()
{
	// log to debug
	KLOG_DBG("piece_idx=" << piece_idx);
	// sanity check - bt_utmsgtype_t::PUNAVAIL MUST be supported
	DBG_ASSERT( full_utmsg()->do_support(bt_utmsgtype_t::PUNAVAIL) );
	// if pieceidx is not in the proper range, autodelete
	if( pieceidx >= remote_pavail().nb_piece() )	return autodelete();

	// if piece_idx piece is already marked as unavail, do nothing 
	if( remote_pavail().is_unavail(pieceidx) )	return true;

	// mark the piece as available in the pieceavail
	m_remote_pavail.mark_unavail(pieceidx);
	
	// notify the callback of this event
	bool	tokeep	= notify_callback( bt_swarm_full_event_t::build_piece_unavail(pieceidx) );
	if( !tokeep )	return false;

	// if this bt_swarm_full_t DO NOT support bt_utmsgtype_t::PIECEWISH, update piecewish
	if( full_utmsg()->no_support(bt_utmsgtype_t::PIECEWISH) ){
		bool	tokeep	= notify_utmsg_dowish_index(pieceidx);
		if( !tokeep )	return false;
	}
	
	// return tokeep
	return true;
}

/** \brief function to notify a dowish_index(pieceidx) from the bt_utmsgtype_t::PIECEWISH
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::notify_utmsg_dowish_index(size_t pieceidx)		throw()
{
	// sanity check - pieceidx MUST NOT simultaneously be in remote_pavail and remote_pwish
	DBGNET_ASSERT( remote_pavail().is_unavail(pieceidx) );
	// if pieceidx is currently available, it is a bug in remote peer, autodelete the cnx
	if( remote_pavail().is_avail(pieceidx) )	return autodelete();
	
	// TODO what if the piece is already dowish
	// - may that happen in normal operation ?
	// - is that a bug ?
	// - if it is a bug, do the usual dbgnet_assert + autodelete
	// - if it can happen in normal operation, just ignore the operation  
	
	// update the m_remote_pwish
	m_remote_pwish.set(pieceidx);
	// notify the callback of this event
	bool	tokeep	= notify_callback( bt_swarm_full_event_t::build_pwish_doindex(pieceidx) );
	if( !tokeep )	return false;

	// return tokeep
	return true;	
}

/** \brief function to notify a nowish_index(pieceidx) from the bt_utmsgtype_t::PIECEWISH
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::notify_utmsg_nowish_index(size_t pieceidx)		throw()
{
	// TODO what if the piece is already nowish

	// update the m_remote_pwish
	m_remote_pwish.clear(pieceidx);
	// notify the callback of this event
	bool	tokeep	= notify_callback( bt_swarm_full_event_t::build_pwish_noindex(pieceidx) );
	if( !tokeep )	return false;

	// return tokeep
	return true;	
}

/** \brief function to notify a nowish_field(bitfield_t) from the bt_utmsgtype_t::PIECEWISH
 * 
 * @erturn a tokeep for the whole bt_swarm_full_t
 */
bool	bt_swarm_full_t::notify_utmsg_dowish_field(const bitfield_t &new_remote_pwish)	throw()
{
	// sanity check - a pieceidx MUST NOT simultaneously be in remote_pavail and remote_pwish
	DBGNET_ASSERT( (remote_pavail() & new_remote_pwish).is_none_set() );
	// if a piece is currently available and wished, it is a bug in remote peer, autodelete
	if( (remote_pavail() & new_remote_pwish).is_any_set() )	return autodelete();

	// backup the old remote_pwish
	bitfield_t	old_pwish	= remote_pwish();
	// update the remote_pwish
	m_remote_pwish	= new_remote_pwish;

	// notify the callback of this event
	bool	tokeep	= notify_callback( bt_swarm_full_event_t::build_pwish_dofield(&old_pwish, &m_remote_pwish) );
	if( !tokeep )	return false;

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
bool bt_swarm_full_t::notify_callback(const bt_swarm_full_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_swarm_full_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





