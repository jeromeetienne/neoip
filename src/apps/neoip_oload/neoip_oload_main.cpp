/*! \file
    \brief main() for neoip-oload

\par Brief Description
This application is used to export in http the content static http, bittorrent or metalink.

\par about wikipedia-common contact
- from siebrand, in #wikimedia-tech you might want to talk to core developers 
  brion or TimStarling and network administrator mark.
  - about what kind of network problem, wikipedia is having
- on #ubuntu-mirror, znarl seems nice and to know the people needed
  - he is part of canonical

\par BUG apparently this url doesnt download thru neoip-oload
- http://cache.googlevideo.com/get_video?video_id=-7JVxE2SYxo&origin=sjl-casing1.sjl.youtube.com
- unknown reason. to debug
- ERROR it does download the issue what the neoip_metavar_ which wasnt set for youtube
  - wget -O /dev/null "http://localhost:4550/http://cache.googlevideo.com/get_video?video_id=-7JVxE2SYxo&origin=sjl-casing1.sjl.youtube.com&neoip_metavar_http_mod_type=flv&neoip_metavar_http_mod_flv_varname=start"
- nevertheless this url got no initial metadata in the flv... which make it rejected
  by the playlist builder...
  - TODO fix this case
- additionnaly when downloading this url i got weird eth0 traffic. it is like
  nothing for a while, then 500kbyte
- the initial connection to get the length is very long too... why is that ?
  - a lot of redirect ? or slow dns ?
  - CURL -IL is fast
  - TODO sort this out (using curl)

*/

/* system include */
/* local include */
#include "neoip_oload_info.hpp"	// to get all the info about this apps
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_oswarp.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_session_exit_ptr.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

#include "neoip_oload_apps.hpp"
#include "neoip_bt_apps_ezplugin.hpp"
#include "neoip_bt_err.hpp"

using namespace neoip;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   main function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief the Main function
 */
int main_internal(int argc, char *argv[])
{
	// insert all the needed plugins for bt apps - MUST be done before anything else
	bt_apps_ezplugin_t::insert_all_needed();
	// standard init
	lib_apps_t *	lib_apps	= nipmem_new lib_apps_t();
	libsess_err_t	libsess_err	= lib_apps->start(argc, argv, oload_apps_t::clineopt_arr()
						, NEOIP_APPS_CANON_NAME, NEOIP_APPS_HUMAN_NAME
						, NEOIP_APPS_VERSION, NEOIP_APPS_SUMMARY
						, NEOIP_APPS_LONGDESC, NEOIP_APPS_TYPE);
	if( libsess_err.failed() ){
		KLOG_ERR("cant start lib_apps_t due to " << libsess_err);
		return -1;
	}
	if( libsess_err.reason() == "DONTLAUNCHAPPS" )	return 0;	
	
	// launch the oload_apps_t
	lib_session_exit_ptr_t<oload_apps_t> oload_apps(nipmem_new oload_apps_t(), lib_session_exit_t::EXIT_ORDER_APPS);
	bt_err_t	bt_err		= oload_apps->start();
	if( bt_err.failed() ){
		KLOG_ERR("Initialization error: " << bt_err);
		return -1;
	}

	// log the event
	KLOG_WARN("NeoIP Oload Application Initialized");
	// Start the event loop - exit only when the apps goes off
	lib_session_get()->loop_run();

	// delete oload_apps_t
	oload_apps.delete_ptr();
	
	// standard deinit
	nipmem_zdelete	lib_apps;
	
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

