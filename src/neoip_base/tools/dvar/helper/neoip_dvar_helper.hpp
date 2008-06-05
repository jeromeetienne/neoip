/*! \file
    \brief Header of the dvar_helper_t class
    
*/


#ifndef __NEOIP_DVAR_HELPER_HPP__ 
#define __NEOIP_DVAR_HELPER_HPP__ 
/* system include */
#include <sstream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	dvar_t;

/** \brief helper for the dvar_t object
 */
class dvar_helper_t {
private:
public:
	static void		to_xml(const dvar_t &dvar, std::ostringstream &oss)	throw();
	static std::string	to_xml(const dvar_t &dvar)				throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DVAR_HELPER_HPP__  */



