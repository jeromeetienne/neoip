/*! \file
    \brief Definition of the unit test for the \ref amf0_build_t

*/

/* system include */
/* local include */
#include "neoip_amf0_nunit.hpp"
#include "neoip_amf0_build.hpp"
#include "neoip_amf0_parse.hpp"
#include "neoip_flv_err.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	amf0_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// setup some dvar_t to do a comparison
	dvar_t	dvar0	= dvar_str_t("_result");
	dvar_t	dvar1	= dvar_dbl_t(1);
	dvar_t	dvar2	= dvar_nil_t();
	dvar_t	dvar3	= dvar_map_t()
			.insert("level"		, dvar_str_t("status"))
			.insert("description"	, dvar_str_t("Connection succeeded."))
			.insert("code"		, dvar_str_t("NetConnection.Connect.Success"));



	// build amf0_data from a dvar_t
	bytearray_t	amf0_data;
	amf0_build_t::to_amf0(dvar0, amf0_data);
	amf0_build_t::to_amf0(dvar1, amf0_data);
	amf0_build_t::to_amf0(dvar2, amf0_data);
	amf0_build_t::to_amf0(dvar3, amf0_data);
	// log to debug
	KLOG_ERR("dvar.to_amf0=" << amf0_data);

	// parse the
	flv_err_t	flv_err;
	dvar_t		dvar;
	flv_err		= amf0_parse_t::amf_to_dvar(amf0_data, dvar);
	NUNIT_ASSERT( flv_err.succeed() );
	NUNIT_ASSERT( dvar == dvar0 );
	flv_err		= amf0_parse_t::amf_to_dvar(amf0_data, dvar);
	NUNIT_ASSERT( flv_err.succeed() );
	NUNIT_ASSERT( dvar == dvar1 );
	flv_err		= amf0_parse_t::amf_to_dvar(amf0_data, dvar);
	NUNIT_ASSERT( flv_err.succeed() );
	NUNIT_ASSERT( dvar == dvar2 );
	flv_err		= amf0_parse_t::amf_to_dvar(amf0_data, dvar);
	NUNIT_ASSERT( flv_err.succeed() );
	NUNIT_ASSERT( dvar == dvar3 );
	KLOG_ERR("dvar=" << dvar);

	// report no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

