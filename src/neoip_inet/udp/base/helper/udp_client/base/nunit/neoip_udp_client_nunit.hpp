/*! \file
    \brief Header of the test of udp_client_t
*/


#ifndef __NEOIP_UDP_CLIENT_NUNIT_HPP__ 
#define __NEOIP_UDP_CLIENT_NUNIT_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_udp_resp_cb.hpp"
#include "neoip_udp_full_cb.hpp"
#include "neoip_udp_client_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the udp_client_t
 * 
 * - currently it only test if the connection works
 */
class udp_client_testclass_t : public nunit_testclass_api_t, private udp_resp_cb_t 
				, private udp_full_cb_t, private udp_client_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 

	/*************** udp_client_t	***************************************/
	std::list<udp_client_t *>	udp_client_db;
	bool		neoip_udp_client_event_cb(void *userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw();

	/*************** udp_resp_t	***************************************/
	udp_resp_t *	udp_resp;	//!< the udp_resp_t on which to listen
	bool		neoip_inet_udp_resp_event_cb(void *userptr, udp_resp_t &cb_udp_resp
						, const udp_event_t &udp_event)		throw();
	/*************** udp_full_db	***************************************/
	std::list<udp_full_t *>	udp_full_db;	//!< the udp_full_t 
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
					, const udp_event_t &udp_event)		throw();

public:
	/*************** ctor/dtor	***************************************/
	udp_client_testclass_t()	throw();
	~udp_client_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	echo_localhost_inetd(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_CLIENT_NUNIT_HPP__  */



