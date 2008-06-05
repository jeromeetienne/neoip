/*! \file
    \brief Definition of the unit test for the \ref thumb_factory_t and co

*/

/* system include */
/* local include */
#include "neoip_thumb_factory_utest.hpp"
#include "neoip_thumb_factory.hpp"
#include "neoip_file_path.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

using namespace neoip;

/** \brief unit test for the thumb_factory_t class
 */
int neoip_thumb_factory_utest()
{
	int		n_error = 0;
	thumb_factory_t	thumb_factory;
	file_path_t	file_path;
	
	file_path	= "/tmp";
	KLOG_ERR("icon name for " << file_path << " is " << thumb_factory.get_thumb_name(file_path));
	file_path	= "/tmp/18_Credits.ogg";
	KLOG_ERR("icon name for " << file_path << " is " << thumb_factory.get_thumb_name(file_path));
	file_path	= "/media/usbdisk/movies2/breakin_1984.avi";
	KLOG_ERR("icon name for " << file_path << " is " << thumb_factory.get_thumb_name(file_path));


	if( n_error )	goto error;
	KLOG_ERR("thumb_factory_t unit test PASSED!!!");

	return n_error;
error:;	KLOG_ERR("thumb_factory_t unit test FAILED!!!");
	return 1;
}



