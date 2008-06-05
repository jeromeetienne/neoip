/*! \file
    \brief Definition of the unit test for the \ref log_t and co

\par Brief description
This module performs an unit test for the log_t classes

*/

/* system include */
#include <string>
/* local include */
#include "neoip_log_utest.hpp"
#include "neoip_log_layer_init.hpp"
#include "neoip_log_layer.hpp"
#include "neoip_log_level.hpp"
#include "neoip_log.hpp"
#include "neoip_log.hpp"

using namespace neoip;

/** \brief unit test for the log_t class
 */
int neoip_log_utest()
{
	int		n_error = 0;

	// init the layer
	log_layer_init();
	// copy the layer pointer
	log_layer_t	*log_layer	= log_layer_get();

	// load a category file
	bool	failed	= log_layer->load_category_file("/tmp/neoip_log.conf");
	DBG_ASSERT( !failed );

	KLOG_ERR("bonjour error");
	KLOG_INFO("bonjour info");
	KLOG_DEBUG("bonjour debug");

	// deinit the layer	
	log_layer_deinit();

	if( n_error )	KLOG_ERR("log_t unit test FAILED!!!");
	else		KLOG_ERR("log_t unit test PASSED!!!");
	return n_error;
}

