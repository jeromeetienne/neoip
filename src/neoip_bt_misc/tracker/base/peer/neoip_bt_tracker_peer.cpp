/*! \file
    \brief Definition of the \ref bt_tracker_peer_t
    
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_bt_tracker_peer.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor with value
 */
bt_tracker_peer_t::bt_tracker_peer_t(const ipport_addr_t &m_ipport, const bt_id_t &m_peerid
					, bool m_is_seed, bool m_want_jamstd)	throw()
{
	// sanity check - the ipport_addr_t MUST be is_fully_qualified()
	DBG_ASSERT( m_ipport.is_fully_qualified() );
	// copy the parameter
	this->m_ipport		= m_ipport;
	this->m_peerid		= m_peerid;
	this->m_is_seed		= m_is_seed;
	this->m_want_jamstd	= m_want_jamstd;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int bt_tracker_peer_t::compare(const bt_tracker_peer_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// NOTE: here both are NOT null
	
	// compare the peerid
	if( peerid() < other.peerid() )			return -1;	
	if( peerid() > other.peerid() )			return +1;	
	// NOTE: here both have the same peerid

	// compare the address
	if( ipport() < other.ipport() )			return -1;	
	if( ipport() > other.ipport() )			return +1;	
	// NOTE: here both have the same ipport

	// compare the is_seed
	if( is_seed() < other.is_seed() )		return -1;	
	if( is_seed() > other.is_seed() )		return +1;	
	// NOTE: here both have the same is_seed

	// compare the want_jamstd
	if( want_jamstd() < other.want_jamstd() )	return -1;	
	if( want_jamstd() > other.want_jamstd() )	return +1;	
	// NOTE: here both have the same want_jamstd

	// here both are equal
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bt_tracker_peer_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss << "ipport="	<< ipport();
	oss << " ";
	oss << "peerid="	<< peerid();
	oss << " ";
	oss << "is_seed="	<< (is_seed() ? "yes" : "no");
	oss << " ";
	oss << "want_jamstd="	<< (want_jamstd() ? "yes" : "no");
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






