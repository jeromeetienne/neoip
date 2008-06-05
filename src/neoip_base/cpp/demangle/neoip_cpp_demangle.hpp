/*! \file
    \brief Some functions to demangle cpp symbols

*/

#ifndef __NEOIP_CPP_DEMANGLE_HPP__ 
#define __NEOIP_CPP_DEMANGLE_HPP__ 

/* system include */
#include <string>
#include <typeinfo>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

std::string	neoip_cpp_demangle_type(const std::string &symbol)	throw();
std::string	neoip_cpp_demangle_function(const std::string &symbol)	throw();

/** \brief return the demangled type name of parameter
 */
template <typename T> std::string neoip_cpp_typename(const T &value)	throw()
{
	return neoip_cpp_demangle_type(typeid(value).name());
}


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CPP_DEMANGLE_HPP__  */


