/*! \file
    \brief main() for neoip-upnp

\par Brief Description
This application is used to export all the functionality of libneoip_upnp into
a commandline tool

*/

/* system include */
/* local include */
#include "neoip_upnp_info.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_ezplugin.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_session_exit_ptr.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

#include "neoip_upnp_apps.hpp"
#include "neoip_upnp_err.hpp"

using namespace neoip;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   main function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief the Main function
 */
int main(int argc, char **argv)
{
	// insert all the available plugins - MUST be done before anything else
	lib_apps_ezplugin_t::insert_all_available();
	// standard init
	lib_apps_t *	lib_apps	= nipmem_new lib_apps_t();
	libsess_err_t	libsess_err	= lib_apps->start(argc, argv, upnp_apps_t::clineopt_arr()
						, NEOIP_APPS_CANON_NAME, NEOIP_APPS_HUMAN_NAME
						, NEOIP_APPS_VERSION, NEOIP_APPS_SUMMARY
						, NEOIP_APPS_LONGDESC, NEOIP_APPS_TYPE);
	if( libsess_err.failed() ){
		KLOG_ERR("cant start lib_apps_t due to " << libsess_err);
		return -1;
	}
	if( libsess_err.reason() == "DONTLAUNCHAPPS" )	return 0;

	// launch the upnp_apps_t
	lib_session_exit_ptr_t<upnp_apps_t> upnp_apps(nipmem_new upnp_apps_t(), lib_session_exit_t::EXIT_ORDER_APPS);
	upnp_err_t	upnp_err	= upnp_apps->start();
	if( upnp_err.failed() ){
		KLOG_ERR("Initialization error: " << upnp_err);
		return -1;
	}

	// log the event
	KLOG_WARN("NeoIP UPNP Application Initialized");
	// Start the event loop - exit only when the apps goes off
	lib_session_get()->loop_run();

	// delete upnp_apps_t 
	upnp_apps.delete_ptr();
	
	// standard deinit
	nipmem_delete	lib_apps;
	
	// return no error
	return 0;
}
