/*! \file
    \brief Class to handle the bt_utmsg_bcast_t

\par Brief Description
\rev bt_utmsg_bcast_t is an extension which allows the broadcast of message to 
all the peers of the swarm (assuming they support this extension).

\par Implementation Notes
- the current implementation actually flood the whole swarm.
- When a message is originated by the local peer, it is happended a statically
  unique nonce among all peers. this message is sent to ALL remote peers.
- When a message is received from a remote peer, the local peer check if the
  nonce has been already seen. 
  - if the nonce has been already seen, the received message is discarded 
  - if the nonce has NOT been already seen, it is forward to the local peer
    callback if any, AND forward to all remote peers except the one from which
    the message has been received 
- This technique ensure to reach everybody but this is very inneficient from a 
  network point of view
  - if a local peer is connected to 60 remote peers, it will send the msg
    60 time and receive it 60 time..
    - this is 59 more than necessary :)
  - azureus chat plugin got a 'routing' algo which reduce the duplication to 4-5
    - but this is statistical. aka there is a high probability to reach everybody
      but no garantee
    - this is a trouble for me because i want garantee
      - e.g. neoip-casto relies on the messages broadcasted by neoip-casti
        if there are not received, neoip-casto fails

\par Possible improvement - network efficiency
- first would be good to measure the overhead produced by those messages
  - they may very well be negligible compared to the amount of data
  - e.g. as of now, a neoip-casti message send a message every 4 pieces and each
    piece is 16kbyte of 32kbyte, this means a single message is originated every
    48kbyte/96kbyte.
- one could reduce the amount of message sent over the wire by
  - keeping a recved_nonce_db in each bt_utmsg_bcast_cnx_t
  - not send a given nonce to this connection if it is present in the recved_nonce_db
- obviously 2 messages may be cross each other in the air on a single connection.
  - especially with the way bittorrent protocol works. where signaling message and
    data message are sent over the same reliable connection.
  - so asking to send a message thru a connection doesnt implies it will be sent
    immediatly, but only after sending all the signal message and data messages
    which have been already queued
    - this is important as the data messages are quite large (typically 16kbyte) 
    - this could be reduced by putting the bcasted command before all other commands
      - currently not implemented
      - not even sure it would be a good thing. think about it
- to reduce this cross over in the air, one could induce some random delay before
  sending the messages.
  - it would reduce the probability of cross-over over the wire
  - nevetheless it create additionnal delay betwen the message origination and the
    reception of the message.
    - it is acceptable ?
    - one could imagine a more cooked algorithm too e.g
      - send to 4 peers immediatly
      - delay the others peers
*/

/* system include */
/* local include */
#include "neoip_bt_utmsg_bcast.hpp"
#include "neoip_bt_utmsg_bcast_cnx.hpp"
#include "neoip_bt_utmsg_bcast_handler.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_utmsg_event.hpp"
#include "neoip_bt_cmd.hpp"
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
bt_utmsg_bcast_t::bt_utmsg_bcast_t()		throw()
{
	// zero some fields
	bt_swarm	= NULL;
	utmsg_cb	= NULL;
	seen_nonce_db	= NULL;
}

/** \brief Destructor
 */
