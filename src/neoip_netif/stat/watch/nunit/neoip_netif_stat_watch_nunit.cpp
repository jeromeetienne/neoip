/*! \file
    \brief unit test for the netif_stat_watch_t unit test
*/

/* system include */
/* local include */
#include "neoip_netif_stat_watch_nunit.hpp"
#include "neoip_netif_stat_watch.hpp"
#include "neoip_netif_stat_arr.hpp"
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
netif_stat_watch_testclass_t::netif_stat_watch_testclass_t()	throw()
{
	// zero some field
	netif_stat_watch= NULL;
}

/** \brief Destructor
 */
netif_stat_watch_testclass_t::~netif_stat_watch_testclass_t()	throw()
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
nunit_err_t	netif_stat_watch_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check
	DBG_ASSERT( netif_stat_watch == NULL );

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	netif_stat_watch_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");	
	// delete the netif_stat_watch
	nipmem_zdelete	netif_stat_watch;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	netif_stat_watch_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	netif_err_t	netif_err;
	// sanity check - the netif_stat_watch MUST be NULL
	DBG_ASSERT( netif_stat_watch == NULL );
	
	// start the netif_stat_watch_t
	netif_stat_watch= nipmem_new netif_stat_watch_t();
	netif_err	= netif_stat_watch->start(this, NULL);
	NUNIT_ASSERT( netif_err.succeed() );
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     netif_stat_watch callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when netif_stat_watch_t has an event to report
 */
bool netif_stat_watch_testclass_t::neoip_netif_stat_watch_cb(void *cb_userptr
						, netif_stat_watch_t &cb_netif_stat_watch
						, const netif_stat_arr_t &netif_stat_arr)	throw()
{
	// log to debug 
	KLOG_DBG("enter netif_stat_arr=" << netif_stat_arr);

	// return no error
	nunit_ftor(NUNIT_RES_OK);

	// return 'tokeep'
	return true;
}

NEOIP_NAMESPACE_END
