/*! \file
    \brief main() for neoip_router

*/

/* system include */
/* local include */
#include "neoip_router_info.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_apps_oswarp.hpp"
#include "neoip_lib_apps_ezplugin.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_session_exit_ptr.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

#include "neoip_router_apps.hpp"
#include "neoip_router_err.hpp"

using namespace neoip;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief the Main function
 */
int main_internal(int argc, char **argv)
{
	// insert all the available plugins - MUST be done before anything else
	lib_apps_ezplugin_t::insert_all_available();
	// standard init
	//nipmem_malloc(10000);
	lib_apps_t *	lib_apps	= nipmem_new lib_apps_t();
	libsess_err_t	libsess_err	= lib_apps->start(argc, argv, router_apps_t::clineopt_arr()
						, NEOIP_APPS_CANON_NAME, NEOIP_APPS_HUMAN_NAME
						, NEOIP_APPS_VERSION, NEOIP_APPS_SUMMARY
						, NEOIP_APPS_LONGDESC, NEOIP_APPS_TYPE);
	if( libsess_err.failed() ){
		KLOG_ERR("cant start lib_apps_t due to " << libsess_err);
		return -1;
	}
	if( libsess_err.reason() == "DONTLAUNCHAPPS" )	return 0;

	// launch the router_apps_t
	lib_session_exit_ptr_t<router_apps_t> router_apps(nipmem_new router_apps_t(), lib_session_exit_t::EXIT_ORDER_APPS);
	router_err_t	router_err	= router_apps->start();
	if( router_err.failed() ){
		KLOG_ERR("Initialization error: " << router_err);
		return -1;
	}
	// if the router_err reason is "DONTLAUNCHAPPS", return 0 now
	// - NOTE: this is used for not launching the apps when the user register for example
	if( router_err.reason() == "DONTLAUNCHAPPS" ){
		// reassign router_err to force router_err_t to free() "DONTLAUNCHAPPS" reason
		// - else nipmem memory leak checker complains about it
		router_err	= router_err_t();
		router_apps.delete_ptr();
		nipmem_delete	lib_apps;
		return 0;
	}

	// log the event
	KLOG_ERR("NeoIP Router Application Initialized");
	// Start the event loop - exit only when the apps goes off
	lib_session_get()->loop_run();

	// delete router_apps_t
	router_apps.delete_ptr();

	// standard deinit
	nipmem_delete	lib_apps;
	
	// return no error
	return 0;
}

/** \brief special main wrapper for daemon application
 * 
 * - This is only done to allow to become a win32 service when compiled on win32
 */
int main(int argc, char *argv[])
{
	return lib_apps_oswarp_t::main_for_daemon(NEOIP_APPS_CANON_NAME, main_internal, argc, argv);
}

