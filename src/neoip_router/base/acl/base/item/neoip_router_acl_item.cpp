/*! \file
    \brief Definition of the \ref router_acl_item_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_router_acl_item.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	router_acl_item_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss << "[";
	oss << "type="		<< type();
	oss << " pattern="	<< pattern();
	oss << "]";
	
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END

