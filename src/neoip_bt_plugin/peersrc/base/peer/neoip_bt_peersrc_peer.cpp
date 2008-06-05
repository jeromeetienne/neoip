/*! \file
    \brief Definition of the \ref bt_peersrc_peer_t

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_peersrc_peer.hpp"
#include "neoip_bt_tracker_peer.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor with values
 */
bt_peersrc_peer_t::bt_peersrc_peer_t(const bt_id_t &peerid, const socket_addr_t &dest_addr
					, bool is_seed, bool want_jamstd)	throw()
{
	this->peerid		(peerid);
	this->dest_addr		(dest_addr);
	this->is_seed		(is_seed);
	this->want_jamstd	(want_jamstd);
}

/** \brief Constructor from a bt_tracker_peer_t
 */
bt_peersrc_peer_t::bt_peersrc_peer_t(const bt_tracker_peer_t &tracker_peer)	throw()
{
	peerid		(tracker_peer.get_peerid());
	dest_addr	(std::string("stcp://")+tracker_peer.get_ipport().to_string());
	is_seed		(tracker_peer.is_seed());
	want_jamstd	(tracker_peer.want_jamstd());
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int bt_peersrc_peer_t::compare(const bt_peersrc_peer_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// compare the peerid
	if( peerid() < other.peerid() )			return -1;
	if( peerid() > other.peerid() )			return +1;
	// NOTE: here both peerid are equal

	// compare the dest_addr
	if( dest_addr() < other.dest_addr() )		return -1;
	if( dest_addr() > other.dest_addr() )		return +1;
	// NOTE: here both dest_addr are equal

	// compare the is_seed
	if( is_seed() < other.is_seed() )		return -1;
	if( is_seed() > other.is_seed() )		return +1;
	// NOTE: here both is_seed are equal

	// compare the want_jamstd
	if( want_jamstd() < other.want_jamstd() )	return -1;
	if( want_jamstd() > other.want_jamstd() )	return +1;
	// NOTE: here both want_jamstd are equal

	// here both are considered equal
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	bt_peersrc_peer_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( dest_addr().is_null() )	return true;
	// if this point is reached, the object is NOT null
	return false;
}
	

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	bt_peersrc_peer_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[";
	oss << "dest_addr="	<< dest_addr();
	oss << " ";
	oss << "peerid="	<< peerid();
	oss << " ";
	oss << "is_seed="	<< (is_seed() ? "yes": "no");
	oss << " ";
	oss << "want_jamstd="	<< (want_jamstd() ? "yes": "no");
	oss << "]";
	
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bt_peersrc_peer_t
 * 
 * - support null bt_peersrc_peer_t
 */
serial_t& operator << (serial_t& serial, const bt_peersrc_peer_t &peersrc_peer)		throw()
{
	// serialize each field of the object
	serial << peersrc_peer.dest_addr();
	serial << peersrc_peer.peerid();
	serial << peersrc_peer.is_seed();
	serial << peersrc_peer.want_jamstd();
	// return serial
	return serial;
}

/** \brief unserialze a bt_peersrc_peer_t
 * 
 * - support null bt_peersrc_peer_t
 */
serial_t& operator >> (serial_t & serial, bt_peersrc_peer_t &peersrc_peer)		throw(serial_except_t)
{
	socket_addr_t	dest_addr;
	bt_id_t		peerid;
	bool		is_seed;
	bool		want_jamstd;

	// reset the destination variable
	peersrc_peer	= bt_peersrc_peer_t();

	// unserialize the data
	serial >> dest_addr;
	serial >> peerid;
	serial >> is_seed;
	serial >> want_jamstd;

	// set the returned variable
	peersrc_peer.dest_addr(dest_addr).peerid(peerid)
			.is_seed(is_seed)
			.want_jamstd(want_jamstd);
	// return serial
	return serial;
}
NEOIP_NAMESPACE_END

