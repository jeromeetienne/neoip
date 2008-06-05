/*! \file
    \brief Definition of the \ref nslan_rec_t
    
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_nslan_rec.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor by default
 */
nslan_rec_t::nslan_rec_t()	throw()
{
}

/** \brief Constructor with payload
 */
nslan_rec_t::nslan_rec_t(const nslan_keyid_t &keyid, const datum_t &payload, const delay_t &ttl)
										throw()
{
	this->keyid	= keyid;
	this->payload	= payload;
	this->ttl	= ttl;
}
	
/** \brief return true if the object is null, false otherwise
 */
bool	nslan_rec_t::is_null()	const throw()
{
	if( keyid.is_null() )	return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::string nslan_rec_t::to_string()			const throw()
{
	std::ostringstream	oss;
	oss << "keyid=" << keyid;
	oss << " ";
	oss << "payload=" << payload;
	oss << " ";
	oss << "ttl=" << ttl;
	return oss.str();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int nslan_rec_t::compare(const nslan_rec_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// compare the keyid
	if( keyid < other.keyid )		return -1;	
	if( keyid > other.keyid )		return +1;	
	// NOTE: here both have the same keyid

	// compare the payload
	if( payload < other.payload )		return -1;	
	if( payload > other.payload )		return +1;	
	// NOTE: here both have the same payload

	// compare the ttl
	if( ttl < other.ttl )		return -1;	
	if( ttl > other.ttl )		return +1;	
	// NOTE: here both have the same keyid


	// here both are equal
	return 0;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serial nslan_rec_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a nslan_rec_t
 */
serial_t& operator << (serial_t& serial, const nslan_rec_t &nslan_rec)	throw()
{
	// serialize the data
	serial << nslan_rec.get_keyid();
	serial << nslan_rec.get_payload();
	serial << nslan_rec.get_ttl();
	// return the object itself
	return serial;
}

/** \brief unserialze a nslan_rec_t
 */
serial_t& operator >> (serial_t& serial, nslan_rec_t &nslan_rec)  		throw(serial_except_t)
{
	nslan_keyid_t	keyid;
	datum_t		payload;
	delay_t		ttl;
	// unserial the data
	serial >> keyid;
	serial >> payload;
	serial >> ttl;
	// copy the unserialed data
	nslan_rec = nslan_rec_t(keyid, payload, ttl);
	// return the object itself
	return serial;
}


NEOIP_NAMESPACE_END;






