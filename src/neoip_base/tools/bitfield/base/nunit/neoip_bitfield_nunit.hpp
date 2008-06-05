/*! \file
    \brief Header of the test of bitfield_t
*/


#ifndef __NEOIP_BITFIELD_NUNIT_HPP__ 
#define __NEOIP_BITFIELD_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	bitfield_t;

/** \brief Class which implement a nunit for the bitfield_t
 */
class bitfield_testclass_t : public nunit_testclass_api_t {
private:
	bool		is_serial_consistent(const bitfield_t &bitfield)	const throw();
	bitfield_t	build_random(size_t nb_bit)					const throw();
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	is_null(const nunit_testclass_ftor_t &testclass_ftor)			throw();
	nunit_res_t	to_canonical_string(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	comparison(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	get_next_set_unset(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	bool_operation(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	serial_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BITFIELD_NUNIT_HPP__  */



