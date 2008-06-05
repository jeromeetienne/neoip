/*! \file
    \brief Definition of the unit test for the \ref kad_rpcresp_t

*/

/* system include */
/* local include */
#include "neoip_kad_rpcresp_nunit.hpp"
#include "neoip_kad_rpcresp.hpp"
#include "neoip_kad_err.hpp"
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
kad_rpcresp_testclass_t::kad_rpcresp_testclass_t()	throw()
{
	// zero some variable
	kad_rpcresp	= NULL;
}

/** \brief Destructor
 */
kad_rpcresp_testclass_t::~kad_rpcresp_testclass_t()	throw()
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
nunit_err_t	kad_rpcresp_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	kad_rpcresp_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the kad_rpcresp_t if needed
	nipmem_zdelete	kad_rpcresp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	kad_rpcresp_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	kad_err_t	kad_err;
	// log to debug
	KLOG_DBG("enter");
#if 0
	// return no error
	return NUNIT_RES_OK;
#endif
	// start the kad_rpcresp_t
	kad_rpcresp	= nipmem_new kad_rpcresp_t();
	kad_err		= kad_rpcresp->start("/kadrpc");
	NUNIT_ASSERT( kad_err.succeed() );
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

NEOIP_NAMESPACE_END

