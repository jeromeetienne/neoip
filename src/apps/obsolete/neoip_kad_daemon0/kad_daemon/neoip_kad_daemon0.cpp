/*! \file
    \brief Definition of the \ref kad_daemon_t class

- for neoip_kad_deamon
  - rename it as neoip_kad_xmlrpcd
  - do a man page
  - do a rpm
  - this stuff does:
    - only a cmdline parsing
    - init of a kad_listener_t
    - init of a http handler for the xmlrpc
    - init of the kad_xmlrpc_t to handler the request from http

*/

/* system include */
#include <iostream>
#include <iomanip>
/* local include */
#include "neoip_kad_daemon.hpp"
#include "neoip_kad_xmlrpc.hpp"
#include "neoip_kad.hpp"
#include "neoip_log.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_property.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_daemon_t::kad_daemon_t()	throw()
{
	kad_xmlrpc	= NULL;
	kad_listener	= NULL;
}

/** \brief Destructor
 */
kad_daemon_t::~kad_daemon_t()	throw()
{
	if( kad_xmlrpc )	nipmem_delete	kad_xmlrpc;
	if( kad_listener )	nipmem_delete	kad_listener;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    START function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
kad_err_t	kad_daemon_t::start()		throw()
{
	const property_t &session_prop	= *(lib_session_get()->get_property());
	kad_err_t	kad_err;

	// check that the listen address is not null
	if( session_prop.find_string("kad_listen_addr").empty() )
		return kad_err_t(kad_err_t::ERROR,"The session property kad_listen_addr is NOT set");

	// get parameter from the sessions property
	ipport_addr_t	listen_addr	= session_prop.find_string("kad_listen_addr");
	ipport_addr_t	langrp_addr	= session_prop.find_string("kad_langrp_addr", "255.255.255.255:7777");
	
	// create the kad_listener_t
	kad_listener	= nipmem_new kad_listener_t();
	// setup the kad_listener_t's listening address
	kad_err		= kad_listener->set_listen_addr(listen_addr);
	if( kad_err.failed() )	goto error;
	// setup the kad_listener_t's langrp addr
	kad_err		= kad_listener->set_langrp_addr(langrp_addr);
	if( kad_err.failed() )	goto error;
	// start it now
	kad_err		= kad_listener->start();
	if( kad_err.failed() )	goto error;

	// create the http handler for the daemon
	{std::string	url_path	= lib_session_get()->get_httpd()->get_rootpath() + "/RPC2";
	kad_xmlrpc	= nipmem_new kad_xmlrpc_t(kad_listener, url_path);
	}
	// return no error
	return kad_err_t::OK;

error:	// in case of error, delete the kad_listener, mark it unused and return the error
	nipmem_delete	kad_listener;
	kad_listener	= NULL;
	return kad_err;
}

NEOIP_NAMESPACE_END

