/*! \file
    \brief Header of the nunit_gsuite_t
*/


#ifndef __NEOIP_COMPRESS_NUNIT_HPP__ 
#define __NEOIP_COMPRESS_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_datum.hpp"
#include "neoip_compress_type.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief nunit_testclass_api_t for the compress_t
 */
class compress_testclass_t : public nunit_testclass_api_t {
private:
	nunit_res_t	test_identity_consistency(const compress_type_t &compress_type
						, const datum_t &orig_datum)	const throw();

public:
	nunit_res_t	test_identity(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_COMPRESS_NUNIT_HPP__  */



