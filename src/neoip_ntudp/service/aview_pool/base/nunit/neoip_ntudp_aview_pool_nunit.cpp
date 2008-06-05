/*! \file
    \brief Definition of the unit test for the \ref ntudp_aview_pool_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_aview_pool_nunit.hpp"
#include "neoip_ntudp_aview_pool.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_npos_saddrecho.hpp"
#include "neoip_ntudp_npos_event.hpp"
#include "neoip_ntudp_nunit_helper.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_aview_pool_testclass_t::ntudp_aview_pool_testclass_t()	throw()
{
	// zero some field
	ntudp_peer1		= NULL;
	ntudp_peer2		= NULL;
	ntudp_aview_pool	= NULL;
}

/** \brief Destructor
 */
ntudp_aview_pool_testclass_t::~ntudp_aview_pool_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	ntudp_aview_pool_testclass_t::neoip_nunit_testclass_init()	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_DBG("enter");
	
	// init the ntudp_peer1
	DBG_ASSERT( !ntudp_peer1 );
	ntudp_peer1	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_default(ntudp_peer1);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// init the ntudp_peer2
	DBG_ASSERT( !ntudp_peer2 );
	ntudp_peer2	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_inetreach(ntudp_peer2, "127.0.0.1:4001");
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	ntudp_aview_pool_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	
	// free the saddrecho_db
	while( !saddrecho_db.empty() ){
		nipmem_delete	saddrecho_db.front();
		saddrecho_db.pop_front();
	}
	// delete the ntudp_aview_pool
	if( ntudp_aview_pool ){
		ntudp_aview_pool->new_aview_unsubscribe(this, NULL);
		nipmem_delete	ntudp_aview_pool;
		ntudp_aview_pool	= NULL;
	}
	// delete the ntudp_peer1
	if( ntudp_peer1 ){
		nipmem_delete	ntudp_peer1;
		ntudp_peer1	= NULL;
	}
	// delete the ntudp_peer2
	if( ntudp_peer2 ){
		nipmem_delete	ntudp_peer2;
		ntudp_peer2	= NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general testing
 */
nunit_res_t ntudp_aview_pool_testclass_t::get_5_aview(const nunit_testclass_ftor_t &testclass_ftor)throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_DBG("enter");
	// create the ntudp_aview_pool_t and start it	
	ntudp_aview_pool	= nipmem_new ntudp_aview_pool_t();
	ntudp_err		= ntudp_aview_pool->start(ntudp_peer1, 1);
	NUNIT_ASSERT( ntudp_err.succeed() );
	// subscribe to new aview
	ntudp_aview_pool->new_aview_subscribe(this, NULL);
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief called when a event_hook_t level is notified
 * 
 * @return a 'tokeep' for the event_hook_t object
 */
bool	ntudp_aview_pool_testclass_t::neoip_event_hook_notify_cb(void *userptr
				, const event_hook_t *cb_event_hook, int hook_level)	throw()
{
	// log to debug
	KLOG_ERR("enter nb_pview_avail=" << ntudp_aview_pool->size());

	// if at least one ipport_aview_t is available, get it and push it in the pview_db
	while( ntudp_aview_pool->size() > 0 ){
		// get the ipport_aview_t from the pool
		ntudp_npos_saddrecho_t*	npos_saddrecho	= ntudp_aview_pool->steal_one_saddrecho();
		// assign the npos_saddrecho callback here
		npos_saddrecho->set_callback(this, NULL);
		// log to debug
		KLOG_ERR("ipport_aview=" << npos_saddrecho->local_addr_aview());

		// push this saddrecho_t in the saddrecho_db
		saddrecho_db.push_back(npos_saddrecho);
	}

	// Stop the test after X ipport_aview_t have been found
	if( saddrecho_db.size() > 5 )	return nunit_ftor(NUNIT_RES_OK);
	
	// return 'tokeep'
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ntudp_npos_saddrecho_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by ntudp_npos_saddrecho_t when the result is known
 */
bool ntudp_aview_pool_testclass_t::neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
					, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
					, const ntudp_npos_event_t &ntudp_npos_event)	throw() 
{
	// sanity check - the event MUST be npos_saddrecho_ok
	DBG_ASSERT( ntudp_npos_event.is_npos_saddrecho_ok() );
	// display the result
	KLOG_DBG("ntudp_npos_saddrecho notified ntudp_npos_event=" << ntudp_npos_event);

	/* NOTE:
	 * - this function is simply a place holder for the ntudp_npos_saddrecho_t of the database
	 */

	// return 'tokeep'
	return true;
}

NEOIP_NAMESPACE_END

