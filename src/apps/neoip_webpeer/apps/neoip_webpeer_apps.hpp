/*! \file
    \brief Header of the \ref webpeer_apps_t
    
*/


#ifndef __NEOIP_WEBPEER_APPS_HPP__ 
#define __NEOIP_WEBPEER_APPS_HPP__ 

/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_lib_session_exit_ptr.hpp"
#include "neoip_casti_apps.hpp"
#include "neoip_casto_apps.hpp"
#include "neoip_oload_apps.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	clineopt_arr_t;

/** \brief Handle the http offload capability
 */
class webpeer_apps_t : NEOIP_COPY_CTOR_DENY {
private:
	lib_session_exit_ptr_t<casti_apps_t> *	casti_apps;
	lib_session_exit_ptr_t<casto_apps_t> *	casto_apps;
	lib_session_exit_ptr_t<oload_apps_t> *	oload_apps;
public:
	/*************** ctor/dtor	***************************************/
	webpeer_apps_t()		throw();
	~webpeer_apps_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t		start()			throw();
	
	/*************** Query function	***************************************/
	static clineopt_arr_t	clineopt_arr()		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_WEBPEER_APPS_HPP__  */










