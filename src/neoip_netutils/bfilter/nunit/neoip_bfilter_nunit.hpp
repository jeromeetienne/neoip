/*! \file
    \brief Header of the test of bfilter_t
*/


#ifndef __NEOIP_BFILTER_NUNIT_HPP__ 
#define __NEOIP_BFILTER_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the bfilter_t
 */
class bfilter_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	insert(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	unify(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	intersect(const nunit_testclass_ftor_t &testclass_ftor)		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BFILTER_NUNIT_HPP__  */



