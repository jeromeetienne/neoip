/*! \file
    \brief Definition of the unit test for the \ref rtmp_parse_t

*/

/* system include */
/* local include */
#include "neoip_rtmp_parse_nunit.hpp"
#include "neoip_rtmp_parse.hpp"
#include "neoip_rtmp_event.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_amf0_parse.hpp"
#include "neoip_flv_err.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
rtmp_parse_testclass_t::rtmp_parse_testclass_t()	throw()
{
	// zero some field
	rtmp_parse	= NULL;
}

/** \brief Destructor
 */
rtmp_parse_testclass_t::~rtmp_parse_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	rtmp_parse_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// open the rtmp_parse_t
	rtmp_err_t	rtmp_err;
	rtmp_parse	= nipmem_new rtmp_parse_t();
	rtmp_err	= rtmp_parse->start(this, NULL);
	if( rtmp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, rtmp_err.to_string() );

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	rtmp_parse_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete variables if needed
	nipmem_zdelete	rtmp_parse;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	rtmp_parse_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	std::string sample_pkt_str(
		"\x03\x00\x00\x01\x00\x01\x54\x14\x00\x00\x00\x00\x02\x00\x07\x63"
		"\x6f\x6e\x6e\x65\x63\x74\x00\x3f\xf0\x00\x00\x00\x00\x00\x00\x03"
		"\x00\x03\x61\x70\x70\x02\x00\x04\x6c\x69\x76\x65\x00\x08\x66\x6c"
		"\x61\x73\x68\x56\x65\x72\x02\x00\x0d\x4c\x4e\x58\x20\x39\x2c\x30"
		"\x2c\x31\x32\x34\x2c\x30\x00\x06\x73\x77\x66\x55\x72\x6c\x02\x00"
		"\x32\x68\x74\x74\x70\x3a\x2f\x2f\x6c\x6f\x63\x61\x6c\x68\x6f\x73"
		"\x74\x2f\x7e\x6a\x65\x72\x6f\x6d\x65\x2f\x72\x65\x61\x64\x77\x65"
		"\x62\x63\x61\x6d\x2f\x72\x65\x61\x64\x77\x65\x62\x63\x61\x6d\x2e"
		"\x73\x77\x66\x00\x05\x74\x63\x55\x72\x6c\x02\x00\xc3\x15\x72\x74"
		"\x6d\x70\x3a\x2f\x2f\x31\x32\x37\x2e\x30\x2e\x30\x2e\x31\x2f\x6c"
		"\x69\x76\x65\x00\x04\x66\x70\x61\x64\x01\x00\x00\x0c\x63\x61\x70"
		"\x61\x62\x69\x6c\x69\x74\x69\x65\x73\x00\x40\x2e\x00\x00\x00\x00"
		"\x00\x00\x00\x0b\x61\x75\x64\x69\x6f\x43\x6f\x64\x65\x63\x73\x00"
		"\x40\x99\x9c\x00\x00\x00\x00\x00\x00\x0b\x76\x69\x64\x65\x6f\x43"
		"\x6f\x64\x65\x63\x73\x00\x40\x6f\x80\x00\x00\x00\x00\x00\x00\x0d"
		"\x76\x69\x64\x65\x6f\x46\x75\x6e\x63\x74\x69\x6f\x6e\x00\x3f\xf0"
		"\x00\x00\x00\x00\x00\x00\x00\x07\x70\x61\x67\x65\x55\xc3\x72\x6c"
		"\x02\x00\x33\x68\x74\x74\x70\x3a\x2f\x2f\x6c\x6f\x63\x61\x6c\x68"
		"\x6f\x73\x74\x2f\x7e\x6a\x65\x72\x6f\x6d\x65\x2f\x72\x65\x61\x64"
		"\x77\x65\x62\x63\x61\x6d\x2f\x72\x65\x61\x64\x77\x65\x62\x63\x61"
		"\x6d\x2e\x68\x74\x6d\x6c\x00\x0e\x6f\x62\x6a\x65\x63\x74\x45\x6e"
		"\x63\x6f\x64\x69\x6e\x67\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		"\x00\x09", 354);
	datum_t	sample_pkt(sample_pkt_str);
	// log to debug
	KLOG_ERR("sample_pkt = " << sample_pkt);
	// notify this data to rtmp_parse_t
	rtmp_parse->notify_data(sample_pkt);
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rtmp_parse_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_parse_t when to notify an event
 */
bool	rtmp_parse_testclass_t::neoip_rtmp_parse_cb(void *cb_userptr, rtmp_parse_t &cb_rtmp_parse
					, const rtmp_event_t &rtmp_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << rtmp_event);
	// sanity check - rtmp_event_t MUST be is_parse_ok()
	DBG_ASSERT( rtmp_event.is_parse_ok() );

	// if rtmp_event_t is fatal, report an error
	if( rtmp_event.is_fatal() ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}

	// sanity check - at this point, rtmp_event_t MUST be a rtmp_event_t::PACKET
	DBG_ASSERT( rtmp_event.value() == rtmp_event_t::PACKET );

	// get the packet from the event
	datum_t		pktbody;
	rtmp_pkthd_t	rtmp_pkthd	= rtmp_event.get_packet(&pktbody);
	KLOG_ERR("rtmp_pkthd=" << rtmp_pkthd);


	// here parse the body just for the sake of it

	// some variable for the amf0_parse_t
	flv_err_t	flv_err;
	dvar_t		dvar;
	bytearray_t	amf0_barray(pktbody);

	// get the initial string
	dvar		= amf0_parse_t::parser(amf0_barray);
	DBG_ASSERT( dvar.type().get_value() == dvar_type_t::STRING );
	DBG_ASSERT( dvar.str().get() == "connect" );
	KLOG_ERR("dvar="<< dvar);

	// get the following double
	dvar		= amf0_parse_t::parser(amf0_barray);
	DBG_ASSERT( dvar.type().get_value() == dvar_type_t::DOUBLE );
	DBG_ASSERT( dvar.dbl().get() == 1.0 );
	KLOG_ERR("dvar="<< dvar);

	// get the following map
	dvar		= amf0_parse_t::parser(amf0_barray);
	DBG_ASSERT( dvar.map()["app"].str().get()		== "live" );
	DBG_ASSERT( dvar.map()["audioCodecs"].dbl().get()	== 1639.0 );
	DBG_ASSERT( dvar.map()["flashVer"].str().get()		== "LNX 9,0,124,0" );
	KLOG_ERR("dvar="<< dvar);


	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END

