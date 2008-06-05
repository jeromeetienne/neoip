/*! \file
    \brief Header of the unit test of the http_nested_uri_t layer
*/


#ifndef __NEOIP_HTTP_NESTED_URI_NUNIT_HPP__ 
#define __NEOIP_HTTP_NESTED_URI_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the http_nested_uri_t
 */
class http_nested_uri_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	is_valid_outter(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	nested_consistency1(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	nested_consistency2(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_NESTED_URI_NUNIT_HPP__  */



