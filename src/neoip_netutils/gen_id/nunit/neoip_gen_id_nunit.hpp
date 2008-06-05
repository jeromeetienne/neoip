/*! \file
    \brief Header of the test of gen_id_t
*/


#ifndef __NEOIP_GEN_ID_NUNIT_HPP__ 
#define __NEOIP_GEN_ID_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the gen_id_t
 */
class gen_id_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	test_is_null(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	serial_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	xmlrpc_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	canonical_str_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	derivation_hash(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	bit_clear_n_highest(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	bit_get_highest_set(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	test_xor(const nunit_testclass_ftor_t &testclass_ftor)			throw();
	nunit_res_t	test_and(const nunit_testclass_ftor_t &testclass_ftor)			throw();
	nunit_res_t	test_rshift(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	comparison_op(const nunit_testclass_ftor_t &testclass_ftor)		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_GEN_ID_NUNIT_HPP__  */



