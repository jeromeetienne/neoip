/*! \file
    \brief Header of the test of tcp_client_t
*/


#ifndef __NEOIP_TCP_CLIENT_NUNIT_HPP__ 
#define __NEOIP_TCP_CLIENT_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_tcp_resp_cb.hpp"
#include "neoip_tcp_client_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the tcp_client_t
 * 
 * - currently it only test if the connection works
 */
class tcp_client_testclass_t : public nunit_testclass_api_t, private tcp_client_cb_t
						, private tcp_resp_cb_t	{
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	
	/*************** tcp_resp_t	***************************************/
	tcp_resp_t *	tcp_resp;
	bool		neoip_tcp_resp_event_cb(void *userptr, tcp_resp_t &cb_tcp_resp
						, const tcp_event_t &tcp_event)	throw();

	/*************** tcp_client_t	*******************************/
	tcp_client_t *	tcp_client;
	bool		neoip_tcp_client_event_cb(void *userptr, tcp_client_t &cb_tcp_client
				, const tcp_event_t &tcp_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	tcp_client_testclass_t()	throw();
	~tcp_client_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	cnx_establishement(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_CLIENT_NUNIT_HPP__  */



