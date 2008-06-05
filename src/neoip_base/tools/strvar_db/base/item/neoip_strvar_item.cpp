/*! \file
    \brief Declaration of the strvar_item_t

*/

/* system include */
/* local include */
#include "neoip_strvar_item.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
strvar_item_t::strvar_item_t(const std::string &key_var, const std::string &val_var)		throw()
{
	// sanity check - key_var MUST NOT be empty
	DBG_ASSERT( !key_var.empty() );
	// sanity check - val_var MUST NOT be empty
//	DBG_ASSERT( !val_var.empty() );
	// copy the parameter
	this->key_var	= key_var;
	this->val_var	= val_var;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main compare function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 objects (ala memcmp)
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int strvar_item_t::compare( const strvar_item_t & other )  const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are not null

	if( key() < other.key() )		return -1;
	if( key() > other.key() )		return -1;
	// NOTE: here both key() are equal

	if( val() < other.val() )		return -1;
	if( val() > other.val() )		return -1;
	// NOTE: here both val() are equal

	// note: here both are considered equal
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a string containing all the strvar_item
 */
std::string strvar_item_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss << "\"" << key() << "\"=\"" << val() << "\"";
	// return the just build string
	return oss.str();
}


NEOIP_NAMESPACE_END







