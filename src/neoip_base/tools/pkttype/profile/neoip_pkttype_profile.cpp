/*! \file
    \brief Implementation of the pkttype_profile_t
    
*/

/* system include */
/* local include */
#include "neoip_pkttype_profile.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
pkttype_profile_t::pkttype_profile_t()						throw()
{
	// init some field
	serial_type_val	= NONE;
}

/** \brief Constructor with values
 */
pkttype_profile_t::pkttype_profile_t(size_t offset, size_t nb_reserved, serial_type_t serial_type) throw()
{
	offset_val	= offset;
	nb_reserved_val	= nb_reserved;
	serial_type_val	= serial_type;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      compare() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Compare the object ala memcmp
 */
int	pkttype_profile_t::compare(const pkttype_profile_t &other)	  const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// compare the offset
	if( offset() < other.offset() )		return -1;
	if( offset() > other.offset() )		return +1;
	// NOTE: here both have the same offset

	// compare the nb_reserved
	if( nb_reserved() < other.nb_reserved() )	return -1;
	if( nb_reserved() > other.nb_reserved() )	return +1;
	// NOTE: here both have the same nb_reserved

	// compare the serial_type
	if( serial_type() < other.serial_type() )	return -1;
	if( serial_type() > other.serial_type() )	return +1;
	// NOTE: here both have the same serial_type

	// here both are considered equal, return 0
	return 0;	
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    Display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Convert an object to a string
 */
std::string	pkttype_profile_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// display the begining of the database
	oss << "offset="	<< offset();
	oss << " ";
	oss << "nb_reserved="	<< nb_reserved();
	oss << " ";
	oss << "serial_type=";
	switch(serial_type()){
	case UINT8:	oss << "uint8";		break;
	case UINT16:	oss << "uint16";	break;
	case UINT32:	oss << "uint32";	break;
	default:	DBG_ASSERT( 0 );
	}
	// return the just built string
	return oss.str();	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  serialization functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Serialize a value from pkttype
 */
serial_t &pkttype_profile_t::serialize(serial_t &serial, size_t val)		const throw()
{
	// sanity check - the value MUST be in the interval of the profile
	DBG_ASSERT( val >= offset() && val < offset() + nb_reserved() );
	// sanity check - the profile MUST be set
	DBG_ASSERT( !is_null() );	
	// serialize the value depending on the serial_type
	switch(serial_type()){
	case UINT8:	serial << uint8_t(val);		break;
	case UINT16:	serial << uint16_t(val);	break;
	case UINT32:	serial << uint32_t(val);	break;
	default:	DBG_ASSERT( 0 );
	}
	// return the serial_t
	return serial;
}

/** \brief Unserialize a value from pkttype and return it
 */
size_t	pkttype_profile_t::unserialize(serial_t &serial)			throw(serial_except_t)
{
	size_t	val	= 0;	// just to stop gcc from crying about uninitialized value
	// sanity check - the profile MUST be set
	DBG_ASSERT( !is_null() );	
	// unserialize the value depending on the serial_type	
	switch(serial_type()){
	case UINT8:{	uint8_t	tmp;
			serial >> tmp;
			val	= tmp;
			break;}
	case UINT16:{	uint16_t tmp;
			serial >> tmp;
			val	= tmp;
			break;}
	case UINT32:{	uint32_t tmp;
			serial >> tmp;
			val	= tmp;
			break;}
	default:	DBG_ASSERT( 0 );
	}
	// sanity check - the value MUST be in the interval of the profile
	DBGNET_ASSERT( val >= offset() && val < offset() + nb_reserved() );
	// return the result
	return val;
}


NEOIP_NAMESPACE_END





