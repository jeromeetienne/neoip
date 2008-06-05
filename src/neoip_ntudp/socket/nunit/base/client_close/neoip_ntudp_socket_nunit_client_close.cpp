/*! \file
    \brief Definition for ntudp_socket_nunit_client_close_t 

\brief Brief description
-# It connects a 'close server' with a ntudp_client_t.
-# if the connection succeed, report succeed
-# if the connection failed, report failed

*/

/* system include */
/* local include */
#include "neoip_ntudp_socket_nunit_client_close.hpp"
#include "neoip_ntudp_client.hpp"
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
ntudp_socket_nunit_client_close_t::ntudp_socket_nunit_client_close_t()	throw()
{
	// zero some field
	ntudp_client	= NULL;
}

/** \brief Destructor
 */
ntudp_socket_nunit_client_close_t::~ntudp_socket_nunit_client_close_t()	throw()
{
	// delete the ntudp_client if needed
	if( ntudp_client )	nipmem_delete	ntudp_client;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
ntudp_err_t	ntudp_socket_nunit_client_close_t::start(ntudp_peer_t *ntudp_peer
				, const ntudp_peerid_t &remote_peerid
				, ntudp_socket_nunit_client_close_cb_t *callback
				, void *userptr)	throw()
{
	ntudp_addr_t	remote_addr	= ntudp_addr_t(remote_peerid, "close_port");
	ntudp_err_t	ntudp_err;
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// init the ntudp_client on ntudp_peer
	ntudp_client	= nipmem_new ntudp_client_t();
	ntudp_err	= ntudp_client->start(ntudp_peer, remote_addr, this, NULL);
	if( ntudp_err.failed() )	return ntudp_err;
	// return no error
	return ntudp_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ntudp_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_client_t when it has an event to notify
 */
bool ntudp_socket_nunit_client_close_t::neoip_ntudp_client_event_cb(void *cb_userptr
							, ntudp_client_t &cb_ntudp_client
							, const ntudp_event_t &ntudp_event)	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_ERR("enter ntudp_event=" << ntudp_event);
	// sanity check - the notified event MUST be is_client_ok()
	DBG_ASSERT( ntudp_event.is_client_ok() );

	// if the ntudp_client_t report a fatal event, notify a error
	if( ntudp_event.is_fatal() )	return notify_callback(false);	

	// handle the event depending of its type
	switch( ntudp_event.get_value() ){
	case ntudp_event_t::CNX_ESTABLISHED:	// notify the caller of the sucess
						return notify_callback(true);
	default:	DBG_ASSERT( 0 );
	}	
	// return 'dontkeep'
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    main function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	ntudp_socket_nunit_client_close_t::notify_callback(bool succeed)		throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_ntudp_socket_nunit_client_close_cb(userptr, *this, succeed);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;	
}




NEOIP_NAMESPACE_END

