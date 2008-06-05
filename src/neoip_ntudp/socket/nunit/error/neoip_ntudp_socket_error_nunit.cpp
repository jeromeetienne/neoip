/*! \file
    \brief Definition of the unit test for the ntudp socket with DIRECT connection
    
*/

/* system include */
/* local include */
#include "neoip_ntudp_socket_error_nunit.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_itor.hpp"
#include "neoip_ntudp_client.hpp"
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
ntudp_socket_error_testclass_t::ntudp_socket_error_testclass_t()	throw()
{
	// zero some field
	ntudp_peer1	= NULL;
	ntudp_peer2	= NULL;
	ntudp_itor	= NULL;
	ntudp_client	= NULL;
}

/** \brief Destructor
 */
ntudp_socket_error_testclass_t::~ntudp_socket_error_testclass_t()	throw()
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
nunit_err_t	ntudp_socket_error_testclass_t::neoip_nunit_testclass_init()	throw()
{
	ntudp_err_t	ntudp_err;
	// init the ntudp_peer1
	DBG_ASSERT( !ntudp_peer1 );
	ntudp_peer1	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_natsym(ntudp_peer1);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// init the ntudp_peer2
	DBG_ASSERT( !ntudp_peer2 );
	ntudp_peer2	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_natsym(ntudp_peer2);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	ntudp_socket_error_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// delete the ntudp_itor
	if( ntudp_itor ){
		nipmem_delete	ntudp_itor;
		ntudp_itor	= NULL;
	}
	// delete the ntudp_client
	if( ntudp_client ){
		nipmem_delete	ntudp_client;
		ntudp_client	= NULL;
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
//                      stuff for unknown_host error
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test for UNKNOWN_HOST ntudp_event_t
 */
nunit_res_t	ntudp_socket_error_testclass_t::unknown_host(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	ntudp_err_t	ntudp_err;
	ntudp_addr_t	remote_addr	= ntudp_addr_t(ntudp_peerid_t::build_random()
								, ntudp_portid_t::build_random());
	// init the ntudp_itor on ntudp_peer
	ntudp_itor	= nipmem_new ntudp_itor_t();
	ntudp_err	= ntudp_itor->start(ntudp_peer1, remote_addr, this, NULL);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified by \ref ntudp_itor_t when it has an event to notify
 */
bool ntudp_socket_error_testclass_t::neoip_ntudp_itor_event_cb(void *cb_userptr
			, ntudp_itor_t &cb_ntudp_itor, const ntudp_event_t &ntudp_event)	throw()
{
	// log to debug
	KLOG_ERR("enter ntudp_event=" << ntudp_event);
	// sanity check - the notified event MUST be is_itor_ok()
	DBG_ASSERT( ntudp_event.is_itor_ok() );

	// delete the ntudp_itor_t and mark is unused
	nipmem_delete	ntudp_itor;
	ntudp_itor	= NULL;

	// if the ntudp_itor_t report a fatal event, notify a error
	if( ntudp_event.is_unknown_host() ){
		nunit_ftor(NUNIT_RES_OK);
	}else{
		nunit_ftor(NUNIT_RES_ERROR);
	}
	// return 'dontkeep'
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      stuff for no_route_to_host error
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test for NO_ROUTE_TO_HOST ntudp_event_t
 */
nunit_res_t	ntudp_socket_error_testclass_t::no_route_to_host(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	ntudp_err_t	ntudp_err;
	ntudp_addr_t	remote_addr	= ntudp_addr_t(ntudp_peer2->local_peerid()
								, ntudp_portid_t::build_random());
	// init the ntudp_client on ntudp_peer
	ntudp_client	= nipmem_new ntudp_client_t();
	ntudp_err	= ntudp_client->start(ntudp_peer1, remote_addr, this, NULL);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}


/** \brief callback notified by \ref ntudp_client_t when it has an event to notify
 */
bool ntudp_socket_error_testclass_t::neoip_ntudp_client_event_cb(void *cb_userptr
			, ntudp_client_t &cb_ntudp_client, const ntudp_event_t &ntudp_event)	throw()
{
	// log to debug
	KLOG_ERR("enter ntudp_event=" << ntudp_event);
	// sanity check - the notified event MUST be is_client_ok()
	DBG_ASSERT( ntudp_event.is_client_ok() );

	// delete the ntudp_client_t and mark is unused
	nipmem_delete	ntudp_client;
	ntudp_client	= NULL;

	// if the ntudp_client_t report a fatal event, notify a error
	if( ntudp_event.is_no_route_to_host() ){
		nunit_ftor(NUNIT_RES_OK);
	}else{
		nunit_ftor(NUNIT_RES_ERROR);
	}
	// return 'dontkeep'
	return false;
}



NEOIP_NAMESPACE_END

