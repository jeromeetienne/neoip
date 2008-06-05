/*! \file
    \brief Header of the test of http_rephd_t
*/


#ifndef __NEOIP_HTTP_REPHD_NUNIT_HPP__ 
#define __NEOIP_HTTP_REPHD_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the http_rephd_t
 */
class http_rephd_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	serial_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	header_content_length(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	header_content_range(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_REPHD_NUNIT_HPP__  */



