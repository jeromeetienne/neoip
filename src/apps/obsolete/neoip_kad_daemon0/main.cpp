/*! \file
    \brief main() for neoip_kad_daemon
    
*/

/* system include */
/* local include */
#include "neoip_libapps.hpp"
#include "neoip_kad_daemon.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_property.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// TODO to remove it is for debug only
#include "neoip_p2daddr_layer_init.hpp"
#include "neoip_kad_utest.hpp"
#include "neoip_xmlrpc_utest.hpp"
#include "neoip_dns_grabber_utest.hpp"
#include "neoip_file_utest.hpp"
#include "neoip_string_utest.hpp"
#include "neoip_mimediag_utest.hpp"
#include "neoip_thumb_factory_utest.hpp"
#include "neoip_compress_utest.hpp"
#include "neoip_p2daddr_utest.hpp"
#include "neoip_asyncop_utest.hpp"
#include "neoip_agethostbyname_utest.hpp"

using namespace neoip;

int main(int argc, char **argv)
{
	// standard init
	libapps_init(argc, argv);

	// init the p2daddr layer
	// - TODO unclear that it should be done here, in the main() and not in the lib init
	p2daddr_layer_init();

	// some unit test to debug
//	neoip_kad_utest();
//	neoip_kad_utest_start();
//	neoip_xmlrpc_utest();
//	neoip_dns_grabber_utest_start();
//	neoip_file_utest();
//	neoip_string_utest();
//	neoip_mimediag_utest();
//	neoip_thumb_factory_utest();
//	neoip_compress_utest();
//	neoip_p2daddr_utest_start();
//	neoip_asyncop_utest_start();
//	neoip_agethostbyname_utest_start();


	// init the daemon
#if 1
	kad_daemon_t *	kad_daemon	= nipmem_new kad_daemon_t();
	kad_err_t	kad_err		= kad_daemon->start();
	if( kad_err.failed() ){
		KLOG_ERR("Failed to start the kad daemon due to " << kad_err);
		return -1;
	}
#endif
	KLOG_ERR("NeoIP DHT daemon Initialized");
	// Start the event loop
	lib_session_get()->loop_run();

#if 1
	// delete the daemon
	nipmem_delete	kad_daemon;
#endif
	// close the utest
//	neoip_kad_utest_end();
//	neoip_p2daddr_utest_end();
//	neoip_dns_grabber_utest_end();
//	neoip_asyncop_utest_end();
//	neoip_agethostbyname_utest_end();

	// deinit the p2daddr layer
	p2daddr_layer_deinit();	

	// standard deinit
	libapps_deinit();
	
	// return no error
	return 0;
}


