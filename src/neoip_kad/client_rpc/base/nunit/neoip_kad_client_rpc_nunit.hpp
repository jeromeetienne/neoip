/*! \file
    \brief Header of the test of kad_client_rpc_t
*/


#ifndef __NEOIP_KAD_CLIENT_RPC_NUNIT_HPP__ 
#define __NEOIP_KAD_CLIENT_RPC_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_kad_ping_rpc_cb.hpp"
#include "neoip_kad_findnode_rpc_cb.hpp"
#include "neoip_kad_findsomeval_rpc_cb.hpp"
#include "neoip_kad_findallval_rpc_cb.hpp"
#include "neoip_kad_delete_rpc_cb.hpp"
#include "neoip_kad_store_rpc_cb.hpp"
#include "neoip_cookie.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declarations
class	udp_vresp_t;
class	kad_listener_t;
class	kad_peer_t;

/** \brief Class which implement a nunit for all the RPC clients
 */
class kad_client_rpc_testclass_t : public nunit_testclass_api_t
					, private kad_ping_rpc_cb_t
					, private kad_findnode_rpc_cb_t
					, private kad_findsomeval_rpc_cb_t
					, private kad_store_rpc_cb_t
					, private kad_findallval_rpc_cb_t
					, private kad_delete_rpc_cb_t
					{
private:
	udp_vresp_t *	udp_vresp;	//!< the udp_vresp for the kad_listener_t
	kad_listener_t*	kad_listener;	//!< the kad_listener_t on which the test is done
	kad_peer_t *	kad_peer;	//!< the kad_peer_t started in the kad_listener_t
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	cookie_id_t	cookie_id;
	
	/*************** callback dispatchers	*******************************/
	bool		neoip_kad_ping_rpc_cb(void *cb_userptr, kad_ping_rpc_t &cb_kad_ping_rpc
							, const kad_event_t &kad_event)	throw();
	bool		neoip_kad_findsomeval_rpc_cb(void *cb_userptr, kad_findsomeval_rpc_t &cb_kad_findsomeval_rpc
							, const kad_event_t &kad_event)	throw();
	bool		neoip_kad_findallval_rpc_cb(void *cb_userptr, kad_findallval_rpc_t &cb_kad_findallval_rpc
							, const kad_event_t &kad_event)	throw();
														
	/*************** ping_doreach_peer	*******************************/
	kad_ping_rpc_t *ping_doreach_peer;
	bool		ping_doreach_peer_cb(void *cb_userptr, kad_ping_rpc_t &cb_kad_ping_rpc
							, const kad_event_t &kad_event)	throw();

	/*************** ping_unreach_peer	*******************************/
	kad_ping_rpc_t *ping_unreach_peer;
	bool		ping_unreach_peer_cb(void *cb_userptr, kad_ping_rpc_t &cb_kad_ping_rpc
							, const kad_event_t &kad_event)	throw();

	/*************** findnode	***************************************/
	kad_findnode_rpc_t *findnode;
	bool		neoip_kad_findnode_rpc_cb(void *cb_userptr, kad_findnode_rpc_t &cb_kad_findnode_rpc
							, const kad_event_t &kad_event)	throw();

	/*************** findsomeval_fornode	*******************************/
	kad_findsomeval_rpc_t *findsomeval_fornode;
	bool		findsomeval_fornode_cb(void *cb_userptr, kad_findsomeval_rpc_t &cb_kad_findsomeval_rpc
							, const kad_event_t &kad_event)	throw();

	/*************** store	***********************************************/
	kad_store_rpc_t *store;
	bool		neoip_kad_store_rpc_cb(void *cb_userptr, kad_store_rpc_t &cb_kad_store_rpc
							, const kad_event_t &kad_event)	throw();

	/*************** findsomeval_forval	*******************************/
	kad_findsomeval_rpc_t *findsomeval_forval;
	bool		findsomeval_forval_cb(void *cb_userptr, kad_findsomeval_rpc_t &cb_kad_findsomeval_rpc
							, const kad_event_t &kad_event)	throw();

	/*************** findallval_do_found	*******************************/
	kad_findallval_rpc_t *findallval_do_found;
	bool		findallval_do_found_cb(void *cb_userptr, kad_findallval_rpc_t &cb_kad_findallval_rpc
							, const kad_event_t &kad_event)	throw();

	/*************** delete_record	***************************************/
	kad_delete_rpc_t *delete_record;
	bool		neoip_kad_delete_rpc_cb(void *cb_userptr, kad_delete_rpc_t &cb_kad_delete_rpc
							, const kad_event_t &kad_event)	throw();

	/*************** findallval_notfound	*******************************/
	kad_findallval_rpc_t *findallval_notfound;
	bool		findallval_notfound_cb(void *cb_userptr, kad_findallval_rpc_t &cb_kad_findallval_rpc
							, const kad_event_t &kad_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	kad_client_rpc_testclass_t()	throw();
	~kad_client_rpc_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	test_ping_doreach_peer(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	test_ping_unreach_peer(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	test_findnode(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	test_findsomeval_fornode(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	test_store(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	test_findsomeval_forval(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	test_findallval_do_found(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	test_delete_record(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	test_findallval_notfound(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_CLIENT_RPC_NUNIT_HPP__  */



