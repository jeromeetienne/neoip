/*! \file
    \brief Header of the nunit_gsuite_t
*/


#ifndef __NEOIP_NTUDP_ADDR_NUNIT_HPP__ 
#define __NEOIP_NTUDP_ADDR_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Example of a nunit_testclass_api_t
 */
class ntudp_addr_testclass_t : public nunit_testclass_api_t {
public:
	nunit_res_t	serial_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	ctor_from_string(const nunit_testclass_ftor_t &testclass_ftor)		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_ADDR_NUNIT_HPP__  */



