/*! \file
    \brief unit test for the rate_sched_t
*/

/* system include */
/* local include */
#include "neoip_rate_sched_nunit.hpp"
#include "neoip_rate_sched_nunit_item.hpp"
#include "neoip_rate_sched.hpp"
#include "neoip_rate_limit.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
rate_sched_testclass_t::rate_sched_testclass_t()	throw()
{
	// zero some field
	rate_sched	= NULL;
}

/** \brief Destructor
 */
rate_sched_testclass_t::~rate_sched_testclass_t()	throw()
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
nunit_err_t	rate_sched_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	rate_sched_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all elements of item_db
	while( !item_db.empty() )	nipmem_delete *item_db.begin();
	// delete the rate_sched_t
	nipmem_zdelete	rate_sched;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the serial consistency
 */
nunit_res_t	rate_sched_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	rate_err_t		rate_err;
	rate_sched_nunit_item_t*nunit_item;
	// log to debug
	KLOG_ERR("enter");

	// init the rate_sched_t
	rate_sched	= nipmem_new rate_sched_t();
	rate_err	= rate_sched->start(300*1000);
	NUNIT_ASSERT( rate_err.succeed() );

	// create a rate_sched_nunit_item_t
	nunit_item	= nipmem_new rate_sched_nunit_item_t();
	rate_err	= nunit_item->start(this, rate_prec_t(100), delay_t::from_sec(10), 5*1000*1000);
	NUNIT_ASSERT( rate_err.succeed() );	
	
	// create a rate_sched_nunit_item_t
	nunit_item	= nipmem_new rate_sched_nunit_item_t();
	rate_err	= nunit_item->start(this, rate_prec_t(51), delay_t::from_sec(5), 2*1000*1000);
	NUNIT_ASSERT( rate_err.succeed() );

	// create a rate_sched_nunit_item_t
	nunit_item	= nipmem_new rate_sched_nunit_item_t();
	rate_err	= nunit_item->start(this, rate_prec_t(50), delay_t::from_sec(0), 1000*1000*1000);
	NUNIT_ASSERT( rate_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return no error
	return NUNIT_RES_DELAYED;
}


NEOIP_NAMESPACE_END
