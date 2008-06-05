/*! \file
    \brief Definition of the unit test for the router_lident_t
    
*/

/* system include */
/* local include */
#include "neoip_router_lident_nunit.hpp"
#include "neoip_router_lident.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function for generation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test if the generation goes ok
 */
nunit_res_t	router_lident_testclass_t::generation(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	router_lident_t	router_lident;
	// log to debug
	KLOG_ERR("enter");
	// check that the router_lident_t is null
	NUNIT_ASSERT( router_lident.is_null() );
	// build a new router_lident_t
	router_lident	= router_lident_t::generate(router_peerid_t::build_random()
					, router_name_t("john_smith.*"), 512
					, x509_cert_t(), x509_privkey_t());
	// check that the router_lident_t is NOT null
	NUNIT_ASSERT( !router_lident.is_null() );
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function for canonical_str consistency
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test if the canonical string convertion is consistent
 */
nunit_res_t	router_lident_testclass_t::canonical_str_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
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
	
	// convert the object to a canonical str
	std::string	canonical_str	= router_lident.to_canonical_string();
	// test if the canonical str is able to reconstruct the same object
	NUNIT_ASSERT( router_lident == router_lident_t(canonical_str) );
	
	
	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function for signature consistency
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test if the router_lident_t is able to verify what it signs
 */
nunit_res_t	router_lident_testclass_t::signature_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
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
	
	// sign a dummy data
	datum_t		data_datum = datum_t("Hello world");
	datum_t		sign_datum = router_lident.privkey().sign_data(data_datum);
	// check if the signature has been produced
	NUNIT_ASSERT( !sign_datum.is_null() );
	// check if the produced signature is valid with the certificate
	NUNIT_ASSERT( router_lident.cert().verify_data(data_datum, sign_datum).succeed() );

	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

