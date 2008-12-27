/*! \file
    \brief main() for neoip_wget

\par Brief Description
This application is used to retrieves files. It support retrieving file from
the http, bittorrent or metalink.
- The bittorrent and metalink metafiles may retrieved from the local file system
  or from the web.
- it is also possible to continue a previously started and interrupted download
  from a .neoip_resumedata files.

*/

/* system include */
/* local include */
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_ezplugin.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_session_exit_ptr.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

#include "neoip_rtmp2xml_info.hpp"
#include "neoip_rtmp2xml_apps.hpp"
#include "neoip_rtmp_err.hpp"

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
	libsess_err_t	libsess_err	= lib_apps->start(argc, argv, rtmp2xml_apps_t::clineopt_arr()
						, NEOIP_APPS_CANON_NAME, NEOIP_APPS_HUMAN_NAME
						, NEOIP_APPS_VERSION, NEOIP_APPS_SUMMARY
						, NEOIP_APPS_LONGDESC, NEOIP_APPS_TYPE);
	if( libsess_err.failed() ){
		KLOG_ERR("cant start lib_apps_t due to " << libsess_err);
		return -1;
	}
	if( libsess_err.reason() == "DONTLAUNCHAPPS" )	return 0;

	// launch the rtmp2xml_apps_t
	lib_session_exit_ptr_t<rtmp2xml_apps_t>	rtmp2xml_apps(nipmem_new rtmp2xml_apps_t(), lib_session_exit_t::EXIT_ORDER_APPS);
	rtmp_err_t	rtmp_err		= rtmp2xml_apps->start();
	if( rtmp_err.failed() ){
		KLOG_ERR("Initialization error: " << rtmp_err);
		return -1;
	}

	// log the event
	KLOG_WARN("NeoIP Get Application Initialized");

	// Start the event loop - exit only when the apps goes off
	lib_session_get()->loop_run();

	// delete rtmp2xml_apps_t
	rtmp2xml_apps.delete_ptr();

	// standard deinit
	nipmem_delete	lib_apps;

	// return no error
	return 0;
}
