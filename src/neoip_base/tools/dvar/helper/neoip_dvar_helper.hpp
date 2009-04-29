/*! \file
    \brief Header of the dvar_helper_t class
    
*/


#ifndef __NEOIP_DVAR_HELPER_HPP__ 
#define __NEOIP_DVAR_HELPER_HPP__ 
/* system include */
#include <sstream>
/* local include */
#include "neoip_namespace.hpp"
#include "neoip_serial_except.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	dvar_t;

/** \brief helper for the dvar_t object
 */
class dvar_helper_t {
private:
	static dvar_t		http_query_parsekeyval(std::string key, dvar_t val)		throw(serial_except_t);
	static void		http_query_mergekeyval(dvar_t &dvar1, dvar_t dvar2)		throw(serial_except_t);
	static dvar_t		http_query_convert_map2arr(const dvar_t &dvar)			throw();
public:
	static void		to_xml(const dvar_t &dvar, std::ostringstream &oss)		throw();
	static std::string	to_xml(const dvar_t &dvar)					throw();
	static void		to_http_query(const dvar_t &dvar, std::ostringstream &oss
							, const std::string &key_prefix
							, const std::string &key_suffix)	throw();
	static std::string	to_http_query(const dvar_t &dvar)				throw();
	static dvar_t		from_http_query(const std::string &query_str)			throw(serial_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DVAR_HELPER_HPP__  */



