/*! \file
    \brief Header of the nunit of neoip_profile.hpp
*/



#ifndef __NEOIP_PROFILE_NUNIT_HPP__ 
#define __NEOIP_PROFILE_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the profile_t
 */
class profile_testclass_t : public nunit_testclass_api_t {
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)			throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PROFILE_NUNIT_HPP__  */



