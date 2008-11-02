/*! \file
    \brief Definition of the unit test for the \ref rtmp_resp_t

*/

/* system include */
/* local include */
#include "neoip_rtmp_resp_nunit.hpp"
#include "neoip_rtmp.hpp"
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
rtmp_resp_testclass_t::rtmp_resp_testclass_t()	throw()
{
	// zero some field
	rtmp_resp	= NULL;
}

/** \brief Destructor
 */
rtmp_resp_testclass_t::~rtmp_resp_testclass_t()	throw()
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
nunit_err_t	rtmp_resp_testclass_t::neoip_nunit_testclass_init()	throw()
{
	flv_err_t	flv_err;
	// build the resp_arg
	socket_resp_arg_t	resp_arg;
	resp_arg	= socket_resp_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
					.domain(socket_domain_t::TCP).type(socket_type_t::STREAM)
					.listen_addr("tcp://0.0.0.0:1935");
	// start the socket_resp_t
	rtmp_resp	= nipmem_new rtmp_resp_t();
	flv_err		= rtmp_resp->start(resp_arg);
	if( flv_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, flv_err.to_string() );
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	rtmp_resp_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete variables if needed
	nipmem_zdelete	rtmp_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	rtmp_resp_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rtmp_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_resp_t when to notify an event
 */
bool	rtmp_resp_testclass_t::neoip_rtmp_resp_cb(void *cb_userptr, rtmp_resp_t &cb_rtmp_resp
					, const rtmp_event_t &rtmp_event)	throw()
{
#if 0
	// log to debug
	KLOG_ERR("enter event=" << rtmp_event);

	// if rtmp_resp_event_t is fatal, report an error
	if( rtmp_event.is_fatal() ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
#endif
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END

