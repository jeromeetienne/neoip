/*! \file
    \brief definition of the \ref log_layer_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_stdlog.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			log function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief log a message 
 */
void 	stdlog_t::do_cout(const std::string &str)			throw()
{
#ifndef	_WIN32
	std::cout << str;
#else
	// TODO: to remove - just a workaround on mingw gcc 3.4.5 being bugged on std::cout/cerr
	printf("%s", str.c_str());
#endif
}

/** \brief log a message 
 */
void 	stdlog_t::do_cerr(const std::string &str)			throw()
{
#ifndef	_WIN32
	std::cerr << str;
#else
	// TODO: to remove - just a workaround on mingw gcc 3.4.5 being bugged on std::cout/cerr
	printf("%s", str.c_str());
#endif
}

NEOIP_NAMESPACE_END







