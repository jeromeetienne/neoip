/*! \file
    \brief Header of the \ref kad_daemon_apps_t class

*/


#ifndef __NEOIP_KAD_DAEMON_APPS_HPP__ 
#define __NEOIP_KAD_DAEMON_APPS_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_lib_session_exit_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	kad_err_t;
class	kad_rpcresp_t;
class	router_lident_t;
class	router_rident_t;
class	router_rident_arr_t;
class	clineopt_arr_t;
class	file_path_t;
class	file_err_t;

/** \brief The object to run the neoip_get apps
 */
class kad_daemon_apps_t : NEOIP_COPY_CTOR_DENY {
private:
	kad_rpcresp_t *	kad_rpcresp;
public:
	/*************** ctor/dtor	***************************************/
	kad_daemon_apps_t()		throw();
	~kad_daemon_apps_t()		throw();
	
	/*************** setup function	***************************************/
	kad_err_t	start()		throw();

	/*************** query function	***************************************/
	static clineopt_arr_t	clineopt_arr()	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_DAEMON_APPS_HPP__  */


 
