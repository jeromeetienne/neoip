

#ifndef __NEOIP_NTUDP_ITOR_RETRY_NUNIT_HPP__ 
#define __NEOIP_NTUDP_ITOR_RETRY_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_ntudp_itor_retry_cb.hpp"
#include "neoip_ntudp_resp_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_peer_t;

/** \brief Class which implement a nunit for the ntudp_socket_t
 */
class ntudp_itor_retry_testclass_t : public nunit_testclass_api_t, private timeout_cb_t
					, private ntudp_itor_retry_cb_t
					, private ntudp_resp_cb_t
					{
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously

	ntudp_peer_t *	ntudp_peer1;
	ntudp_peer_t *	ntudp_peer2;
	
	/*************** ntudp_itor_t stuff	*******************************/
	ntudp_itor_retry_t *	itor_retry;
	bool neoip_ntudp_itor_retry_event_cb(void *cb_userptr, ntudp_itor_retry_t &cb_ntudp_itor_retry
							, const ntudp_event_t &ntudp_event)	throw();

	/*************** timeout_t before launching dest session	*******/
	timeout_t	dest_timeout;	//!< the timeout to wait for the end of the bootstrap
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw();

	/*************** ntudp_resp_t stuff	*******************************/
	ntudp_resp_t *	ntudp_resp;
	bool neoip_ntudp_resp_event_cb(void *cb_userptr, ntudp_resp_t &cb_ntudp_resp
							, const ntudp_event_t &ntudp_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_itor_retry_testclass_t()	throw();
	~ntudp_itor_retry_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_ITOR_RETRY_NUNIT_HPP__  */



