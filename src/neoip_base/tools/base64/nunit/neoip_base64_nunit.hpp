/*! \file
    \brief Header of the test of base64_t
*/


#ifndef __NEOIP_BASE64_NUNIT_HPP__ 
#define __NEOIP_BASE64_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the base64_t
 */
class base64_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	norm_alphabet(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	safe_alphabet(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	self_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BASE64_NUNIT_HPP__  */



