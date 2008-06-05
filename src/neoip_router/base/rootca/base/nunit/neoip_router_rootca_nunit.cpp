/*! \file
    \brief Definition of the unit test for the router_rootca_t
    
*/

/* system include */
/* local include */
#include "neoip_router_rootca_nunit.hpp"
#include "neoip_router_rootca.hpp"
#include "neoip_x509.hpp"
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
nunit_res_t	router_rootca_testclass_t::canonical_str_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	x509_privkey_t	rootca_privkey;
	x509_request_t	rootca_request;
	x509_cert_t	rootca_cert;
	crypto_err_t	crypto_err;
	// log to debug
	KLOG_DBG("enter");
	// generate the private key
	crypto_err	= rootca_privkey.generate(512);
	NUNIT_ASSERT( crypto_err.succeed() );
	// generate the certificate request needed to build the certificate 
	crypto_err	= rootca_request.generate("superdomain ca", rootca_privkey);
	NUNIT_ASSERT( crypto_err.succeed() );
	// generate the selfsigned certificate for the just built request
	crypto_err	= rootca_cert.generate_selfsigned(rootca_request, rootca_privkey);
	NUNIT_ASSERT( crypto_err.succeed() );
	
	// build the domain_db
	item_arr_t<router_name_t>	domain_db;
	domain_db	+= router_name_t("*.superdomain");
	// build the router_rootca_t
	router_rootca_t	router_rootca(rootca_cert, domain_db);
	
	// convert the object to a canonical str
	std::string	canonical_str	= router_rootca.to_canonical_string();
	// test if the canonical str is able to reconstruct the same object
	NUNIT_ASSERT( router_rootca == router_rootca_t(canonical_str) );
	
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

