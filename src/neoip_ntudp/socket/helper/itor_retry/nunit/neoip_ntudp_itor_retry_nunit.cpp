/*! \file
    \brief Definition of the unit test for \ref ntudp_itor_retry_t

\par Brief Description
-# Start one peer and start ntudp_itor_retry_t on it
-# wait a bit (during this time the ntudp_itor_retry_t will failed on UNKNOWN_HOST)
-# Start another peer with a responder on it
-# succeed if the ntudp_itor_retry_t receives connection established
-# fails if ntudp_itor_retry_t receives any other event.

*/

/* system include */
/* local include */
#include "neoip_ntudp_itor_retry_nunit.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_itor_retry.hpp"
#include "neoip_ntudp_resp.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_ntudp_nunit_helper.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_itor_retry_testclass_t::ntudp_itor_retry_testclass_t()	throw()
{
	// zero some field
	ntudp_peer1	= NULL;
	ntudp_peer2	= NULL;
	ntudp_resp	= NULL;
	itor_retry	= NULL;
}

/** \brief Destructor
 */
ntudp_itor_retry_testclass_t::~ntudp_itor_retry_testclass_t()	throw()
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
nunit_err_t	ntudp_itor_retry_testclass_t::neoip_nunit_testclass_init()	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_DBG("enter");
	
	// init the ntudp_peer1
	DBG_ASSERT( !ntudp_peer1 );
	ntudp_peer1	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_inetreach(ntudp_peer1, "127.0.0.1:4000");	
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	ntudp_itor_retry_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// delete the ntudp_itor
	if( itor_retry ){
		nipmem_delete	itor_retry;
		itor_retry	= NULL;
	}
	// delete the ntudp_resp
	if( ntudp_resp ){
		nipmem_delete	ntudp_resp;
		ntudp_resp	= NULL;
	}
	// delete the ntudp_peer1
	if( ntudp_peer1 ){
		nipmem_delete	ntudp_peer1;
		ntudp_peer1	= NULL;
	}
	// delete the ntudp_peer2
	if( ntudp_peer2 ){
		nipmem_delete	ntudp_peer2;
		ntudp_peer2	= NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit test function
////////////////////////////////////////////////////////////////////////////////


/** \brief general testing
 */
nunit_res_t	ntudp_itor_retry_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ntudp_addr_t	remote_addr	= "resp peerid:resp portid";
	ntudp_err_t	ntudp_err;
	// init the itor_retry on ntudp_peer1
	itor_retry	= nipmem_new ntudp_itor_retry_t();
	ntudp_err	= itor_retry->start(ntudp_peer1, remote_addr, this, NULL);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());
	
	// Start the timeout
	dest_timeout.start(delay_t::from_sec(1), this, NULL);
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief callback called when the timeout_t expire
 */
bool 	ntudp_itor_retry_testclass_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_ERR("enter");
	// stop the timeout
	dest_timeout.stop();
	
	// init the ntudp_peer2
	DBG_ASSERT( !ntudp_peer2 );
	ntudp_peer2	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_inetreach(ntudp_peer2, "127.0.0.1:4001", "resp peerid");	
	DBG_ASSERT( ntudp_err.succeed() );

	// init the ntudp_resp on ntudp_peer2
	ntudp_resp	= nipmem_new ntudp_resp_t();
	ntudp_err	= ntudp_resp->start(ntudp_peer2, ":resp portid", this, NULL);
	DBG_ASSERT( ntudp_err.succeed() );
	
	// return 'tokeep'
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      ntudp_resp_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_resp_t when a connection is established
 */
bool ntudp_itor_retry_testclass_t::neoip_ntudp_resp_event_cb(void *cb_userptr, ntudp_resp_t &cb_ntudp_resp
							, const ntudp_event_t &ntudp_event)	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_ERR("enter ntudp_event=" << ntudp_event);
	// sanity check - the notified event MUST be is_resp_ok()
	DBG_ASSERT( ntudp_event.is_resp_ok() );
	
	// handle the event depending of its type
	switch( ntudp_event.get_value() ){
	case ntudp_event_t::CNX_ESTABLISHED:{
			ntudp_full_t *	ntudp_full	= ntudp_event.get_cnx_established();
			nipmem_delete	ntudp_full;
			break;}
	default:	DBG_ASSERT( 0 );
	}	
	
	// return tokeep
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ntudp_itor_retry_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_itor_retry_t when it has an event to notify
 */
bool ntudp_itor_retry_testclass_t::neoip_ntudp_itor_retry_event_cb(void *cb_userptr
							, ntudp_itor_retry_t &cb_ntudp_itor_retry
							, const ntudp_event_t &ntudp_event)	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_ERR("enter ntudp_event=" << ntudp_event);
	// sanity check - the notified event MUST be is_itor_ok()
	DBG_ASSERT( ntudp_event.is_itor_ok() );

	// delete the ntudp_itor_t and mark it unused
	nipmem_zdelete	itor_retry;

	// if the ntudp_itor_retry_t report a fatal event, notify a nunit error
	if( ntudp_event.is_fatal() ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}

	// handle the event depending of its type
	switch( ntudp_event.get_value() ){
	case ntudp_event_t::CNX_ESTABLISHED:
			nipmem_delete	ntudp_event.get_cnx_established();
			neoip_nunit_testclass_deinit();
			nunit_ftor(NUNIT_RES_OK);
			break;
	default:	DBG_ASSERT( 0 );
	}	
	// return 'dontkeep'
	return false;
}



NEOIP_NAMESPACE_END

