/*! \file
    \brief Definition of the unit test for the \ref mimediag_t and co

*/

/* system include */
/* local include */
#include "neoip_mimediag_utest.hpp"
#include "neoip_mimediag.hpp"
#include "neoip_file_path.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

using namespace neoip;

/** \brief unit test for the mimediag_t class
 */
int neoip_mimediag_utest()
{
	int		n_error = 0;
	// create the mimediag
	mimediag_t	mimediag;
	file_path_t	file_path;

	file_path	= "/tmp/nbla.pdf";
	KLOG_ERR("for " << file_path << " the mimetype=" << mimediag.from_file(file_path) );
	file_path	= "/tmp/";
	KLOG_ERR("for " << file_path << " the mimetype=" << mimediag.from_file(file_path) );
	file_path	= "/tmp/IBM_Linux_Commercial.avi";
	KLOG_ERR("for " << file_path << " the mimetype=" << mimediag.from_file(file_path) );
	file_path	= "/tmp/nbla.mpeg";
	KLOG_ERR("for " << file_path << " the mimetype=" << mimediag.from_file(file_path) );
	
	
	if( n_error )	goto error;
	KLOG_ERR("mimediag_t unit test PASSED!!!");

	return n_error;
error:;	KLOG_ERR("mimediag_t unit test FAILED!!!");
	return 1;
}

