/*! \file
    \brief Header of the nunit_gsuite_t
*/


#ifndef __NEOIP_STRING_NUNIT_HPP__ 
#define __NEOIP_STRING_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Example of a nunit_testclass_api_t
 */
class string_testclass_t : public nunit_testclass_api_t {
private:
public:
	nunit_res_t	test_split(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	test_replace(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	test_casecmp(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	test_to_upperlower(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	test_escape(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	test_strip(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	test_nice_string(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	test_to_uint(const nunit_testclass_ftor_t &testclass_ftor)		throw();	
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_STRING_NUNIT_HPP__  */



