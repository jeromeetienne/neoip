/*! \file
    \brief Definition of the \ref bt_peersrc_helper_t class
    
*/

/* system include */
/* local include */
#include "neoip_bt_peersrc_helper.hpp"
#include "neoip_bt_peersrc_peer_arr.hpp"
#include "neoip_ipport_addr_arr.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       compact format stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a ipport_addr_arr_t to a std::string in 'compact format'
 */
std::string bt_peersrc_helper_t::ipport_addr_arr_to_compactfmt(const ipport_addr_arr_t &ipport_addr_arr) throw()
{
	bytearray_t	bytearray;
	// go through the whole ipport_addr_arr_t
	for(size_t i = 0; i < ipport_addr_arr.size(); i++ ){
		const ipport_addr_t &	ipport_addr	= ipport_addr_arr[i];
		uint32_t		ipaddr		= ipport_addr.get_ipaddr().get_v4_addr();
		uint16_t		port		= ipport_addr.get_port();
		// sanity check - the ipport_addr MUST be fully qualified
		DBG_ASSERT( ipport_addr.is_fully_qualified() );
		// put the ipaddr and port in the bytearray
		bytearray << ipaddr;
		bytearray << port;
	}
	// return the result
	return bytearray.to_stdstring();
}

/** \brief Generate a ipport_addr_arr_t from a std::string in 'compact format'
 * 
 * @return the generated ipport_addr_arr_t or a null one if an error occurs
 */
ipport_addr_arr_t bt_peersrc_helper_t::ipport_addr_arr_from_compactfmt(const std::string &str_compactfmt)	throw()
{
	ipport_addr_arr_t	ipport_addr_arr;
	// convert the datum into a bytearray_t to ease the parsing
	bytearray_t 		bytearray(str_compactfmt);
	// if the bytearray length is not multiple of 6 (4byte ip address, 2byte port), return error
	if( bytearray.length() % 6 )	return ipport_addr_arr_t();
	// parse the bytearray_t - it is a suite of ipv4 address / port
	while( !bytearray.empty() ){
		uint32_t	ipaddr;
		uint16_t	port;
		// unserialize the ipaddr and port
		// - no need to test for serial_except_t as the bytearray_t is a multiple 6
		bytearray >> ipaddr >> port;
		// build the resulting ipport_addr_t
		ipport_addr_t	ipport_addr(ip_addr_t(ipaddr), port);
		// if the ipport_addr_t is not is_fully_qualified, discard it
		// - NOTE: this is a workaround a bug seen in utorrent1.6 utpex which gives port = 0
		if( !ipport_addr.is_fully_qualified() )	continue;
		// build the ipport_addr_r to the arrays
		ipport_addr_arr	+= ipport_addr_t(ip_addr_t(ipaddr), port);
	}
	// return the just built ipport_addr_arr_t
	return ipport_addr_arr;
}

/** \brief Convert a bt_peersrc_peer_arr_t to a std::string in 'compact format'
 */
std::string	bt_peersrc_helper_t::peer_arr_to_compactfmt(const bt_peersrc_peer_arr_t &peer_arr)	throw()
{
	ipport_addr_arr_t	ipport_addr_arr;
	// convert the bt_peersrc_peer_arr_t in a ipport_addr_arr_t
	for(size_t i = 0; i < peer_arr.size(); i++){
		const socket_addr_t & 	dest_addr	= peer_arr[i].dest_addr();
		// convert it to a ipport_addr_t
		ipport_addr_t		ipport_addr	= dest_addr.get_peerid_vapi()->to_string()
							+ std::string(":")
							+ dest_addr.get_portid_vapi()->to_string();
		DBG_ASSERT( ipport_addr.is_fully_qualified() );
		// add this ipport_addr_t to the array
		ipport_addr_arr	+= ipport_addr;
	}

	// return std::string in compact format
	return ipport_addr_arr_to_compactfmt(ipport_addr_arr);
}

/** \brief Generate a bt_peersrc_peer_arr_t from a std::string in 'compact format'
 * 
 * @return the generated bt_peersrc_peer_arr_t or a null one if an error occurs
 */
bt_peersrc_peer_arr_t	bt_peersrc_helper_t::peer_arr_from_compactfmt(const std::string &str_compactfmt)throw()
{
	ipport_addr_arr_t	ipport_addr_arr;
	bt_peersrc_peer_arr_t	peer_arr;
	// convert the str_compactfmt into a ipport_addr_arr_t
	ipport_addr_arr	= ipport_addr_arr_from_compactfmt(str_compactfmt);
	// convert the ipport_addr_arr_t into a bt_peersrc_peer_arr_t
	for(size_t i = 0; i < ipport_addr_arr.size(); i++){
		const ipport_addr_t &	ipport_addr	= ipport_addr_arr[i];
		socket_addr_t		listen_addr	= std::string("stcp://") + ipport_addr.to_string();
		peer_arr	+= bt_peersrc_peer_t(bt_id_t(), listen_addr);
	}
	// return the just built bt_peersrc_peer_arr_t
	return peer_arr;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			peerflag stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert a bt_peersrc_peer_arr_t to a std::string in 'peerflag'
 */
std::string	bt_peersrc_helper_t::peer_arr_to_peerflag(const bt_peersrc_peer_arr_t &peer_arr)	throw()
{
	bytearray_t	bytearray;
	// go through the whole ipport_addr_arr_t
	for(size_t i = 0; i < peer_arr.size(); i++ ){
		const bt_peersrc_peer_t & peersrc_peer	= peer_arr[i];
		// build the peerflag for this bt_peersrc_peer_t
		uint8_t	peerflag	= 0;
		if( peersrc_peer.want_jamstd())	peerflag |= 0x01;
		if( peersrc_peer.is_seed() )	peerflag |= 0x02;
		// put the peerflag in the bytearray
		bytearray << peerflag;
	}
	// return the result
	return bytearray.to_stdstring();
}


/** \brief Convert a bt_peersrc_peer_arr_t to a std::string in 'peerflag'
 */
void	bt_peersrc_helper_t::peer_arr_from_peerflag(bt_peersrc_peer_arr_t &peer_arr
					, const std::string &str_peerflag)	throw()
{
	// if str_peerflag size is different from peer_arr, return now
	if( str_peerflag.size() != peer_arr.size() )	return;
	// parse the bytearray_t - it is a suite of ipv4 address / port
	for(size_t i = 0; i < peer_arr.size(); i++){ 
		bt_peersrc_peer_t &	peersrc_peer	= peer_arr[i];
		uint8_t			peerflag	= str_peerflag[i];
		// convert peerflag to update bt_peersrc_peer_t
		peersrc_peer.is_seed		( (peerflag & 0x02) ? true : false ); 
		peersrc_peer.want_jamstd	( (peerflag & 0x01) ? true : false ); 
	}
}


NEOIP_NAMESPACE_END


