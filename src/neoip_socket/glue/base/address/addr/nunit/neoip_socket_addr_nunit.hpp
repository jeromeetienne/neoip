/*! \file
    \brief Header of the test of socket_addr_t
*/


#ifndef __NEOIP_SOCKET_ADDR_NUNIT_HPP__ 
#define __NEOIP_SOCKET_ADDR_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the socket_addr_t
 */
class socket_addr_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	test_ctor(const nunit_testclass_ftor_t &testclass_ftor)			throw();
	nunit_res_t	comparison_operator(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	copy_operator(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	serial_with_null(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	serial_with_domain(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_ADDR_NUNIT_HPP__  */



