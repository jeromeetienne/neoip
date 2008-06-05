/*! \file
    \brief Definition of the unit test for the \ref xmlrpc_call_t and \rec xmlrpc_build_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_xmlrpc_utest.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_xmlrpc_exttype.hpp"
#include "neoip_cookie.hpp"
#include "neoip_datum.hpp"
#include "neoip_log.hpp"

using namespace neoip;



/** \brief unit test for the xmlrpc class
 */
int neoip_xmlrpc_utest()
{
	int		n_error		= 0;
	xmlrpc_build_t	xmlrpc_build;
	xmlrpc_parse_t	xmlrpc_parse;

#if 0
	xmlrpc_build.put_fault(-1, "WOW this is bad");
#endif
	
#if 1
	cookie_id_t	cookie_id;
	datum_t		datum("bonjour", 7);
	xmlrpc_build.start_resp();
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEGIN;
	xmlrpc_build	<< 	xmlrpc_build_t::ARRAY_BEGIN;
//	xmlrpc_build	<< 		cookie_id;
//	xmlrpc_build	<< 		kad_id;
	xmlrpc_build	<< 		datum;
//	xmlrpc_build	<< 		double(1.6);
//	xmlrpc_build	<< 		true;
//	xmlrpc_build	<< 		false;
//	xmlrpc_build	<< 		std::string("Here is the answer :");
	xmlrpc_build	<< 	xmlrpc_build_t::ARRAY_END;
	xmlrpc_build	<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEGIN;
	xmlrpc_build	<< int32_t(42);
	xmlrpc_build	<< xmlrpc_build_t::PARAM_END;
#endif
#if 0
	xmlrpc_build.start_call("My Wonderfull Method");
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEGIN;
	xmlrpc_build 		<< int32_t(9999);
	xmlrpc_build 		<< int32_t(32);
	xmlrpc_build	<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEGIN;
	xmlrpc_build 		<< int32_t(42);
	xmlrpc_build 		<< int32_t(44);
	xmlrpc_build 		<< int32_t(45);
	xmlrpc_build 		<< int32_t(46);
	xmlrpc_build 		<< int32_t(47);
	xmlrpc_build	<< xmlrpc_build_t::PARAM_END;
#endif
	KLOG_ERR("Output = " << xmlrpc_build.get_xml_output() );

#if 0	

	if( xmlrpc_parse.set_document(xmlrpc_build.get_xml_output()) ){
		KLOG_ERR("Parsing a malformed XMLRPC message");
	}else{
		int32_t	a, b, c;
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN;
		xmlrpc_parse 		>> a;
		xmlrpc_parse 		>> b;
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_END;	
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN;
		xmlrpc_parse 		>> c;
		xmlrpc_parse	>> xmlrpc_parse_t::PARAM_END;	
		KLOG_ERR("a= " << a);
		KLOG_ERR("b= " << b);
		KLOG_ERR("c= " << c);
	}
#endif
	if( n_error )	goto error;
	KLOG_ERR("xmlrpc utest PASSED");
	return 0;
error:;
	KLOG_ERR("xmlrpc utest FAILED!!!!");
	return -1;
}


