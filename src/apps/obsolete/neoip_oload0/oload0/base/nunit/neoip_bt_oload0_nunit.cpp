/*! \file
    \brief Definition of the unit test for the \ref bt_oload0_t

*/

/* system include */
/* local include */
#include "neoip_bt_oload0_nunit.hpp"
#include "neoip_bt_oload0.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_oload0_testclass_t::bt_oload0_testclass_t()	throw()
{
	// zero some field
	bt_oload0	= NULL;
}

/** \brief Destructor
 */
bt_oload0_testclass_t::~bt_oload0_testclass_t()	throw()
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
nunit_err_t	bt_oload0_testclass_t::neoip_nunit_testclass_init()	throw()
{

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	bt_oload0_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the bt_oload0_t if needed
	nipmem_zdelete	bt_oload0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_oload0_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// start the bt_oload0_t
	bt_err_t	bt_err;
	bt_oload0	= nipmem_new bt_oload0_t();
	bt_err		= bt_oload0->start();
	NUNIT_ASSERT( bt_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

NEOIP_NAMESPACE_END

