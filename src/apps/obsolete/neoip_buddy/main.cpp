/*! \file
    \brief main() for neoip_buddy
    
*/

/* system include */
/* local include */
#include "neoip_libapps.hpp"
#include "neoip_bud_main.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_property.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"
#include "neoip_p2daddr_layer_init.hpp"

using namespace neoip;

int main(int argc, char **argv)
{
	// standard init
	libapps_init(argc, argv);

	// init the p2daddr layer
	// - TODO unclear that it should be done here, in the main() and not in the lib init
	p2daddr_layer_init();
#if 1
	bud_main_t *	bud_main;
	// init the daemon
	bud_main	= nipmem_new bud_main_t();
	bool	failed	= bud_main->start();
	if( failed ){
		KLOG_ERR("Failed to start the NeoIP Buddy");
		return -1;
	}
#endif
#if 0
	url_redir_t	url_redir	= nipmem_new url_redir_t();
	{bool	failed	= url_redir->start();
	DBG_ASSERT( !failed );	// TODO poor error management
	}
#endif
	// log the event	
	KLOG_ERR("NeoIP Buddy Initialized");
	// Start the event loop
	lib_session_get()->loop_run();
	
#if 1
	// deinit bud_main_t
	nipmem_delete	bud_main;
#endif

	// deinit the p2daddr layer
	p2daddr_layer_deinit();
	// standard deinit
	libapps_deinit();
	
	// return no error
	return 0;
}


