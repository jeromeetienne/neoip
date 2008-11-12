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
	rtmp_full	= NULL;
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
	rtmp_err_t	rtmp_err;
	// build the resp_arg
	socket_resp_arg_t	resp_arg;
	resp_arg	= socket_resp_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
					.domain(socket_domain_t::TCP).type(socket_type_t::STREAM)
					.listen_addr("tcp://0.0.0.0:1935");
	// start the socket_resp_t
	rtmp_resp	= nipmem_new rtmp_resp_t();
	rtmp_err	= rtmp_resp->start(resp_arg, this, NULL);
	if( rtmp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, rtmp_err.to_string() );
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
	nipmem_zdelete	rtmp_full;
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
						, rtmp_full_t *cb_rtmp_full)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// if there is already a rtmp_full_t initialized, delete this one immediatly
	if( this->rtmp_full ){
		// log to debug
		KLOG_ERR("one rtmp_full is already handled. dropping new one");
		nipmem_zdelete	cb_rtmp_full;
		return true;
	}

	// copy the parameter
	rtmp_full	= cb_rtmp_full;
	// start the rtmp_full_t
	rtmp_err_t	rtmp_err;
	rtmp_err	= rtmp_full->start(this, NULL);
	if( rtmp_err.failed() ){
		KLOG_ERR("Cant start the rtmp_full_t due to " << rtmp_err);
		nipmem_zdelete rtmp_full;
		return true;
	}

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rtmp_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_resp_t when to notify an event
 */
bool	rtmp_resp_testclass_t::neoip_rtmp_full_cb(void *cb_userptr, rtmp_full_t &cb_rtmp_full
						, const rtmp_event_t &rtmp_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << rtmp_event);

	// if rtmp_event.is_fatal(), delete the rtmp_full_t
	if( rtmp_event.is_fatal() ){
		// log to debug
		KLOG_ERR("delete rtmp_full_t due to " << rtmp_event);
		// delete rtmp_full_t
		nipmem_zdelete	rtmp_full;
		// return dontkeep as rtmp_full_t just got deleted
		return false;
	}

	// sanity check - at this point, rtmp_event_t MUST be a rtmp_event_t:PACKET
	DBG_ASSERT( rtmp_event.is_packet() );

	// return tokeep
	return true;
}
NEOIP_NAMESPACE_END

