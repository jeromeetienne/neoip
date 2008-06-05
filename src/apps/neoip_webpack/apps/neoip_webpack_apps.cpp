/*! \file
    \brief Definition of the \ref webpack_apps_t

\par Brief Description
webpack_apps_t is bundle all the neoip application related to the web i.e 
neoip-casti, neoip-casto and neoip-oload.

*/

/* system include */
/* local include */
#include "neoip_webpack_apps.hpp"
#include "neoip_clineopt_arr.hpp"

#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
webpack_apps_t::webpack_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	casti_apps	= NULL;
	casto_apps	= NULL;
	oload_apps	= NULL;
}

/** \brief Destructor
 */
webpack_apps_t::~webpack_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// delete casti_apps_t if needed
	nipmem_zdelete	casti_apps;
	// delete casto_apps_t if needed
	nipmem_zdelete	casto_apps;
	// delete oload_apps_t if needed
	nipmem_zdelete	oload_apps;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	webpack_apps_t::start()						throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_WARN("enter");

#if 1
	// init casti_apps
	casti_apps	= nipmem_new lib_session_exit_ptr_t<casti_apps_t>(nipmem_new casti_apps_t()
						, lib_session_exit_t::EXIT_ORDER_APPS);
	bt_err		= (*casti_apps)->start();
	if( bt_err.failed() )	return bt_err;
#endif

#if 1
	// init casto_apps
	casto_apps	= nipmem_new lib_session_exit_ptr_t<casto_apps_t>(nipmem_new casto_apps_t()
						, lib_session_exit_t::EXIT_ORDER_APPS);
	bt_err		= (*casto_apps)->start();
	if( bt_err.failed() )	return bt_err;
#endif
	// init oload_apps
	oload_apps	= nipmem_new lib_session_exit_ptr_t<oload_apps_t>(nipmem_new oload_apps_t()
						, lib_session_exit_t::EXIT_ORDER_APPS);
	bt_err		= (*oload_apps)->start();
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	webpack_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;

	// NOTE: here must be only common option
	// - be carefull of conflict 
	
	// add the --nodaemon cmdline option
	clineopt	= clineopt_t("nodaemon", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("To keep the daemon in front");
	clineopt.alias_name_db().append("d");
	clineopt_arr	+= clineopt;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}


NEOIP_NAMESPACE_END;




