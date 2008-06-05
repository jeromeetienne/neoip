/*! \file
    \brief Definition of the unit test for \ref ntudp_client_t

\par Brief Description
-# Start one peer
-# create a ntudp_resp_t
-# create a ntudp_client_t
-# succeed if the ntudp_client_t receives connection established
-# fails if ntudp_client_t receives any other event.

*/

/* system include */
/* local include */
#include "neoip_ntudp_client_nunit.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_client.hpp"
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
ntudp_client_testclass_t::ntudp_client_testclass_t()	throw()
{
	// zero some field
	ntudp_peer	= NULL;
	ntudp_resp	= NULL;
	ntudp_client	= NULL;
}

/** \brief Destructor
 */
ntudp_client_testclass_t::~ntudp_client_testclass_t()	throw()
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
nunit_err_t	ntudp_client_testclass_t::neoip_nunit_testclass_init()	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_DBG("enter");
	
	// init the ntudp_peer1
	DBG_ASSERT( !ntudp_peer );
	ntudp_peer	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_inetreach(ntudp_peer, "127.0.0.1:4000");
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	ntudp_client_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// delete the ntudp_client_t
	nipmem_zdelete	ntudp_client;
	// delete the ntudp_resp_t
	nipmem_zdelete	ntudp_resp;
	// delete the ntudp_peer_t
	nipmem_zdelete	ntudp_peer;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit test function
////////////////////////////////////////////////////////////////////////////////


/** \brief general testing
 */
nunit_res_t	ntudp_client_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	ntudp_err_t	ntudp_err;
	// init the ntudp_resp on ntudp_peer1
	ntudp_resp	= nipmem_new ntudp_resp_t();
	ntudp_err	= ntudp_resp->start(ntudp_peer, ntudp_addr_t(), this, NULL);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// init the ntudp_client on ntudp_peer1
	ntudp_client	= nipmem_new ntudp_client_t();
	ntudp_err	= ntudp_client->start(ntudp_peer, ntudp_resp->listen_addr(), this, NULL);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      ntudp_resp_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_resp_t when a connection is established
 */
bool ntudp_client_testclass_t::neoip_ntudp_resp_event_cb(void *cb_userptr, ntudp_resp_t &cb_ntudp_resp
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
//                       ntudp_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_client_t when it has an event to notify
 */
bool ntudp_client_testclass_t::neoip_ntudp_client_event_cb(void *cb_userptr
							, ntudp_client_t &cb_ntudp_client
							, const ntudp_event_t &ntudp_event)	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_ERR("enter ntudp_event=" << ntudp_event);
	// sanity check - the notified event MUST be is_client_ok()
	DBG_ASSERT( ntudp_event.is_client_ok() );

	// delete the ntudp_itor_t and mark it unused
	nipmem_zdelete	ntudp_client;

	// if the ntudp_client_t report a fatal event, notify a nunit error
	if( ntudp_event.is_fatal() ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}

	// handle the event depending of its type
	switch( ntudp_event.get_value() ){
	case ntudp_event_t::CNX_ESTABLISHED:
			neoip_nunit_testclass_deinit();
			nunit_ftor(NUNIT_RES_OK);
			break;
	default:	DBG_ASSERT( 0 );
	}	
	// return 'dontkeep'
	return false;
}



NEOIP_NAMESPACE_END

