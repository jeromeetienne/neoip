/*! \file
    \brief Header of the nunit_gsuite_t
*/


#ifndef __NEOIP_DELAYGEN_NUNIT_HPP__ 
#define __NEOIP_DELAYGEN_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Example of a nunit_testclass_api_t
 */
class delaygen_testclass_t : public nunit_testclass_api_t {
private:
public:
	nunit_res_t	delaygen_arg(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	regular_nonrandom(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	regular_random(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	expboff_nonrandom(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	expboff_random(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAYGEN_NUNIT_HPP__  */



