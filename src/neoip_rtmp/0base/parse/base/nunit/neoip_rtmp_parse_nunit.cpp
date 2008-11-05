/*! \file
    \brief Definition of the unit test for the \ref rtmp_parse_t

*/

/* system include */
/* local include */
#include "neoip_rtmp_parse_nunit.hpp"
#include "neoip_rtmp_parse.hpp"
#include "neoip_rtmp_event.hpp"
#include "neoip_rtmp_err.hpp"
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
					, const rtmp_event_t &parse_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << parse_event);

	// if rtmp_event_t is fatal, report an error
	if( parse_event.is_fatal() ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}

	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END

