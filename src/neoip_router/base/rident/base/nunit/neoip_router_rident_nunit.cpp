/*! \file
    \brief Definition of the unit test for the router_rident_t
    
*/

/* system include */
/* local include */
#include "neoip_router_rident_nunit.hpp"
#include "neoip_router_lident.hpp"
#include "neoip_router_rident.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function for canonical_str consistency
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test if the canonical string convertion is consistent
 */
nunit_res_t	router_rident_testclass_t::canonical_str_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	router_lident_t	router_lident;
	// log to debug
	KLOG_ERR("enter");
	// build a new router_lident_t
	router_lident	= router_lident_t::generate(router_peerid_t::build_random()
						, router_name_t("john_smith.*"), 512
						, x509_cert_t(), x509_privkey_t());
	// check that the router_lident_t is NOT null
	NUNIT_ASSERT( !router_lident.is_null() );	
	// convert the router_lident_t to a router_rident_t
	router_rident_t	router_rident	= router_lident.to_rident();
	
	// convert the object to a canonical str
	std::string	canonical_str	= router_rident.to_canonical_string();
	// test if the canonical str is able to reconstruct the same object
	NUNIT_ASSERT( router_rident == router_rident_t(canonical_str) );
	
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

