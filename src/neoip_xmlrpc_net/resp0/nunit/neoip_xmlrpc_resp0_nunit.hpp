/*! \file
    \brief Header of the test of xmlrpc_resp0_t
*/


#ifndef __NEOIP_XMLRPC_RESP0_NUNIT_HPP__ 
#define __NEOIP_XMLRPC_RESP0_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_http_sclient_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	xmlrpc_resp0_t;

/** \brief Class which implement a nunit for the xmlrpc_resp0_t
 */
class xmlrpc_resp0_testclass_t : public nunit_testclass_api_t, private http_sclient_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	xmlrpc_resp0_t *		xmlrpc_resp0;
	/*************** http_client_t	***************************************/
	http_sclient_t *http_sclient;
	bool 		neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
						, const http_sclient_res_t &sclient_res)	throw();
public:
	/*************** ctor/dtor	***************************************/
	xmlrpc_resp0_testclass_t()	throw();
	~xmlrpc_resp0_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_RESP0_NUNIT_HPP__  */



