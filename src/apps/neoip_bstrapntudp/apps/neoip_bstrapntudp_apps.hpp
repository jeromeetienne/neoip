/*! \file
    \brief Header of the \ref bstrapntudp_apps_t class

*/


#ifndef __NEOIP_BSTRAPNTUDP_APPS_HPP__ 
#define __NEOIP_BSTRAPNTUDP_APPS_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_lib_session_exit_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_peer_t;
class	ntudp_err_t;
class	router_peer_t;
class	clineopt_arr_t;

/** \brief The object to run the neoip_get apps
 */
class bstrapntudp_apps_t : NEOIP_COPY_CTOR_DENY {
private:
	std::list<ntudp_peer_t *>	peer_db;
public:
	/*************** ctor/dtor	***************************************/
	bstrapntudp_apps_t()		throw();
	~bstrapntudp_apps_t()		throw();
	
	/*************** setup function	***************************************/
	ntudp_err_t		start()		throw();

	/*************** query function	***************************************/
	static clineopt_arr_t	clineopt_arr()	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BSTRAPNTUDP_APPS_HPP__  */


 
