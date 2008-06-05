/*! \file
    \brief Header of the nunit_gsuite_t
*/


#ifndef __NEOIP_DELAY2_NUNIT_HPP__ 
#define __NEOIP_DELAY2_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Example of a nunit_testclass_api_t
 */
class delay2_testclass_t : public nunit_testclass_api_t {
private:
public:
	nunit_res_t	basic_query(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	comparison_operator(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	arithmetic_operator(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	serial_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAY2_NUNIT_HPP__  */



