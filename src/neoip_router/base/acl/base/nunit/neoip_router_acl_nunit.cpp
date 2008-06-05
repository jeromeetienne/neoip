/*! \file
    \brief Definition of the unit test for the router_acl_t
    
*/

/* system include */
/* local include */
#include "neoip_router_acl_nunit.hpp"
#include "neoip_router_acl.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function for generation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test if the generation goes ok
 */
nunit_res_t	router_acl_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_ERR("enter");
	
	router_acl_t	router_acl;
	router_acl.append(router_acl_type_t::REJECT, "john.neoip");
	router_acl.append(router_acl_type_t::ACCEPT, "*.neoip");

	NUNIT_ASSERT( router_acl.size() == 2 );
	NUNIT_ASSERT( router_acl.reject("john.neoip") == true );
	NUNIT_ASSERT( router_acl.reject("smith.neoip") == false );
	NUNIT_ASSERT( router_acl.reject("notintheacl") == true );

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

