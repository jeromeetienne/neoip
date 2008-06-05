/*! \file
    \brief main() for neoip_btcli

*/

/* system include */
/* local include */
#include "neoip_btcli_info.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_oswarp.hpp"
#include "neoip_lib_apps_ezplugin.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_session_exit_ptr.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// list of include specific to this apps
#include "neoip_btcli_apps.hpp"
#include "neoip_bt_err.hpp"

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
	libsess_err_t	libsess_err	= lib_apps->start(argc, argv, btcli_apps_t::clineopt_arr()
						, NEOIP_APPS_CANON_NAME, NEOIP_APPS_HUMAN_NAME
						, NEOIP_APPS_VERSION, NEOIP_APPS_SUMMARY
						, NEOIP_APPS_LONGDESC, NEOIP_APPS_TYPE);
	if( libsess_err.failed() ){
		KLOG_ERR("cant start lib_apps_t due to " << libsess_err);
		return -1;
	}
	if( libsess_err.reason() == "DONTLAUNCHAPPS" )	return 0;
	
	// launch the btcli_apps_t
	lib_session_exit_ptr_t<btcli_apps_t> btcli_apps(nipmem_new btcli_apps_t(), lib_session_exit_t::EXIT_ORDER_APPS);
	bt_err_t	bt_err		= btcli_apps->start();
	if( bt_err.failed() ){
		KLOG_ERR("Initialization error: " << bt_err);
		return -1;
	}

	// log the event
	KLOG_WARN("NeoIP BTcli Application Initialized");
	// Start the event loop - exit only when the apps goes off
	lib_session_get()->loop_run();

	// delete btcli_apps_t
	btcli_apps.delete_ptr();
	
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

