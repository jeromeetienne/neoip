/*! \file
    \brief Definition of the \ref clineopt_t

*/

/* system include */
#include <sstream>
/* local include */
#include "neoip_clineopt.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if this clineopt_t may handle a given string
 */
bool	clineopt_t::may_handle(const std::string &str)			const throw()
{
	// if this str == canonical_name(), then return true
	if( str == canonical_name() )	return true;
	// if this str is one of the alias_name, return true
	for(size_t i = 0; i < alias_name_db().size(); i++){
		if( str == alias_name_db()[i] )	return true;
	}
	// if there are no matches, return false
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	clineopt_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss        << "canonical_name="	<< canonical_name();
	oss << " " << "alias_name_db="	<< alias_name_db();
	oss << " " << "default_value="	<< default_value();
	oss << " " << "help_string="	<< help_string();
	oss << " " << "param_mode="	<< param_mode();
	oss << " " << "option_mode="	<< option_mode();
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END

