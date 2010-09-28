/*! \file
    \brief Header of the \ref router_apps_t class

*/


#ifndef __NEOIP_ROUTER_APPS_HPP__ 
#define __NEOIP_ROUTER_APPS_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_lib_session_exit_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	router_err_t;
class	router_peer_t;
class	router_lident_t;
class	router_rident_t;
class	router_rident_arr_t;
class	router_rootca_t;
class	router_rootca_arr_t;
class	router_acache_t;
class	clineopt_arr_t;
class	file_path_t;
class	file_err_t;
class	x509_cert_t;
class	x509_privkey_t;
class	router_name_t;

/** \brief The object to run the neoip_get apps
 */
class router_apps_t : NEOIP_COPY_CTOR_DENY {
private:
	router_peer_t *	router_peer;

	/*************** Some cmdline actions	*******************************/
	router_err_t	cmdline_action_register()	throw();
	router_err_t	cmdline_action_rootca_create()	throw();
	router_err_t	cmdline_action_info()		throw();
public:
	/*************** ctor/dtor	***************************************/
	router_apps_t()		throw();
	~router_apps_t()	throw();
	
	/*************** setup function	***************************************/
	router_err_t		start()		throw();

	/*************** query function	***************************************/
	static clineopt_arr_t	clineopt_arr()	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_APPS_HPP__  */


 
