

#ifndef __NEOIP_NTUDP_CLIENT_NUNIT_HPP__ 
#define __NEOIP_NTUDP_CLIENT_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_ntudp_client_cb.hpp"
#include "neoip_ntudp_resp_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_peer_t;

/** \brief Class which implement a nunit for the ntudp_socket_t
 */
class ntudp_client_testclass_t : public nunit_testclass_api_t
					, private ntudp_client_cb_t
					, private ntudp_resp_cb_t
					{
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously
	
	ntudp_peer_t *	ntudp_peer;
	
	/*************** ntudp_itor_t stuff	*******************************/
	ntudp_client_t*	ntudp_client;
	bool neoip_ntudp_client_event_cb(void *cb_userptr, ntudp_client_t &cb_ntudp_client
							, const ntudp_event_t &ntudp_event)	throw();

	/*************** ntudp_resp_t stuff	*******************************/
	ntudp_resp_t *	ntudp_resp;
	bool neoip_ntudp_resp_event_cb(void *cb_userptr, ntudp_resp_t &cb_ntudp_resp
							, const ntudp_event_t &ntudp_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_client_testclass_t()	throw();
	~ntudp_client_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_CLIENT_NUNIT_HPP__  */



