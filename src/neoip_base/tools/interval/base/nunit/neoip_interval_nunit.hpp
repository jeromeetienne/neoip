

#ifndef __NEOIP_INTERVAL_NUNIT_HPP__ 
#define __NEOIP_INTERVAL_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the interval_t
 */
class interval_testclass_t : public nunit_testclass_api_t {
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	item_ctor(const nunit_testclass_ftor_t &testclass_ftor)			throw();
	nunit_res_t	item_serial_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	item_is_distinct(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	item_fully_include(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	interval_add_item(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	interval_sub_item(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	interval_serial_consistency(const nunit_testclass_ftor_t &testclass_ftor)		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_INTERVAL_NUNIT_HPP__  */



