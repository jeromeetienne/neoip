/*! \file
    \brief Header of the test of obj_factory_t
*/


#ifndef __NEOIP_OBJ_FACTORY_NUNIT_HPP__ 
#define __NEOIP_OBJ_FACTORY_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the obj_factory_t
 */
class obj_factory_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OBJ_FACTORY_NUNIT_HPP__  */



