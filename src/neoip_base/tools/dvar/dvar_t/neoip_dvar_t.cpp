/*! \file
    \brief Declaration of the dvar_t
*/

/* system include */
/* local include */
#include "neoip_dvar_t.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_dvar_int.hpp"
#include "neoip_dvar_dbl.hpp"
#include "neoip_dvar_str.hpp"
#include "neoip_dvar_boolean.hpp"
#include "neoip_dvar_nil.hpp"
#include "neoip_dvar_arr.hpp"
#include "neoip_dvar_map.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
dvar_t::dvar_t()	throw()
{
	// zero some field
	dvar_vapi	= NULL;
}

/** \brief Constructor
 */
dvar_t::~dvar_t()	throw()
{
	// free the dvar_vapi_t if needed
	nipmem_zdelete	dvar_vapi;
}

/** \brief init the dvar_t from a dvar_vapi_t
 */
dvar_t::dvar_t(const dvar_vapi_t &other_vapi)	throw()
{
	dvar_vapi	= other_vapi.clone();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          copy stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Copy constructor
 */
dvar_t::dvar_t(const dvar_t &other)	throw()
{
	if( !other.is_null() )	dvar_vapi	= other.dvar_vapi->clone();
	else			dvar_vapi	= NULL;
}


/** \brief assignement operator
 */
dvar_t &	dvar_t::operator = (const dvar_t & other)	throw()
{
	// if it is a self assignement, do nothing
	if( this == &other )	return *this;

	// nullify the current object
	nipmem_zdelete	dvar_vapi;
	// copy it
	if( !other.is_null() )	dvar_vapi	= other.dvar_vapi->clone();
	else			dvar_vapi	= NULL;

	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the type this dvar_t
 */
dvar_type_t	dvar_t::type()		const throw()
{
	// if this dvar_t is null, return dvar_type_t::NONE
	if( is_null() )		return dvar_type_t::NONE;
	// else return the actual type of this dvar_t
	return dvar_vapi->type();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      compare() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Compare the object ala memcmp
 */
int 	dvar_t::compare(const dvar_t &other)	const throw()
{
	// handle the null case
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are not null

	// if they are not of the same type, the lowest type value is the lowest dvar_t
	if( type().get_value() < other.type().get_value() )	return -1;
	if( type().get_value() > other.type().get_value() )	return +1;
	// NOTE: here both are of the same dvar_type_t

	// use the dvar_vapi_t::compare
	return dvar_vapi->compare(*other.dvar_vapi);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       convertion to dvar_type_t::INTEGER
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the dvar_t into a dvar_int_t
 *
 * - the dvar_t type MUST be dvar_type_t::INTEGER to call this function
 */
dvar_int_t &	dvar_t::integer()	throw()
{
	// sanity check - the type MUST be dvar_type_t::INTEGER
	DBG_ASSERT( type() == dvar_type_t::INTEGER );
	// return a reference to the dvar_int_t
	return dynamic_cast <dvar_int_t &>(*dvar_vapi);
}

/** \brief convert the dvar_t into a dvar_int_t
 *
 * - the dvar_t type MUST be dvar_type_t::INTEGER to call this function
 */
const dvar_int_t &	dvar_t::integer()	const throw()
{
	// sanity check - the type MUST be dvar_type_t::INTEGER
	DBG_ASSERT( type() == dvar_type_t::INTEGER );
	// return a reference to the dvar_int_t
	return dynamic_cast <const dvar_int_t &>(*dvar_vapi);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       convertion to dvar_type_t::DOUBLE
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the dvar_t into a dvar_dbl_t
 *
 * - the dvar_t type MUST be dvar_type_t::DOUBLE to call this function
 */
dvar_dbl_t &	dvar_t::dbl()	throw()
{
	// sanity check - the type MUST be dvar_type_t::DOUBLE
	DBG_ASSERT( type() == dvar_type_t::DOUBLE );
	// return a reference to the dvar_dbl_t
	return dynamic_cast <dvar_dbl_t &>(*dvar_vapi);
}

/** \brief convert the dvar_t into a dvar_dbl_t
 *
 * - the dvar_t type MUST be dvar_type_t::DOUBLE to call this function
 */
const dvar_dbl_t &	dvar_t::dbl()	const throw()
{
	// sanity check - the type MUST be dvar_type_t::DOUBLE
	DBG_ASSERT( type() == dvar_type_t::DOUBLE );
	// return a reference to the dvar_dbl_t
	return dynamic_cast <const dvar_dbl_t &>(*dvar_vapi);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       convertion to dvar_type_t::STRING
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the dvar_t into a dvar_str_t
 *
 * - the dvar_t type MUST be dvar_type_t::STRING to call this function
 */
dvar_str_t &	dvar_t::str()	throw()
{
	// sanity check - the type MUST be dvar_type_t::STRING
	DBG_ASSERT( type() == dvar_type_t::STRING );
	// return a reference to the dvar_str_t
	return dynamic_cast <dvar_str_t &>(*dvar_vapi);
}

/** \brief convert the dvar_t into a dvar_str_t
 *
 * - the dvar_t type MUST be dvar_type_t::STRING to call this function
 */
const dvar_str_t &	dvar_t::str()	const throw()
{
	// sanity check - the type MUST be dvar_type_t::STRING
	DBG_ASSERT( type() == dvar_type_t::STRING );
	// return a reference to the dvar_str_t
	return dynamic_cast <const dvar_str_t &>(*dvar_vapi);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       convertion to dvar_type_t::BOOLEAN
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the dvar_t into a dvar_boolean_t
 *
 * - the dvar_t type MUST be dvar_type_t::BOOLEAN to call this function
 */
dvar_boolean_t &	dvar_t::boolean()	throw()
{
	// sanity check - the type MUST be dvar_type_t::BOOLEAN
	DBG_ASSERT( type() == dvar_type_t::BOOLEAN );
	// return a reference to the dvar_boolean_t
	return dynamic_cast <dvar_boolean_t &>(*dvar_vapi);
}

/** \brief convert the dvar_t into a dvar_boolean_t
 *
 * - the dvar_t type MUST be dvar_type_t::BOOLEAN to call this function
 */
const dvar_boolean_t &	dvar_t::boolean()	const throw()
{
	// sanity check - the type MUST be dvar_type_t::BOOLEAN
	DBG_ASSERT( type() == dvar_type_t::BOOLEAN );
	// return a reference to the dvar_boolean_t
	return dynamic_cast <const dvar_boolean_t &>(*dvar_vapi);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       convertion to dvar_type_t::NIL
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the dvar_t into a dvar_str_t
 *
 * - the dvar_t type MUST be dvar_type_t::NIL to call this function
 */
dvar_nil_t &	dvar_t::nil()	throw()
{
	// sanity check - the type MUST be dvar_type_t::NIL
	DBG_ASSERT( type() == dvar_type_t::NIL );
	// return a reference to the dvar_nil_t
	return dynamic_cast <dvar_nil_t &>(*dvar_vapi);
}

/** \brief convert the dvar_t into a dvar_nil_t
 *
 * - the dvar_t type MUST be dvar_type_t::NIL to call this function
 */
const dvar_nil_t &	dvar_t::nil()	const throw()
{
	// sanity check - the type MUST be dvar_type_t::NIL
	DBG_ASSERT( type() == dvar_type_t::NIL );
	// return a reference to the dvar_nil_t
	return dynamic_cast <const dvar_nil_t &>(*dvar_vapi);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       convertion to dvar_type_t::ARRAY
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the dvar_t into a dvar_arr_t
 *
 * - the dvar_t type MUST be dvar_type_t::ARRAY to call this function
 */
dvar_arr_t &	dvar_t::arr()	throw()
{
	// sanity check - the type MUST be dvar_type_t::ARRAY
	DBG_ASSERT( type() == dvar_type_t::ARRAY );
	// return a reference to the dvar_arr_t
	return dynamic_cast <dvar_arr_t &>(*dvar_vapi);
}

/** \brief convert the dvar_t into a dvar_arr_t
 *
 * - the dvar_t type MUST be dvar_type_t::ARRAY to call this function
 */
const dvar_arr_t &	dvar_t::arr()	const throw()
{
	// sanity check - the type MUST be dvar_type_t::ARRAY
	DBG_ASSERT( type() == dvar_type_t::ARRAY );
	// return a reference to the dvar_arr_t
	return dynamic_cast <const dvar_arr_t &>(*dvar_vapi);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       convertion to dvar_type_t::MAP
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the dvar_t into a dvar_map_t
 *
 * - the dvar_t type MUST be dvar_type_t::MAP to call this function
 */
dvar_map_t &	dvar_t::map()	throw()
{
	// sanity check - the type MUST be dvar_type_t::MAP
	DBG_ASSERT( type() == dvar_type_t::MAP );
	// return a reference to the dvar_map_t
	return dynamic_cast <dvar_map_t &>(*dvar_vapi);
}

/** \brief convert the dvar_t into a dvar_map_t
 *
 * - the dvar_t type MUST be dvar_type_t::MAP to call this function
 */
const dvar_map_t &	dvar_t::map()	const throw()
{
	// sanity check - the type MUST be dvar_type_t::MAP
	DBG_ASSERT( type() == dvar_type_t::MAP );
	// return a reference to the dvar_map_t
	return dynamic_cast <const dvar_map_t &>(*dvar_vapi);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a string
 */
std::string	dvar_t::to_string()	const throw()
{
	// if the dvar_t is null, simply return "null"
	if( is_null() )	return "null";
	// else call the dvar_vapi_t::to_string()
	return dvar_vapi->to_string();
}

NEOIP_NAMESPACE_END







