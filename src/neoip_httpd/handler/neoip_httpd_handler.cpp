/*! \file
    \brief Definition of the \ref httpd_handler_t class
    
*/

/* system include */
/* local include */
#include "neoip_httpd_handler.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor by default
 */
httpd_handler_t::httpd_handler_t()						throw()
{
	callback = NULL;
}

/** \brief Constructor with value
 */
httpd_handler_t::httpd_handler_t(std::string path, httpd_handler_cb_t *callback, void *userptr
					, httpd_handler_flag_t handler_flag)	throw()
{
	// copy the parameter
	this->path		= path;
	this->handler_flag	= handler_flag;
	this->callback		= callback;
	this->userptr		= userptr;
}
NEOIP_NAMESPACE_END






