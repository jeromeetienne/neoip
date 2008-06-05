/*! \file
    \brief Definition of the \ref bt_tracker_reply_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_tracker_reply.hpp"
#include "neoip_bt_tracker_request.hpp"
#include "neoip_bencode.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_dvar.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	bt_tracker_reply_t::is_null()	const throw()
{
	if( request_period().is_null() && failure_reason().empty() )	return true;
	// if this point is reached, the object is NOT null
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          bencode function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a bencoded std::string of the bt_tracker_reply_t
 * 
 * - the bt_tracker_request_t is used to determine the options to build the request
 *   - compact or nopeerid
 */
datum_t	bt_tracker_reply_t::to_bencode(const bt_tracker_request_t &request)	const throw()
{
	std::ostringstream	oss;
	dvar_t			dvar	= dvar_map_t();
	// sanity check - this bt_tracker_reply_t MUST not be null
	DBG_ASSERT( this->is_null() == false );
	// if this bt_tracker_reply_t failed
	if( !failure_reason().empty() ){
		dvar.map().insert("failure reason", dvar_str_t(failure_reason()));
	}else{
		dvar.map().insert("interval"	, dvar_int_t(request_period().to_sec_32bit()));
		dvar.map().insert("complete"	, dvar_int_t(nb_seeder()));
		dvar.map().insert("incomplete"	, dvar_int_t(nb_leecher()));
		dvar.map().insert("peers"	, peer_arr_to_dvar(request));
	}
	
	// bencode the just built dvar_t and return it
	return datum_t( bencode_t::from_dvar(dvar) );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    convert bt_tracker_peer_arr_t to dvar_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Return a nocompact dvar_t representing the peer_arr of this bt_tracker_reply_t
 * 
 * - the bt_tracker_request_t is used to determine the options to build the request
 *   - compact or nopeerid
 */
dvar_t	bt_tracker_reply_t::peer_arr_to_dvar(const bt_tracker_request_t &request)	const throw()
{
	if( request.compact() )	return peer_arr_to_dvar_docompact(request);
	else			return peer_arr_to_dvar_nocompact(request);
}

/** \brief Return a nocompact dvar_t representing the peer_arr of this bt_tracker_reply_t
 * 
 * - the bt_tracker_request_t is used to determine the options to build the request
 *   - compact or nopeerid
 */
dvar_t	bt_tracker_reply_t::peer_arr_to_dvar_nocompact(const bt_tracker_request_t &request)	const throw()
{
	dvar_t	arr_dvar	= dvar_arr_t();
	// sanity check - here the request.compact() MUST NOT be set
	DBG_ASSERT( !request.compact() );
	// go through the whole bt_tracker_peer_arr_t
	for(size_t i = 0; i < peer_arr().size(); i++ ){
		const bt_tracker_peer_t &	peer	= peer_arr()[i];
		// sanity check - the bt_tracker_peer_t ipport_addr_t MUST be is_fully_qualified()
		DBG_ASSERT( peer.get_ipport().is_fully_qualified() );
		// build the dvar_t for this bt_tracker_peer_t
		dvar_t	peer_dvar	= dvar_map_t();
		peer_dvar.map().insert("ip"	, dvar_str_t(peer.get_ipport().get_ipaddr().to_string()));
		peer_dvar.map().insert("port"	, dvar_int_t(peer.get_ipport().get_port()));
		if( !request.nopeerid() )
			peer_dvar.map().insert("peer id", dvar_str_t( string_t::from_datum(peer.get_peerid().to_datum()) ));
		// add this peer_dvar to the arr_dvar
		arr_dvar.arr().append( peer_dvar );
	}
	// return the result
	return arr_dvar;
}

/** \brief Return a compact dvar_t representing the peer_arr of this bt_tracker_reply_t
 * 
 * - the bt_tracker_request_t is used to determine the options to build the request
 *   - compact or nopeerid
 */
dvar_t	bt_tracker_reply_t::peer_arr_to_dvar_docompact(const bt_tracker_request_t &request)	const throw()
{
	bytearray_t	bytearray;
	// sanity check - here the request.compact() MUST be set
	DBG_ASSERT( request.compact() );
	// go through the whole bt_tracker_peer_arr_t
	for(size_t i = 0; i < peer_arr().size(); i++ ){
		const bt_tracker_peer_t&peer	= peer_arr()[i];
		uint32_t		ipaddr	= peer.get_ipport().get_ipaddr().get_v4_addr();
		uint16_t		port	= peer.get_ipport().get_port();
		// sanity check - the bt_tracker_peer_t ipport_addr_t MUST be is_fully_qualified()
		DBG_ASSERT( peer.get_ipport().is_fully_qualified() );
		// put the ipaddr and port in the bytearray
		bytearray << ipaddr << port;
	}
	// return the result
	return dvar_str_t(string_t::from_datum(bytearray.to_datum()));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             from_bencode
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a trackbr_reply_t from a bencoded string
 * 
 * - return a null one if the parsing fails.
 */
bt_tracker_reply_t	bt_tracker_reply_t::from_bencode(const datum_t &datum)		throw()
{
	// convert the http reply message body to a dvar_t
	dvar_t	reply_dvar	= bencode_t::to_dvar(datum);
	
	// log to debug
	KLOG_DBG("after reply_dvar=" << reply_dvar);

	// if the convertion failed or if the reply is poorly formed, notify the caller
	if( reply_dvar.is_null()||  reply_dvar.type() != dvar_type_t::MAP
				||!(reply_dvar.map().contain("failure reason", dvar_type_t::STRING)
				    || reply_dvar.map().contain("peers"))
				){
		return bt_tracker_reply_t();
	}

	// handle if the reply contains a faillure reason
	if( reply_dvar.map().contain("failure reason", dvar_type_t::STRING) ){
		bt_tracker_reply_t	reply;
		reply.failure_reason	(reply_dvar.map()["failure reason"].str().get());
		return reply;
	}
	
	// build the bt_tracker_reply_t if no error occured
	bt_tracker_reply_t	reply;
	// parse the peer_arr
	reply.peer_arr		(dvar_to_peer_arr(reply_dvar.map()["peers"]));
	// parse the request period - supposed to be mandatory but some tracker dont provide it
	if( reply_dvar.map().contain("interval", dvar_type_t::INTEGER) ){
		reply.request_period	(delay_t::from_sec(reply_dvar.map()["interval"].integer().get()));
	}else{
		// TODO what is this constant, shouldnt it be a default in profile or elsewhere ?
		reply.request_period	(delay_t::from_min(42));
	}
	// parse the nb_seeded - supposed to be mandatory but some tracker dont provide it
	if( reply_dvar.map().contain("complete", dvar_type_t::INTEGER) ){
		reply.nb_seeder	(reply_dvar.map()["complete"].integer().get());
	}else{
		reply.nb_seeder	( 0 );
	}
	// parse the nb_leecher - supposed to be mandatory but some tracker dont provide it
	if( reply_dvar.map().contain("incomplete", dvar_type_t::INTEGER) ){
		reply.nb_leecher(reply_dvar.map()["incomplete"].integer().get());
	}else{
		reply.nb_leecher( 0 );
	}
	// return the built result
	return reply;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 parse the various flaver of the peers_dvar
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse the "peers" fields returned by the http reply from the tracker
 */
bt_tracker_peer_arr_t	bt_tracker_reply_t::dvar_to_peer_arr(const dvar_t &peers_dvar)	throw()
{
	// log to debug
	KLOG_DBG("peers_dvar=" << peers_dvar);
	// parse the peers_dvar depending on its type
	if( peers_dvar.type() == dvar_type_t::ARRAY )	return dvar_to_peer_arr_nocompact(peers_dvar);
	if( peers_dvar.type() == dvar_type_t::STRING )	return dvar_to_peer_arr_docompact(peers_dvar);
	// NOTE: this point MUST NOT be reached
	DBGNET_ASSERT( 0 );
	return bt_tracker_peer_arr_t();
}

/** \brief Parse the "peers" fields returned by the http reply from the tracker
 * 
 * - parse the 'original version' aka an array of dictionnary containing ip/port/peer id keys
 *   - additionnaly support the 'no_peer_id' option in which the reply doesnt include the peerid
 */
bt_tracker_peer_arr_t	bt_tracker_reply_t::dvar_to_peer_arr_nocompact(const dvar_t &peers_dvar)	throw()
{
	bt_tracker_peer_arr_t	peer_arr;	
	// log to debug
	KLOG_DBG("peers_dvar=" << peers_dvar);
	// sanity check - the peers_dvar MUST be a dvar_type_t::ARRAY
	DBG_ASSERT( peers_dvar.type() == dvar_type_t::ARRAY );
	// go thru all the element of the array
	for(size_t i = 0; i < peers_dvar.arr().size(); i++){
		const dvar_t	dvar	= peers_dvar.arr()[i];
		ip_addr_t	ip_addr;
		uint16_t	port;
		bt_id_t	peerid;
		// sanity check - on the dvar variable
		if( dvar.is_null() || dvar.type() != dvar_type_t::MAP
				|| !dvar.map().contain("ip")
				||  dvar.map()["ip"].type()		!= dvar_type_t::STRING
				|| !dvar.map().contain("port")
				||  dvar.map()["port"].type()		!= dvar_type_t::INTEGER
				|| (dvar.map().contain("peer id")
				    && dvar.map()["peer id"].type()	!= dvar_type_t::STRING)
				){
			DBGNET_ASSERT( 0 );
			return bt_tracker_peer_arr_t();
		}
		// get the ip_addr and port from the dvar
		ip_addr	= dvar.map()["ip"].str().get();
		port	= dvar.map()["port"].integer().get();
		// parse the peerid if it is present - it is optionnal 
		// - if the profile.nopeerid_request() is set, the reply doesnt contains a peerid
		if( dvar.map().contain("peer id") )
			peerid	= bt_id_t( datum_t(dvar.map()["peer id"].str().get()) );
		else
			peerid	= bt_id_t();
		// build the resulting ipport_addr_t
		ipport_addr_t	ipport_addr(ip_addr, port);
		// if the ipport_addr_t is not is_fully_qualified, discard it
		// - NOTE: this is a workaround a bug seen where http tracker which gives port = 0
		//   - a similar bug has been seen in utpex from utorrent1.6
		if( !ipport_addr.is_fully_qualified() )	continue;
		// add the bt_tracker_peer_t in the bt_tracker_peer_arr_t
		peer_arr	+= bt_tracker_peer_t( ipport_addr, peerid );
	}
	// return the result
	return peer_arr;
}

/** \brief Parse the "peers" fields returned by the http reply from the tracker
 * 
 * - when it is a 'compact' reply
 */
bt_tracker_peer_arr_t	bt_tracker_reply_t::dvar_to_peer_arr_docompact(const dvar_t &peers_dvar)	throw()
{
	bt_tracker_peer_arr_t	peer_arr;
	// log to debug
	KLOG_DBG("peers_dvar=" << peers_dvar);
	// sanity check - the peers_dvar MUST be a dvar_type_t::STRING
	DBG_ASSERT( peers_dvar.type() == dvar_type_t::STRING );
	// convert the peers_dvar into a bytearray_t to ease the parsing	
	bytearray_t	bytearray	= bytearray_t(datum_t(peers_dvar.str().get()));
	// parse the bytearray_t - it is a suite of ipv4 address / port
	while( !bytearray.empty() ){
		uint32_t	ipaddr;
		uint16_t	port;
		try {
			bytearray >> ipaddr;
			bytearray >> port;
		}catch(serial_except_t &e){
			DBGNET_ASSERT( 0 );
			return bt_tracker_peer_arr_t();
		}
		// add the bt_tracker_peer_t in the bt_tracker_peer_arr_t
		ipport_addr_t	ipport_addr(ip_addr_t(ipaddr), port);
		// if the ipport_addr_t is not is_fully_qualified, discard it
		// - NOTE: this is a workaround a bug seen where http tracker which gives port = 0
		//   - a similar bug has been seen in utpex from utorrent1.6
		if( !ipport_addr.is_fully_qualified() )	continue;
		// add the new bt_tracker_peer_t in the peer_arr
		peer_arr	+= bt_tracker_peer_t( ipport_addr, bt_id_t() );
	}
	// return the result 
	return peer_arr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	bt_tracker_reply_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	if( !failure_reason().empty() ){
		oss        << "failure_reason="<< failure_reason();
	}else{
		oss        << "request_period="	<< request_period();
		oss << " " << "nb_seeder="	<< nb_seeder();
		oss << " " << "nb_leecher="	<< nb_leecher();
		oss << " " << "peer_arr="	<< peer_arr();
	}
	// return the just built string
	return oss.str();
}
NEOIP_NAMESPACE_END

