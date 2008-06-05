/*! \file
    \brief Definition of the unit test for the \ref http_listener_t

- TODO this is more a testbed than actual nunit
  - it depends on external configuration
  - the solution is to use client and server from this code and performe the test like this

*/

/* system include */
/* local include */
#include "neoip_http_listener_nunit.hpp"
#include "neoip_http_listener.hpp"
#include "neoip_socket_resp_arg.hpp"
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
http_listener_testclass_t::http_listener_testclass_t()	throw()
{
	// zero some field
	http_listener	= NULL;
}

/** \brief Destructor
 */
http_listener_testclass_t::~http_listener_testclass_t()	throw()
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
nunit_err_t	http_listener_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// sanity check
	DBG_ASSERT( !http_listener );
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	http_listener_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the http_listener_t
	nipmem_zdelete http_listener;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	http_listener_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	http_err_t	http_err;
	// log to debug
	KLOG_DBG("enter");

	// build the resp_arg
	socket_resp_arg_t	resp_arg;
	resp_arg	= socket_resp_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
				.domain(socket_domain_t::TCP)
				.type(socket_type_t::STREAM)
				.listen_addr("tcp://127.0.0.1:4000");
				
	// start the http_listener_t
	http_listener	= nipmem_new http_listener_t();
	http_err	= http_listener->start(resp_arg);
	NUNIT_ASSERT( http_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}


NEOIP_NAMESPACE_END

