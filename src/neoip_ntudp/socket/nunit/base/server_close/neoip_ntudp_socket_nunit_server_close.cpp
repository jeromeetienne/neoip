/*! \file
    \brief Definition for ntudp_socket_nunit_server_close_t 

\brief Brief description
-# It start a ntudp_resp_t on port "close port"
-# then every received connection is immediatly closed

*/

/* system include */
/* local include */
#include "neoip_ntudp_socket_nunit_server_close.hpp"
#include "neoip_ntudp_resp.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_event.hpp"
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
ntudp_socket_nunit_server_close_t::ntudp_socket_nunit_server_close_t()	throw()
{
	// zero some field
	ntudp_resp	= NULL;
}

/** \brief Destructor
 */
ntudp_socket_nunit_server_close_t::~ntudp_socket_nunit_server_close_t()	throw()
{
	// delete the ntudp_server if needed
	if( ntudp_resp )	nipmem_delete	ntudp_resp;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
ntudp_err_t	ntudp_socket_nunit_server_close_t::start(ntudp_peer_t *ntudp_peer)	throw()
{
	ntudp_addr_t	listen_addr	= ntudp_addr_t(ntudp_peerid_t(), "close_port");
	ntudp_err_t	ntudp_err;
	// init the ntudp_server on ntudp_peer
	ntudp_resp	= nipmem_new ntudp_resp_t();
	ntudp_err	= ntudp_resp->start(ntudp_peer, listen_addr, this, NULL);
	if( ntudp_err.failed() )	return ntudp_err;
	// return no error
	return ntudp_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ntudp_resp_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_resp_t when it has an event to notify
 */
bool ntudp_socket_nunit_server_close_t::neoip_ntudp_resp_event_cb(void *cb_userptr
							, ntudp_resp_t &cb_ntudp_resp
							, const ntudp_event_t &ntudp_event)	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_ERR("enter ntudp_event=" << ntudp_event);
	// sanity check - the notified event MUST be is_resp_ok()
	DBG_ASSERT( ntudp_event.is_resp_ok() );

	// handle the event depending of its type
	switch( ntudp_event.get_value() ){
	case ntudp_event_t::CNX_ESTABLISHED: {
			// simply delete the just established ntudp_full_t
			ntudp_full_t *	ntudp_full	= ntudp_event.get_cnx_established();
			nipmem_delete	ntudp_full;
			break;}
	default:	DBG_ASSERT( 0 );
	}	
	// return 'tokeep'
	return true;
}





NEOIP_NAMESPACE_END

