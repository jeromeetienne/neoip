/*! \file
    \brief Definition of the unit test for the \ref bencode_t
*/

/* system include */
/* local include */
#include "neoip_bencode_nunit.hpp"
#include "neoip_bencode.hpp"
#include "neoip_dvar.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief function to test a bencode_t
 */
nunit_res_t	bencode_testclass_t::encode(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// create a dvar
	dvar_t	dvar	= dvar_map_t();
	dvar.map().insert("bar", dvar_str_t("spam"));
	dvar.map().insert("foo", dvar_int_t(42));
	// encode and compare the result	
	NUNIT_ASSERT( bencode_t::from_dvar(dvar) == "d3:bar4:spam3:fooi42ee" );
	
	// return no error
	return NUNIT_RES_OK;
}

/** \brief function to test a bencode_t
 */
nunit_res_t	bencode_testclass_t::decode(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// create a dvar
	dvar_t	dvar	= dvar_map_t();
	dvar.map().insert("bar", dvar_str_t("spam"));
	dvar.map().insert("foo", dvar_int_t(42));
	// decode and compare the result	
	NUNIT_ASSERT( bencode_t::to_dvar("d3:bar4:spam3:fooi42ee") == dvar );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief function to test a bencode_t
 */
nunit_res_t	bencode_testclass_t::consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// create a dvar
	dvar_t	dvar1	= dvar_map_t();
	dvar1.map().insert("bar", dvar_str_t("spam"));
	dvar1.map().insert("foo", dvar_int_t(42));
	// encode and decode
	std::string	encoded	= bencode_t::from_dvar(dvar1);
	dvar_t		dvar2	= bencode_t::to_dvar(encoded);
	// check the identity
	NUNIT_ASSERT( dvar1 == dvar2 );
	
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

