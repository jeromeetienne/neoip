/*! \file
    \brief Header of the unit test of the nudp_t layer
*/


#ifndef __NEOIP_NUDP_NUNIT_HPP__ 
#define __NEOIP_NUDP_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_nudp_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
// NONE...

/** \brief Class which implement a nunit for the nudp_listener_t
 */
class nudp_testclass_t : public nunit_testclass_api_t,  private nudp_cb_t, private timeout_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	size_t			xmit_count;
	/*************** nudp	***************************************/
	nudp_t *	nudp;
	bool		neoip_inet_nudp_event_cb(void *cb_userptr, nudp_t &cb_nudp, pkt_t &pkt
					, const ipport_addr_t &local_addr
					, const ipport_addr_t &remote_addr)	throw();

	/*************** rxmit_timeout	***************************************/
	timeout_t	rxmit_timeout;
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	nudp_testclass_t()	throw();
	~nudp_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUDP_NUNIT_HPP__  */



