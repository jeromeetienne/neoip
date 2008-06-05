/*! \file
    \brief Definition of the unit test for the \ref ntudp_pserver_pool_t

\par Brief description
-# launch a \ref ntudp_peer_t as inetreach KO
-# scan the pool
-# wait a bit
-# launch another \ref ntudp_peer_t as inetreach OK
-# wait for the new one to appears

*/

/* system include */
/* local include */
#include "neoip_ntudp_pserver_pool_nunit.hpp"
#include "neoip_ntudp_pserver_pool.hpp"
#include "neoip_ntudp_peer.hpp"
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
ntudp_pserver_pool_testclass_t::ntudp_pserver_pool_testclass_t()	throw()
{
	// zero some field
	ntudp_peer1	= NULL;
	ntudp_peer2	= NULL;
	pserver_pool	= NULL;
}

/** \brief Destructor
 */
ntudp_pserver_pool_testclass_t::~ntudp_pserver_pool_testclass_t()	throw()
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
nunit_err_t	ntudp_pserver_pool_testclass_t::neoip_nunit_testclass_init()	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_DBG("enter");
	
	// init the ntudp_peer1
	DBG_ASSERT( !ntudp_peer1 );
	ntudp_peer1	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_default(ntudp_peer1);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	ntudp_pserver_pool_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// delete the pserver_pool
	if( pserver_pool ){
		pserver_pool->new_unknown_unsubscribe(this, NULL);
		nipmem_delete	pserver_pool;
		pserver_pool	= NULL;
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

/** \brief called when a event_hook_t level is notified
 * 
 * @return a 'tokeep' for the event_hook_t object
 */
bool ntudp_pserver_pool_testclass_t::neoip_event_hook_notify_cb(void *userptr
				, const event_hook_t *cb_event_hook, int hook_level) throw()
{
	// log to debug
	KLOG_ERR("enter");
	// scan the unknown database of the pserver_pool_t, and return OK if the ntudp_peer2 is found
	for(size_t i = 0; i < pserver_pool->unknown_size(); i++){
		// if the i-th element if not the ntudp_peer2, skip it
		if( pserver_pool->unknown_at(i) != ntudp_peer2->listen_addr_pview() )	continue;
		// if it is, return OK
		return nunit_ftor(NUNIT_RES_OK);
	}

	// ask for more if the ntudp_peer2 has not been found
	pserver_pool->get_more_unknown();

	// return 'tokeep'
	return true;
}

/** \brief general testing
 */
nunit_res_t	ntudp_pserver_pool_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_ERR("enter");

	// create the ntudp_pserver_pool_t and start it	
	pserver_pool	= nipmem_new ntudp_pserver_pool_t();
	ntudp_err	= pserver_pool->start(ntudp_peer1);
	NUNIT_ASSERT( ntudp_err.succeed() );

	// subscribe to new unknown
	pserver_pool->new_unknown_subscribe(this, NULL);
	// ask for more
	pserver_pool->get_more_unknown();
	
	// start the timeout to init ntudp_peer2
	timeout.start(delay_t::from_msec(500), this, NULL);

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback called when the timeout_t expire
 */
bool 	ntudp_pserver_pool_testclass_t::neoip_timeout_expire_cb(void *userptr
						, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter expiration for ntudp_peer2 creation");
	// stop the timeout
	timeout.stop();

	// init the ntudp_peer2
	DBG_ASSERT( !ntudp_peer2 );
	ntudp_err_t	ntudp_err;
	ntudp_peer2	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_inetreach(ntudp_peer2, "127.0.0.1:4001");	
	EXP_ASSERT( ntudp_err.succeed() );

	// report 'tokeep
	return true;
}


NEOIP_NAMESPACE_END

