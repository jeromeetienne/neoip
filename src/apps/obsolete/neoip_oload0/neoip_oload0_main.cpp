/*! \file
    \brief main() for neoip_oload0

\par Brief Description
neoip-oload0 is an application which act as a http gateway. it is accessible
via a http url and is able to retrieve file from the internet based on 3 formats:
- static: which is a static http content
- torrent: which is a .torrent file
- metalink: which is a .metalink file

\par TODO
- currently bt_oload0 configuration is fully hardcoded
  - export it and make it configurable
- make a oload0_runner_t class to put all the apps logic in it
  - i call all the main classes as _runner_t.. but oload0_apps_t seems more
    descriptive

*/

/* system include */
/* local include */
#include "neoip_lib_apps.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_bt_oload0.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

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
	// standard init
	lib_apps_t *	lib_apps	= nipmem_new lib_apps_t();
	libsess_err_t	libsess_err	= lib_apps->start(argc, argv, "neoip_oload"
						, "user", bt_oload0_t::clineopt_arr()
						, "0.01beta", "tool to offload http download", "" );
	if( libsess_err.failed() ){
		KLOG_ERR("cant start lib_apps_t due to " << libsess_err);
		return -1;
	}
	if( libsess_err.reason() == "DONTLAUNCHAPPS" )	return 0;
	
	// launch the bt_oload0_t
	bt_oload0_t *	bt_oload0	= nipmem_new bt_oload0_t();
	bt_err_t	bt_err		= bt_oload0->start();
	if( bt_err.failed() ){
		KLOG_ERR("Initialization error: " << bt_err);
		return -1;
	}

	// log the event
	KLOG_ERR("NeoIP HTTP OffLoad Application Initialized");
	// Start the event loop - exit only when the apps goes off
	lib_session_get()->loop_run();

	// delete bt_oload0_t
	nipmem_delete	bt_oload0;
	
	// standard deinit
	nipmem_delete	lib_apps;
	
	// return no error
	return 0;
}
