/*! \file
    \brief Class to handle the bt_swarm_full_utmsg_t

\par TODO
- i dont understand why bt_utmsgtype_t::HANDSHAKE is in the convtable

\par Brief Description
\par bt_swarm_full_utmsg_t handles the bt_protoflag_t::UT_MSGPROTO stuff for a
given bt_swarm_full_t. this includes the bt_utmsgtype_t::HANDSHAKE and the routing
of received bt_utmsgtype_t to the proper bt_utmsg_vapi_t (found via bt_swarm_utmsg_t)

- the protocol is described in 
  http://www.rasterbar.com/products/libtorrent/extension_protocol.html

\par About notifiying the bt_utmsg_vapi_t
- the cnx_opened is notified only when the bt_utmsgtype_t::HANDSHAKE has been received
  the cnx_closed is notified on destruction of the object IIF cnx_opened has been
  previously notified

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_sendq.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_utmsg.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_bt_utmsgtype.hpp"
#include "neoip_bt_utmsg_vapi.hpp"
#include "neoip_bt_utmsg_cnx_vapi.hpp"
#include "neoip_bt_peersrc_peer.hpp"
#include "neoip_bt_cmd.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bencode.hpp"
#include "neoip_dvar.hpp"
#include "neoip_pkt.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_swarm_full_utmsg_t::bt_swarm_full_utmsg_t(bt_swarm_full_t *swarm_full)	throw()
{
	// copy the parameters
	this->swarm_full	= swarm_full;
	// zero some field
	notified_opened		= false;
	// init the convertion table for the bt_utmsgtype_t with the bt_utmsgtype_t::HANDSHAKE
	// - bt_utmsgtype_t::HANDSHAKE is ALWAYS 0 and is always supported
	bool succeed = convtable_db.insert(std::make_pair(bt_utmsgtype_t::HANDSHAKE, 0)).second;
	DBG_ASSERT( succeed );
}

/** \brief Destructor
 */
