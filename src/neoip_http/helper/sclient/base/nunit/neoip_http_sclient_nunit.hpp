/*! \file
    \brief Header of the http_sclient_t callback
*/


#ifndef __NEOIP_HTTP_SCLIENT_NUNIT_HPP__ 
#define __NEOIP_HTTP_SCLIENT_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_http_sclient_cb.hpp"
#include "neoip_http_sresp_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	http_client_pool_t;
class	http_listener_t;

/** \brief Class which implement a nunit for the http_client_t
 * 
 * - currently it only test if the connection works
 */
class http_sclient_testclass_t : public nunit_testclass_api_t, private http_sclient_cb_t
				, private http_sresp_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	http_client_pool_t *	client_pool;
	size_t			nb_request;
	http_listener_t *	http_listener;

	/*************** http_sresp_t	***************************************/
	http_sresp_t *		http_sresp;
	bool			neoip_http_sresp_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
						, http_sresp_ctx_t &sresp_ctx)	throw();
	/*************** http_client_t	***************************************/
	http_sclient_t *	http_sclient;
	bool 			neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
					, const http_sclient_res_t &sclient_res)throw();
public:
	/*************** ctor/dtor	***************************************/
	http_sclient_testclass_t()	throw();
	~http_sclient_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	one_cnx(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SCLIENT_NUNIT_HPP__  */



