/*! \file
    \brief Header of the \ref kad_http_t class

*/


#ifndef __NEOIP_KAD_DAEMON_HPP__ 
#define __NEOIP_KAD_DAEMON_HPP__ 
/* system include */
/* local include */
#include "neoip_kad.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	kad_xmlrpc_t;

/** \brief To handle the service of kad_daemon
 */
class kad_daemon_t : NEOIP_COPY_CTOR_DENY {
private:
	kad_listener_t *		kad_listener;	//!< the kad_listener of the daemon
	kad_xmlrpc_t *		kad_xmlrpc;	//!< to handle the xmlrpc for the daemon
public:
	/*************** ctor/dtor	***************************************/
	kad_daemon_t()	throw();
	~kad_daemon_t()	throw();
	
	/*************** setup function	***************************************/
	kad_err_t	start()		throw();
	
	/*************** query function	***************************************/
	kad_listener_t &	get_kad_listener()	throw()	{ return *kad_listener;	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_DAEMON_HPP__  */



