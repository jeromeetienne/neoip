/*! \file
    \brief Header of the test of kad_client_rpc_t
*/


#ifndef __NEOIP_KAD_PUBLISH_NUNIT_HPP__ 
#define __NEOIP_KAD_PUBLISH_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_kad_publish_cb.hpp"
#include "neoip_kad_query_cb.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declarations
class	udp_vresp_t;
class	kad_listener_t;
class	kad_peer_t;

/** \brief Class which implement a nunit for all the RPC clients
 */
class kad_publish_testclass_t : public nunit_testclass_api_t
					, private kad_publish_cb_t
					, private kad_query_cb_t
					{
private:
	udp_vresp_t *	udp_vresp;	//!< the udp_vresp for the kad_listener_t
	kad_listener_t*	kad_listener;	//!< the kad_listener_t on which the test is done
	kad_peer_t *	kad_peer;	//!< the kad_peer_t started in the kad_listener_t
	
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	

	/*************** kad_query_t callback dispatcher	***************/
	bool 		neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
							, const kad_event_t &kad_event)	throw();	
	/*************** kad_publish_t	***************************************/
	kad_publish_t *	kad_publish;
	bool		neoip_kad_publish_cb(void *cb_userptr, kad_publish_t &cb_kad_publish
							, const kad_event_t &kad_event)	throw();	

	/*************** query_dofound	***************************************/
	kad_query_t *	query_dofound;
	bool		query_dofound_cb(void *cb_userptr, kad_query_t &cb_kad_query
							, const kad_event_t &kad_event)	throw();

	/*************** query_dofound	***************************************/
	kad_query_t *	query_nofound;
	bool 		query_nofound_cb(void *cb_userptr, kad_query_t &cb_kad_query
							, const kad_event_t &kad_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	kad_publish_testclass_t()	throw();
	~kad_publish_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	test_publish_start(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	test_query_dofound(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	test_publish_stop(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	test_query_nofound(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_PUBLISH_NUNIT_HPP__  */



