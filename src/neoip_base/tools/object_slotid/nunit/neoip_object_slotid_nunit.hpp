/*! \file
    \brief Header of the test of object_slotid_t
*/


#ifndef __NEOIP_OBJECT_SLOTID_NUNIT_HPP__ 
#define __NEOIP_OBJECT_SLOTID_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the object_slotid_t
 */
class object_slotid_testclass_t : public nunit_testclass_api_t {
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	using_tokeep(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	copy_operator(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	normal_assignement(const nunit_testclass_ftor_t &testclass_ftor)	throw();	
	nunit_res_t	self_assignement(const nunit_testclass_ftor_t &testclass_ftor)		throw();	
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OBJECT_SLOTID_NUNIT_HPP__  */



