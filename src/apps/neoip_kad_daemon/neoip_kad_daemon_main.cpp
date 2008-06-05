/*! \file
    \brief main() for kad_daemon_apps_t

*/

/* system include */
/* local include */
#include "neoip_kad_daemon_info.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_oswarp.hpp"
#include "neoip_lib_apps_ezplugin.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_session_exit_ptr.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

#include "neoip_kad_daemon_apps.hpp"
#include "neoip_kad_err.hpp"

using namespace neoip;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   main function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief the Main function
 */
int main_internal(int argc, char **argv)
{
	// insert all the available plugins - MUST be done before anything else
	lib_apps_ezplugin_t::insert_all_available();
	// standard init
	lib_apps_t *	lib_apps	= nipmem_new lib_apps_t();
	libsess_err_t	libsess_err	= lib_apps->start(argc, argv, kad_daemon_apps_t::clineopt_arr()
						, NEOIP_APPS_CANON_NAME, NEOIP_APPS_HUMAN_NAME
						, NEOIP_APPS_VERSION, NEOIP_APPS_SUMMARY
						, NEOIP_APPS_LONGDESC, NEOIP_APPS_TYPE);
	if( libsess_err.failed() ){
		KLOG_ERR("cant start lib_apps_t due to " << libsess_err);
		return -1;
	}
	if( libsess_err.reason() == "DONTLAUNCHAPPS" )	return 0;
	
	// launch the kad_daemon_apps_t
	lib_session_exit_ptr_t<kad_daemon_apps_t> kad_daemon_apps(nipmem_new kad_daemon_apps_t(), lib_session_exit_t::EXIT_ORDER_APPS);
	kad_err_t		kad_err		= kad_daemon_apps->start();
	if( kad_err.failed() ){
		KLOG_ERR("Initialization error: " << kad_err);
		return -1;
	}

	// log the event
	KLOG_WARN("NeoIP Get Application Initialized");
	// Start the event loop - exit only when the apps goes off
	lib_session_get()->loop_run();

	// delete kad_daemon_apps_t
	kad_daemon_apps.delete_ptr();
	
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


