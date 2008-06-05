/*! \file
    \brief Definition of the unit test for the \ref router_name_t

*/

/* system include */
/* local include */
#include "neoip_router_name_nunit.hpp"
#include "neoip_router_name.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	router_name_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// test some is_null case
	NUNIT_ASSERT( router_name_t().is_null() );
	NUNIT_ASSERT( router_name_t("").is_null() );


	// log to debug
	KLOG_DBG("val=" << router_name_t("myhost.exampledomain.com"));
	KLOG_DBG("val=" << router_name_t("myhost.*"));
	KLOG_DBG("val=" << router_name_t("*.exampledomain.com"));

	// test some invalid case with the router_name_t::GLOB_STR
	NUNIT_ASSERT( router_name_t("*.slota.*").is_null() );
	NUNIT_ASSERT( router_name_t("*").is_null() );
	NUNIT_ASSERT( router_name_t("myhost.*.exampledomain.com").is_null() );
	NUNIT_ASSERT( router_name_t("myhost.ex#ampledomain.com").is_null() );
	
	// test query class function
	NUNIT_ASSERT( router_name_t("myhost.*").is_host_only() );
	NUNIT_ASSERT( router_name_t("*.exampledomain.com").is_domain_only() );
	NUNIT_ASSERT( router_name_t("myhost.exampledomain.com").is_fully_qualified() );
	NUNIT_ASSERT( router_name_t("myhost.exampledomain.com").domain() == "exampledomain.com" );
	NUNIT_ASSERT( router_name_t("myhost.exampledomain.com").host() == "myhost" );
	NUNIT_ASSERT( router_name_t("*.exampledomain.com").host() == "*" );
	NUNIT_ASSERT( router_name_t("myhost.*").domain() == "*" );
	
	// test contain function
	NUNIT_ASSERT(  router_name_t("*.exampledomain.com").match(router_name_t("myhost.exampledomain.com")) );
	NUNIT_ASSERT( !router_name_t("*.exampledomain.com").match(router_name_t("myhost.nogooddomain.com")) );

	// test set_domain
	NUNIT_ASSERT( router_name_t("myhost.*").add_domain(router_name_t("*.exampledomain.com")) == router_name_t("myhost.exampledomain.com") );
	NUNIT_ASSERT( router_name_t("myhost.*") / router_name_t("*.exampledomain.com") == router_name_t("myhost.exampledomain.com") );

	// report no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

