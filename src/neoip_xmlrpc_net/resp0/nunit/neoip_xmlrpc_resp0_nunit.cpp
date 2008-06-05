/*! \file
    \brief Definition of the unit test for the \ref xmlrpc_resp0_t

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_resp0_nunit.hpp"
#include "neoip_xmlrpc_resp0.hpp"
#include "neoip_xmlrpc_resp0_sfct.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_http_sclient.hpp"
#include "neoip_http_sclient_res.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Declaration of the xmlrpc_resp0_sfct_db_t containing the functions for nunit
 * 
 * - it is typically put in a .hpp
 */
NEOIP_XMLRPC_RESP0_SFCT_DECLARATION(xmlrpc_resp0_nunit_sfct_db);

/** \brief Definition of the xmlrpc_resp0_sfct_db_t containing the functions for nunit
 * 
 * - it is typically put in a .cpp
 */
NEOIP_XMLRPC_RESP0_SFCT_DEFINITION(xmlrpc_resp0_nunit_sfct_db);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			example of function exported in xmlrpc
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief External function doing a simple 'add' of two int32_t and returning the result
 */
int32_t	add_fct(void *userptr, int32_t value1, int32_t value2)	throw()
{
	return value1 + value2;	
}

// put the add_fct in the xmlrpc_resp0_nunit_sfct_db
NEOIP_XMLRPC_RESP0_SFCT_ITEM(add, 2, xmlrpc_resp0_nunit_sfct_db
				, int32_t, add_fct, (int32_t, int32_t));

/** \brief External function doing a simple 'sub' of two int32_t and returning the result
 */
int32_t	sub_fct(void *userptr, int32_t value1, int32_t value2)	throw()
{
	return value1 - value2;	
}

// put the sub_fct in the xmlrpc_resp0_nunit_sfct_db
NEOIP_XMLRPC_RESP0_SFCT_ITEM(sub, 2, xmlrpc_resp0_nunit_sfct_db
				, int32_t, sub_fct, (int32_t, int32_t));

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
xmlrpc_resp0_testclass_t::xmlrpc_resp0_testclass_t()	throw()
{
	// zero some variable
	xmlrpc_resp0	= NULL;
	http_sclient	= NULL;
}

/** \brief Destructor
 */
xmlrpc_resp0_testclass_t::~xmlrpc_resp0_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	xmlrpc_resp0_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	xmlrpc_resp0_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the xmlrpc_resp0_t if needed
	nipmem_zdelete	xmlrpc_resp0;
	// destroy the http_sclient_t if needed
	nipmem_zdelete	http_sclient;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	xmlrpc_resp0_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	xmlrpc_err_t	xmlrpc_err;
	// start the xmlrpc_resp0_t
	xmlrpc_resp0	= nipmem_new xmlrpc_resp0_t();
	xmlrpc_err	= xmlrpc_resp0->start("/neoip/RPC2");
	NUNIT_ASSERT( xmlrpc_err.succeed() );
	// just add the list of exported function to the xmlrpc_resp0_t
	xmlrpc_resp0->sfct_db_arr()	+= xmlrpc_resp0_nunit_sfct_db;
	
	
	// build the xmlrpc call for sub(5,3)
	xmlrpc_build_t	xmlrpc_build;	
	xmlrpc_build << xmlrpc_build_t::CALL_BEG("sub");
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG << int32_t(5) << xmlrpc_build_t::PARAM_END;
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEG << int32_t(3) << xmlrpc_build_t::PARAM_END;
	xmlrpc_build << xmlrpc_build_t::CALL_END;
	datum_t		data2post	= xmlrpc_build.to_datum();

	// build the http_reqhd_t to use for the http_sclient_t
	http_reqhd_t	http_reqhd;
	http_reqhd.method("POST").uri("http://127.0.0.1:9080/neoip/RPC2");
	http_reqhd.header_db().append("Content-Length", OSTREAMSTR(data2post.length()));
	// start the http_sclient_t
	http_err_t	http_err;
	http_sclient	= nipmem_new http_sclient_t();
	http_err	= http_sclient->start(http_reqhd, this, NULL, data2post);
	DBG_ASSERT( http_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     http_sclient_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t to provide event
 */
bool	xmlrpc_resp0_testclass_t::neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &cb_sclient_res)	throw() 
{
	http_sclient_res_t	sclient_res	= cb_sclient_res;
	// log to debug
	KLOG_DBG("enter http_sclient_res=" << sclient_res);
	KLOG_DBG("reply_body=" << sclient_res.reply_body().to_stdstring());

	/*
	 * Parse the xmlrpc resp0onse
	 */	

	// set the document for xmlrpc_parse_t
	xmlrpc_parse_t	xmlrpc_parse;
	xmlrpc_parse.set_document(sclient_res.reply_body().to_datum());
	DBG_ASSERT( !xmlrpc_parse.is_null() );
	// declare the variable to read from the xmlrpc
	int32_t	result;
	try {
		xmlrpc_parse >> xmlrpc_parse_t::RESP_BEG;
		xmlrpc_parse	>> result;
		xmlrpc_parse >> xmlrpc_parse_t::RESP_END;
	} catch(xml_except_t &e) {
		KLOG_ERR("xml_except_t=" << e.what());
		return nunit_ftor(NUNIT_RES_ERROR);
	}
	// check the parameters have the expected values
	if( result == 2 )	nunit_ftor(NUNIT_RES_OK);
	else			nunit_ftor(NUNIT_RES_ERROR);

	// delete the http_sclient_res_t
	nipmem_zdelete	http_sclient;	

	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END