bt_utmsg_bcast_t::~bt_utmsg_bcast_t()		throw()
{
	// unregister this object
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	if( utmsg_cb ){
		bool	tokeep	= notify_utmsg_cb( bt_utmsg_event_t::build_unregister() );
		DBG_ASSERT( tokeep == true );
	}
	// close all pending bt_utmsg_bcast_cnx_t
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
	// sanity check - handler_db MUST be empty at this point
	// - up to the caller to destroy them
	DBG_ASSERT( handler_db.empty() );
	// delete seen_nonce_db if needed
	nipmem_zdelete	seen_nonce_db;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_utmsg_bcast_t &bt_utmsg_bcast_t::set_profile(const bt_utmsg_bcast_profile_t &profile)	throw()
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
bt_err_t	bt_utmsg_bcast_t::start(bt_swarm_t *bt_swarm, bt_utmsg_cb_t *utmsg_cb
							, void *userptr)	throw()
{
	bool	tokeep;
	// copy the parameter
	this->bt_swarm	= bt_swarm;
	this->utmsg_cb	= utmsg_cb;
	this->userptr	= userptr;
	// init the seen_nonce_db
	seen_nonce_db	= nipmem_new seen_nonce_db_t(profile.nonce_expire_delay());
	
	// register this utmsg to the callback
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return dontkeep
	tokeep	= notify_utmsg_cb( bt_utmsg_event_t::build_doregister() );
	DBG_ASSERT( tokeep == true );
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Originate a pkt_t with the opstr operation and payload
 */
void	bt_utmsg_bcast_t::originate_pkt(const std::string &opstr, const pkt_t &payload)	throw()
{
	bt_utmsg_bcast_nonce_t	cmd_nonce	= bt_utmsg_bcast_nonce_t::build_random();
	bt_cmd_t		bt_cmd;
	// put the cmd_nonce in the seen_nonce_db
	seen_nonce_db->update(cmd_nonce);
	// build the bt_cmd_t
	bt_cmd		= build_cmd(cmd_nonce, opstr, payload);
	// forward the bt_cmd_t to all bt_utmsg_bcast_cnx_t
	forward_cmd(bt_cmd, NULL);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a bt_cmd_t to send
 */
bt_cmd_t	bt_utmsg_bcast_t::build_cmd(const bt_utmsg_bcast_nonce_t &bcast_nonce
				, const std::string &opstr, const pkt_t &payload)	throw()
{
	dvar_t	dvar	= dvar_map_t();
	// build the dvat_map_t for this bt_cmd_t
	dvar.map().insert("n", dvar_str_t(bcast_nonce.to_stdstring()));
	dvar.map().insert("o", dvar_str_t(opstr));
	dvar.map().insert("p", dvar_str_t(payload.to_stdstring()));

	// generate the payload of the bt_cmd_t
	pkt_t	pkt;
	pkt << bt_utmsgtype_t(bt_utmsgtype_t::BCAST);
	pkt.append(datum_t(bencode_t::from_dvar(dvar)));
	// return the just built bt_cmd_t
	return bt_cmd_t::build_utmsg_payl(pkt.to_datum(datum_t::NOCOPY));
}

/** \brief forward a bt_cmd_t to all bt_utmsg_bcast_cnx_t but src_cnx
 * 
 * - NOTE: src_cnx may be NULL. if so, the bt_cmd_t is sent to all without exception
 */
void	bt_utmsg_bcast_t::forward_cmd(const bt_cmd_t &bt_cmd, bt_utmsg_bcast_cnx_t *src_cnx) throw()
{
	std::list<bt_utmsg_bcast_cnx_t *>::iterator	iter;
	// go thru the whole cnx_db
	for( iter = cnx_db.begin(); iter != cnx_db.end(); iter++){
		bt_utmsg_bcast_cnx_t *	bcast_cnx	= *iter;
		bt_swarm_full_utmsg_t *	full_utmsg	= bcast_cnx->full_utmsg();
		bt_swarm_full_t *	swarm_full	= full_utmsg->get_swarm_full();
		// if this bcast_cnx IS the src_cnx, goto the next
		if( bcast_cnx == src_cnx )	continue;
		// send this bt_cmd to the corresponding bt_swarm_full_t
		swarm_full->send_cmd( bt_cmd );
	}
}

/** \brief Parse a pkt_t received by a bt_utmsg_bcast_cnx_t source connection
 */
bt_err_t bt_utmsg_bcast_t::parse_pkt(const pkt_t &pkt, bt_utmsg_bcast_cnx_t *src_cnx)	throw()
{
	// convert the message body to a dvar_t
	dvar_t	dvar	= bencode_t::to_dvar(pkt.to_datum(datum_t::NOCOPY));
	// log to debug
	KLOG_DBG("enter bt_utmsgtype=" << bt_utmsgtype << " pkt=" << pkt);

	// if the handshake_dvar failed to parse the bencoded data, return an error
	if( dvar.is_null() || dvar.type() != dvar_type_t::MAP )
		return bt_err_t(bt_err_t::ERROR,"unable to find bcast payload as dvar_type_t::MAP");

	// if the byteacct_dvar doesnt include the balance, return an error
	if( !dvar.map().contain("n", dvar_type_t::STRING) )
		return bt_err_t(bt_err_t::ERROR,"unable to find 'n' bcast payload");
	if( !dvar.map().contain("o", dvar_type_t::STRING) )
		return bt_err_t(bt_err_t::ERROR,"unable to find 'o' bcast payload");
	if( !dvar.map().contain("p", dvar_type_t::STRING) )
		return bt_err_t(bt_err_t::ERROR,"unable to find 'p' bcast payload");

	
	// get the data from the dvar_t
	bt_utmsg_bcast_nonce_t	bcast_nonce	( datum_t( dvar.map()["n"].str().get() ) );
	std::string		opstr		= dvar.map()["o"].str().get(); 
	pkt_t			payload		= pkt_t( dvar.map()["p"].str().get() );

	// if this bcast_nonce exist in seen_nonce_db, discard this packet	
	if( seen_nonce_db->contain(bcast_nonce) ){
		KLOG_ERR("received an already seen bcast_nonce for opstr=" << opstr << " and payload=" << payload);
		return bt_err_t::OK;
	}
	// put the cmd_nonce in the seen_nonce_db
	seen_nonce_db->insert(bcast_nonce);

	// forward the packet to others
	forward_cmd( build_cmd(bcast_nonce, opstr, payload), src_cnx );
	
	// try to find a bt_utmsg_bcast_handler_t for this opstr
	bt_utmsg_bcast_handler_t *	bcast_handler;
	bcast_handler	= handler_by_opstr(opstr);
	// if there is one bt_utmsg_bcast_handler, pass it the payload
	if( bcast_handler )	bcast_handler->notify_recved_pkt(payload);

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			handler management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a pointer on the bt_utmsg_bcast_handler_t matching opstr, NULL if none matches
 */
bt_utmsg_bcast_handler_t *bt_utmsg_bcast_t::handler_by_opstr(const std::string &opstr)	const throw()
{
	std::list<bt_utmsg_bcast_handler_t *>::const_iterator	iter;
	// go thru the whole handler_db
	for(iter = handler_db.begin(); iter != handler_db.end(); iter++){
		bt_utmsg_bcast_handler_t * bcast_handler	= *iter;
		// if this bcast_handler matches, return its pointer
		if( bcast_handler->opstr() == opstr )	return bcast_handler;
	}
	// if this point is reached, none matches, so return NULL
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    bt_utmsg_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the string which identify this bt_utmsg_vapi_t
 */
std::string	bt_utmsg_bcast_t::utmsgstr()		const throw()
{
#if 0
	return "bcast";
#else	// 'scramble' the string to make it harder to guess
	// - usefull for the 'hidding' of this options
	return bt_id_t("bcast").to_canonical_string();
#endif
}

/** \brief Construct a bt_utmsg_bcast_cnx_t from a bt_swarm_full_utmsg_t
 */
bt_utmsg_cnx_vapi_t * bt_utmsg_bcast_t::cnx_ctor(bt_swarm_full_utmsg_t *full_utmsg)	throw()
{
	bt_err_t	bt_err;	
	// create a bt_utmsg_bcast_cnx_t for this bt_swarm_full_utmsg_t
	bt_utmsg_bcast_cnx_t *bcast_cnx;
	bcast_cnx	= nipmem_new bt_utmsg_bcast_cnx_t();
	bt_err		= bcast_cnx->start(this, full_utmsg);
	DBG_ASSERT( bt_err.succeed() );
	// return the just build bcast_cnx
	return bcast_cnx;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the bt_utmsg_event_t
 */
bool bt_utmsg_bcast_t::notify_utmsg_cb(const bt_utmsg_event_t &event)	throw()
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