bt_swarm_full_utmsg_t::~bt_swarm_full_utmsg_t()					throw()
{
	// delete all the bt_utmsg_cnx_vapi_t from the cnx_vapi_db
	while( !cnx_vapi_db.empty() )	nipmem_delete	cnx_vapi_db.begin()->second;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a bt_peersrc_peer_t for this remote peer
 */
bt_peersrc_peer_t	bt_swarm_full_utmsg_t::to_peersrc_peer()	const throw()
{
	bt_peersrc_peer_t	peersrc_peer;
	// if remote_tcp_listen_ipport is not fully_qualified, return a null one
	if( !remote_tcp_listen_ipport.is_fully_qualified() )	return bt_peersrc_peer_t();
	
	// set the remote_peerid 
	peersrc_peer.peerid	(swarm_full->remote_peerid());
	// set the dest_addr
	peersrc_peer.dest_addr	(std::string("stcp://") + remote_tcp_listen_ipport.to_string());
	// set the is_seed
	peersrc_peer.is_seed	(swarm_full->is_seed());
	
	// TODO what about the "want_jamstd"
	// - this state is not available... do the modif 
	
	
	// return the just-built peersrc_peer
	return peersrc_peer;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         handshake managerment
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Generate a handshake payload
 */
datum_t	bt_swarm_full_utmsg_t::generate_handshake()		const throw()
{
	bt_swarm_t *		bt_swarm	= swarm_full->get_swarm();
	bt_swarm_utmsg_t *	swarm_utmsg	= bt_swarm->swarm_utmsg();
	bt_session_t *		bt_session	= bt_swarm->get_session();
	dvar_t			handshake_dvar	= dvar_map_t();

	// insert the program version
	handshake_dvar.map().insert("v", dvar_str_t("unknown version of golden ratio"));
	
	// insert the tcp_listen_port 
	// - NOTE: not influenced by nposwatch_t::ipaddr_pview() as only the port is sent
	// - TODO: need modification in case of upnp as the listen_ipport_pview will change
	//   - according to the specification, it is ok to retransmit the handshake and 
	//     the receiver should handle the case.
	//   - "It is valid to send the handshake message more than once during the lifetime
	//      of a connection, the sending client should not be disconnected. An implementation
	//      may choose to ignore the subsequent handshake messages (or parts of them)."
	//     from the specification.
	ipport_addr_t	listen_ipport_pview	= bt_session->listen_pview();
	handshake_dvar.map().insert("p", dvar_int_t(listen_ipport_pview.port()));

	// insert the convtable
	handshake_dvar.map().insert("m", dvar_map_t());

	// create an alias on the convtable field
	dvar_map_t &convtable_dvar	= handshake_dvar.map()["m"].map();

	// build the convtable for each registered bt_utmsg_vapi_t
	const bt_swarm_utmsg_t::utmsg_vapi_db_t &utmsg_vapi_db	= swarm_utmsg->utmsg_vapi_db();
	bt_swarm_utmsg_t::utmsg_vapi_db_t::const_iterator	iter;
	for(iter = utmsg_vapi_db.begin(); iter != utmsg_vapi_db.end(); iter++){
		const bt_utmsg_vapi_t *	utmsg_vapi	= *iter;
		bt_utmsgtype_t		bt_utmsgtype	= utmsg_vapi->utmsgtype();
		std::string		utmsgstr	= utmsg_vapi->utmsgstr();
		// add this bt_utmsg_vapi_t in the convtable_dvar
		convtable_dvar.insert( utmsgstr, dvar_int_t(bt_utmsgtype.get_value()));
	}

	// convert the handshake_dvar into its bencoded format
	return datum_t(bencode_t::from_dvar(handshake_dvar));
}


/** \brief function to call in case of error while parsing incoming payload
 */
bool	bt_swarm_full_utmsg_t::parsing_error(const std::string &reason)	throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR(reason);
	// return tokeep
	return true;
}

/** \brief send the bt_utmsgtype_t::HANDSHAKE
 */
bt_err_t	bt_swarm_full_utmsg_t::send_handshake()		throw()
{
	// build the payload for the bt_utmsgtype_t::HANDSHAKE
	pkt_t	payload;
	payload << bt_utmsgtype_t(bt_utmsgtype_t::HANDSHAKE);
	payload.append(generate_handshake());
	// build the bt_cmd_t to send
	bt_cmd_t bt_cmd	= bt_cmd_t::build_utmsg_payl(payload.to_datum(datum_t::NOCOPY));
	// send it thru the bt_swarm_full_t
	swarm_full->send_cmd( bt_cmd );
	// return no error
	return bt_err_t::OK;
}

/** \brief parse a bt_utmsgtype_t::HANDSHAKE
 * 
 * @return a tokeep for the bt_swarm_full_t
 */
bool	bt_swarm_full_utmsg_t::parse_handshake(pkt_t &pkt)	throw()
{
	bt_swarm_t *		bt_swarm	= swarm_full->get_swarm();
	bt_swarm_utmsg_t *	swarm_utmsg	= bt_swarm->swarm_utmsg();	
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);

	// convert the handshake message body to a dvar_t
	dvar_t	handshake_dvar	= bencode_t::to_dvar(pkt.to_datum(datum_t::NOCOPY));

	// if the handshake_dvar failed to parse the bencoded data, return an error
	if( handshake_dvar.is_null() || handshake_dvar.type() != dvar_type_t::MAP )
		return parsing_error("unable to find handshake_dvar as dvar_type_t::MAP");

	// get the prog_version if available
	if( handshake_dvar.map().contain("v", dvar_type_t::STRING) )
		remote_prog_version	= handshake_dvar.map()["v"].str().get();

	// get the tcp_listen_port if available
	if( handshake_dvar.map().contain("p", dvar_type_t::INTEGER) ){
		uint16_t tcp_listen_port = handshake_dvar.map()["p"].integer().get();
		// build the remote_tcp_listen_ipport from the connection remote_addr and listen port
		remote_tcp_listen_ipport = swarm_full->remote_addr().get_peerid_vapi()->to_string()
						+ std::string(":") + OSTREAMSTR(tcp_listen_port);
		// if tcp_listen_port is 0, nullify remote_tcp_listen_ipport
		if( tcp_listen_port == 0 )	remote_tcp_listen_ipport = ipport_addr_t();
	}

	// if the handshake_dvar doesnt contains the convtable map, return now
	if( !handshake_dvar.map().contain("m", dvar_type_t::MAP) )	return true;
	// create an alias on the convtable_dvar
	const dvar_map_t &convtable_dvar	= handshake_dvar.map()["m"].map();

	// parse the convtable for each registered bt_utmsg_vapi_t
	const bt_swarm_utmsg_t::utmsg_vapi_db_t &utmsg_vapi_db	= swarm_utmsg->utmsg_vapi_db();
	bt_swarm_utmsg_t::utmsg_vapi_db_t::const_iterator	iter;
	for(iter = utmsg_vapi_db.begin(); iter != utmsg_vapi_db.end(); iter++){
		bt_utmsg_vapi_t *	utmsg_vapi	= *iter;
		bt_utmsgtype_t		bt_utmsgtype	= utmsg_vapi->utmsgtype();
		std::string		utmsgstr	= utmsg_vapi->utmsgstr();
		// if this utmsg is not supported by remove peer, goto the next
		if( !convtable_dvar.contain(utmsgstr, dvar_type_t::INTEGER) )	continue;

		// get the opcode used by the remote peer
		uint8_t	opcode	= convtable_dvar[utmsgstr].integer().get();

		// if the new opcode is 0, DO NOT add it and goes to the next
		// - if an opcode is 0, this mean disable the option
		if( opcode == 0 ){
			// if bt_utmsgtype_t is not in convtable_db, goto the next
			// - this means a bug in the remote peer as it put a 0 opcode for
			//   an extension which has not yet been initialized
			if( convtable_db.find(bt_utmsgtype) == convtable_db.end() )	continue;
			// sanity check - the cnx_vapi_db MUST contain the bt_utmsgtype_t too
			DBG_ASSERT( cnx_vapi_db.find(bt_utmsgtype) != cnx_vapi_db.end() );
			// delete the cnx_vapi object
			bt_utmsg_cnx_vapi_t * cnx_vapi	= cnx_vapi_db.find(bt_utmsgtype)->second;
			nipmem_delete	cnx_vapi;	
			// remote this bt_utmsgtype_t from both database
			cnx_vapi_db.erase(bt_utmsgtype);
			convtable_db.erase(bt_utmsgtype);
			continue;
		}		

		// insert/update this bt_utmsgtype to the convtable_db
		convtable_db[bt_utmsgtype] = opcode;

		// create the bt_utmsg_cnx_vapi_t - if not yet initialized
		// - it links itself to the bt_swarm_full_utmsg_t
		if( cnx_vapi_db.find(bt_utmsgtype) == cnx_vapi_db.end() ){
			bt_utmsg_cnx_vapi_t * cnx_vapi	= utmsg_vapi->cnx_ctor(this);
			DBG_ASSERT( cnx_vapi );
		}
	}

	// return no error
	return true;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse a bt_cmdtype_t::UTMSG_PAYL received by the attached bt_swarm_full_t
 * 
 * @return a tokeep for the bt_swarm_full_t
 */
bool	bt_swarm_full_utmsg_t::parse_utmsg_payl(pkt_t &pkt)		throw()
{
	uint8_t			recved_opcode;
	bt_utmsgtype_t		recved_utmsgtype;
	// log to debug
	KLOG_DBG("enter pkt="<< pkt);

	// parse the bt_utmsgtype_t
	try {
		pkt >> recved_opcode;
	}catch(serial_except_t &e){
		return bt_err_t::ERROR;
	}

	// log to debug
	KLOG_DBG("recved_opcode=" << (int)recved_opcode);

	// convert the recved_opcode with the convtable_db_t
	convtable_db_t::const_iterator	iter;
	for(iter = convtable_db.begin(); iter != convtable_db.end(); iter++){
		const bt_utmsgtype_t &	local_utmsgtype	= iter->first;
		const uint8_t &		remote_opcode	= iter->second;
		// if the remote_opcode is not the recved_opcode, goto the next
		if( recved_opcode != remote_opcode )	continue;
		// set the recved_utmsgtype and leave the loop
		recved_utmsgtype	= local_utmsgtype;
		break;
	}
	// if the recved_opcode has not been found in the convtable_db, return an error
	if( iter == convtable_db.end() )
		return parsing_error("received opcode not in convtable_db from a " + swarm_full->remote_peerid().peerid_progfull());

	// if the recved_utmsgtype is a HANDSHAKE, parse it internally
	if( recved_utmsgtype == bt_utmsgtype_t::HANDSHAKE )	return parse_handshake(pkt);

	// sanity check - the cnx_vapi_db MUST contain the bt_utmsgtype_t too
	DBG_ASSERT( cnx_vapi_db.find(recved_utmsgtype) != cnx_vapi_db.end() );

	// send the packet to parse to the bt_utmsg_cnx_vapi_t
	bt_utmsg_cnx_vapi_t * cnx_vapi	= cnx_vapi_db.find(recved_utmsgtype)->second;
	return cnx_vapi->parse_pkt(recved_utmsgtype, pkt);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			cnx_vapi_db management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on the bt_utmsg_cnx_vapi_t matching this bt_utmsgtype_t
 * 
 * - if none matches, it returns NULL
 */
bt_utmsg_cnx_vapi_t *bt_swarm_full_utmsg_t::cnx_vapi(const bt_utmsgtype_t &bt_utmsgtype) 
										const throw()
{
	// try to find the bt_utmsg_cnx_vapi_t for this bt_utmsgtype_t
	cnx_vapi_db_t::const_iterator	iter	 = cnx_vapi_db.find(bt_utmsgtype);
	// if not found, return NULL
	if( iter == cnx_vapi_db.end() )	return NULL;
	// else return a pointer on the matching bt_utmsg_cnx_vapi_t
	return iter->second;
}

/** \brief DoLink a bt_utmsg_cnx_vapi_t to this object
 * 
 * - this is used by the bt_utmsg_cnx_vapi_t which link themselves here when created
 */
void	bt_swarm_full_utmsg_t::cnx_vapi_dolink(bt_utmsg_cnx_vapi_t *cnx_vapi)	throw()
{
	// get the bt_utmsgtype_t for this cnx_vapi
	const bt_utmsgtype_t &	bt_utmsgtype	= cnx_vapi->utmsg_vapi()->utmsgtype();
	// sanity check - the cnx_vapi_db MUST NOT contain the bt_utmsgtype_t
	DBG_ASSERT( cnx_vapi_db.find(bt_utmsgtype) == cnx_vapi_db.end() );
	// add this cnx_vapi to the cnx_vapi_db
	cnx_vapi_db.insert(std::make_pair(bt_utmsgtype, cnx_vapi));
}

/** \brief UnLink a bt_utmsg_cnx_vapi_t fropm this object
 */
void	bt_swarm_full_utmsg_t::cnx_vapi_unlink(bt_utmsg_cnx_vapi_t *cnx_vapi)	throw()
{
	// get the bt_utmsgtype_t for this cnx_vapi
	const bt_utmsgtype_t &	bt_utmsgtype	= cnx_vapi->utmsg_vapi()->utmsgtype();
	// sanity check - the cnx_vapi_db MUST contain the bt_utmsgtype_t
	DBG_ASSERT( cnx_vapi_db.find(bt_utmsgtype) != cnx_vapi_db.end() );
	// remove this cnx_vapi from the cnx_vapi_db
	cnx_vapi_db.erase(bt_utmsgtype);
}


NEOIP_NAMESPACE_END





