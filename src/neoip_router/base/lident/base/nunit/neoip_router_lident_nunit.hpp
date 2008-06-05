

#ifndef __NEOIP_ROUTER_LIDENT_NUNIT_HPP__ 
#define __NEOIP_ROUTER_LIDENT_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the router_lident_t
 */
class router_lident_testclass_t : public nunit_testclass_api_t {
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	generation(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	canonical_str_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	signature_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_LIDENT_NUNIT_HPP__  */



