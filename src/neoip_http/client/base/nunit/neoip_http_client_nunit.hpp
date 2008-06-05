/*! \file
    \brief Header of the test of http_client_t
*/


#ifndef __NEOIP_HTTP_CLIENT_NUNIT_HPP__ 
#define __NEOIP_HTTP_CLIENT_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_http_client_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the http_client_t
 * 
 * - currently it only test if the connection works
 */
class http_client_testclass_t : public nunit_testclass_api_t, private http_client_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 

	/*************** http_client_t	*******************************/
	http_client_t *http_client;
	bool 		neoip_http_client_cb(void *cb_userptr, http_client_t &cb_http_client
				, const http_err_t &http_err, const http_rephd_t &http_rephd
				, socket_full_t *socket_full, const bytearray_t &recved_data)	throw();
public:
	/*************** ctor/dtor	***************************************/
	http_client_testclass_t()	throw();
	~http_client_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	one_cnx(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_CLIENT_NUNIT_HPP__  */



