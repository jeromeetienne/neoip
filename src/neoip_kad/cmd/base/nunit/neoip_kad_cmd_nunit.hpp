/*! \file
    \brief Header of the test of kad_listener_t
*/


#ifndef __NEOIP_KAD_CMD_NUNIT_HPP__ 
#define __NEOIP_KAD_CMD_NUNIT_HPP__ 
/* system include */
#include <vector>
#include <list>
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_timeout.hpp"
#include "neoip_kad_store_cb.hpp"
#include "neoip_kad_query_cb.hpp"
#include "neoip_kad_query_some_cb.hpp"
#include "neoip_kad_delete_cb.hpp"
#include "neoip_kad_closestnode_cb.hpp"
#include "neoip_kad_rec.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declarations
class	udp_vresp_t;
class	kad_listener_t;
class	kad_peer_t;

/** \brief Class which implement a nunit for the kad_store_t/kad_query_t/kad_delete_t
 */
class kad_cmd_testclass_t : public nunit_testclass_api_t, private timeout_cb_t 
						, private kad_store_cb_t
						, private kad_query_cb_t
						, private kad_query_some_cb_t
						, private kad_delete_cb_t
						, private kad_closestnode_cb_t
						{
public:	/////////////////////// constant declaration ///////////////////////////
	//!< the number of kad_peer_t to launch
	static const size_t	NB_PEER_TO_LAUNCH;
	//!< delay_t to wait to let the kad_peer_t's bootsrap with each other
	static const delay_t	PEER_BSTRAP_DELAY;
private:
	udp_vresp_t *			udp_vresp;	//!< the udp_vresp for the kad_listener_t
	kad_listener_t*			kad_listener;	//!< the kad_listener_t on which the test is done
	std::vector<kad_peer_t *>	peer_db;	//!< list of all the kad_peer_t
	std::vector<kad_rec_t>		record_db;

	/*************** bsrap_timeout	***************************************/
	timeout_t	bstrap_timeout;
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();	

	/*************** kad_query_t callback dispatcher	***************/
	bool neoip_kad_query_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw();

	/*************** kad_query_some_t callback dispatcher	***************/
	bool neoip_kad_query_some_cb(void *cb_userptr, kad_query_some_t &cb_kad_query_some
						, const kad_event_t &kad_event)	throw();
												
	/*************** kad_store_t	***************************************/
	std::list<kad_store_t *>	store_db;
	bool neoip_kad_store_cb(void *cb_userptr, kad_store_t &cb_kad_store
						, const kad_event_t &kad_event)	throw();

	/*************** query_some_dofound	***************************************/
	std::list<kad_query_some_t *>	query_some_dofound_db;
	bool query_some_dofound_cb(void *cb_userptr, kad_query_some_t &cb_kad_query_some
						, const kad_event_t &kad_event)	throw();
					
	/*************** query_dofound	***************************************/
	std::list<kad_query_t *>	query_dofound_db;
	bool query_dofound_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw();

	/*************** kad_delete_t	***************************************/
	std::list<kad_delete_t *>	delete_db;
	bool neoip_kad_delete_cb(void *cb_userptr, kad_delete_t &cb_kad_delete
						, const kad_event_t &kad_event)	throw();

	/*************** query_some_nofound	***************************************/
	std::list<kad_query_some_t *>	query_some_nofound_db;
	bool query_some_nofound_cb(void *cb_userptr, kad_query_some_t &cb_kad_query_some
						, const kad_event_t &kad_event)	throw();
						
	/*************** query_nofound	***************************************/
	std::list<kad_query_t *>	query_nofound_db;
	bool query_nofound_cb(void *cb_userptr, kad_query_t &cb_kad_query
						, const kad_event_t &kad_event)	throw();

	/*************** kad_closestnode_t	***************************************/
	std::list<kad_closestnode_t *>	closestnode_db;
	bool neoip_kad_closestnode_cb(void *cb_userptr, kad_closestnode_t &cb_kad_closestnode
						, const kad_event_t &kad_event)	throw();
							
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
public:
	/*************** ctor/dtor	***************************************/
	kad_cmd_testclass_t()	throw();
	~kad_cmd_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	wait_for_bstrap(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	store_record(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	query_some_dofound(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	query_dofound(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	delete_record(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	query_some_nofound(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	query_nofound(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	closestnode(const nunit_testclass_ftor_t &testclass_ftor)		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_CMD_NUNIT_HPP__  */



