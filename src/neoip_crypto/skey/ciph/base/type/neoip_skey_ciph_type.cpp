/*! \file
    \brief Definition of the \ref neoip_skey_ciph_type.cpp

*/

/* system include */
/* local include */
#include "neoip_skey_ciph_type.hpp"
#include "neoip_log.hpp"
#include "neoip_assert.hpp"
#include "neoip_string.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief Constructor From String
 */
skey_ciph_type_t::skey_ciph_type_t( const char *type_str )		throw()
{
	std::vector<std::string>	tmp = string_t::split( type_str, "/" );
	if( tmp.size() == 2 ){
		algo		= tmp[0].c_str();
		mode		= tmp[1].c_str();
	}
	if( algo.is_null() || mode.is_null() ){
		KLOG_ERR("Invalid skey_ciph_type_t string =" << type_str);
		nullify();
		return;
	}
}

/** \brief Constructor from type
 */
skey_ciph_type_t::skey_ciph_type_t(skey_ciph_algo_t algo, skey_ciph_mode_t mode)	throw()
{
	this->algo		= algo;
	this->mode		= mode;
}

/** \brief Destructor
 */
skey_ciph_type_t::~skey_ciph_type_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   OSTREAM redirection
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string skey_ciph_type_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return algo.to_string() + "/" + mode.to_string();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   Comparison Operator
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int skey_ciph_type_t::compare( const skey_ciph_type_t & other )  const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// Handle the case where they are not both of the same algo
	if( get_algo() < other.get_algo())	return -1;
	if( get_algo() > other.get_algo())	return +1;
	// NOTE: here both of the same algo

	// Handle the case where they are not both of the same mode
	if( get_mode() < other.get_mode())	return -1;
	if( get_mode() > other.get_mode())	return +1;
	// NOTE: here both of the same mode
	
	// here both at considered equal
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   SERIALIZATION
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief serialize a skey_ciph_type_t
 */
serial_t& operator << (serial_t& serial, const skey_ciph_type_t &ciph_type)	throw()
{
	serial << ciph_type.get_algo();
	serial << ciph_type.get_mode();
	return serial;
}

/** \brief unserialize a skey_ciph_type_t
 */
serial_t& operator >> (serial_t& serial, skey_ciph_type_t &ciph_type) 		throw(serial_except_t)
{
	skey_ciph_algo_t	algo;
	skey_ciph_mode_t	mode;
	serial >> algo;
	serial >> mode;
	// init ciph_type
	ciph_type = skey_ciph_type_t( algo, mode);
	return serial;
}

NEOIP_NAMESPACE_END


