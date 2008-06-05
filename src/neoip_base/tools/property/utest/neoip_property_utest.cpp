/*! \file
    \brief Definition of the unit test for the \ref property_t and co

*/

/* system include */
/* local include */
#include "neoip_property_utest.hpp"
#include "neoip_property.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

using namespace neoip;

/** \brief unit test for the property_t class
 */
int neoip_property_utest()
{
	int		n_error = 0;
	property_t	property;
	
	if( property.insert_string("first_name", "john") )	goto error;
	if( property.insert_string("last_name" , "smith") )	goto error;
	
	if( property.exist("first_name")== false )		goto error;
	if( property.exist("last_name")	== false )		goto error;
	if( property.exist("blibli")	!= false )		goto error;

	if( property.find_string("first_name")	!= "john" )		goto error;
	if( property.find_string("last_name")	!= "smith" )		goto error;
	if( property.find_string("blabla")	!= "" )			goto error;

#if 0
	// test a load_file
	if( property.load_file("conf_netcat1/neoip_session.conf") )	goto error;
	if( property.find_string("lib_httpd_listen")!= "127.0.0.1:9080")goto error;
#endif

//	property.dump_dbg();
	
	
	if( n_error )	goto error;
	KLOG_ERR("property_t unit test PASSED!!!");

	return n_error;
error:;	KLOG_ERR("property_t unit test FAILED!!!");
	return 1;
}

