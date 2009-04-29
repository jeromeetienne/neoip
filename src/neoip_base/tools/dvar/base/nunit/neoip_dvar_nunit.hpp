/*! \file
    \brief Header of the nunit of dvar_t
*/


#ifndef __NEOIP_DVAR_NUNIT_HPP__
#define __NEOIP_DVAR_NUNIT_HPP__
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Example of a nunit_testclass_api_t
 */
class dvar_testclass_t : public nunit_testclass_api_t {
private:
public:
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	integer(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	dbl(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	string(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	boolean(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	nil(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	array(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	map(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	to_xml(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	to_http_query(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	from_http_query(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DVAR_NUNIT_HPP__  */



