/*! \file
    \brief Header of the test of tokeep_check_t
*/


#ifndef __NEOIP_TOKEEP_CHECK_NUNIT_HPP__ 
#define __NEOIP_TOKEEP_CHECK_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the tokeep_check_t
 */
class tokeep_check_testclass_t : public nunit_testclass_api_t {
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	using_internal(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	using_define(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	copy_operator(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	assignement(const nunit_testclass_ftor_t &testclass_ftor)	throw();	
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TOKEEP_CHECK_NUNIT_HPP__  */



