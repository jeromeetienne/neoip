/*! \file
    \brief Header of the test of tcp_client_t
*/


#ifndef __NEOIP_TCP_CHARGEN_NUNIT_HPP__ 
#define __NEOIP_TCP_CHARGEN_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_tcp_chargen_client_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	tcp_chargen_server_t;
class	rate_sched_t;

/** \brief Class which implement a nunit for the tcp_client_t
 * 
 * - currently it only test if the connection works
 */
class tcp_chargen_testclass_t : public nunit_testclass_api_t, private tcp_chargen_client_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	tcp_chargen_server_t *	chargen_server;
	rate_sched_t *		recv_rsched;
	rate_sched_t *		xmit_rsched;

	/*************** tcp_chargen_client_t	*******************************/
	tcp_chargen_client_t*	chargen_client;
	bool			neoip_tcp_chargen_client_cb(void *userptr
						, tcp_chargen_client_t &cb_chargen_client
						, const inet_err_t &inet_err)	throw();
public:
	/*************** ctor/dtor	***************************************/
	tcp_chargen_testclass_t()		throw();
	~tcp_chargen_testclass_t()		throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_CHARGEN_NUNIT_HPP__  */



