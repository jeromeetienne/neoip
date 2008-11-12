/*! \file
    \brief Definition of the unit test for the \ref amf0_build_t

*/

/* system include */
#include <algorithm>
/* local include */
#include "neoip_rtmp_build_nunit.hpp"
#include "neoip_rtmp_build.hpp"
#include "neoip_rtmp_pkthd.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_amf0_build.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	rtmp_build_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_ERR("enter");
	// build the rtmp_pkdhd_t
	// - no need for populating rtmp_pkdhd_t::body_length() as it depends on the data
	rtmp_pkthd_t	rtmp_pkthd;
	rtmp_pkthd.channel_id(3);
	rtmp_pkthd.timestamp(delay_t::from_msec(0));
	rtmp_pkthd.type(rtmp_type_t::INVOKE);
	rtmp_pkthd.stream_id(0);

	// build rtmp_body
	bytearray_t	rtmp_body;
	amf0_build_t::to_amf0(dvar_str_t("_result")	, rtmp_body);
	amf0_build_t::to_amf0(dvar_dbl_t(1)		, rtmp_body);
	amf0_build_t::to_amf0(dvar_nil_t()		, rtmp_body);
	amf0_build_t::to_amf0(dvar_map_t()
			.insert("level"		, dvar_str_t("status"))
			.insert("description"	, dvar_str_t("Connection succeeded."))
			.insert("code"		, dvar_str_t("NetConnection.Connect.Success"))
			.insert("dummy"		, dvar_str_t("Hello world, im coding on sunday"))
							, rtmp_body);
	// log to debug
	KLOG_ERR("dvar.to_amf0=" << rtmp_body);

	bytearray_t	bytearray;
	rtmp_build_t::serialize(rtmp_pkthd, rtmp_body, bytearray);

	// log to debug
	KLOG_ERR("data=" << bytearray);

	// report no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

