/*! \file
    \brief Definition of the \ref rtmp_resp_t

*/

/* system include */
/* local include */
#include "neoip_rtmp_resp.hpp"
#include "neoip_rtmp_resp_cnx.hpp"
#include "neoip_rtmp_full.hpp"
#include "neoip_socket_resp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

// definition of \ref rtmp_parse_profile_t constant
const size_t	rtmp_resp_t::HANDSHAKE_PADLEN	= 1536;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
rtmp_resp_t::rtmp_resp_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	socket_resp	= NULL;
}

/** \brief Destructor
 */
rtmp_resp_t::~rtmp_resp_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// close all pending rtmp_resp_cnx_t
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
	// delete the socket_resp if needed
	nipmem_zdelete	socket_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
rtmp_err_t	rtmp_resp_t::start(const socket_resp_arg_t &resp_arg
			, rtmp_resp_cb_t *callback, void *userptr)	throw()
{
	// copy the callback parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// start the socket_resp_t
	socket_err_t	socket_err;
	socket_resp	= nipmem_new socket_resp_t();
	socket_err	= socket_resp->start(resp_arg, this, NULL);
	if( socket_err.failed() )	return rtmp_err_from_socket(socket_err);

	// return no error
	return rtmp_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_resp_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_resp_t when a connection is established
 */
bool	rtmp_resp_t::neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << socket_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( socket_event.is_resp_ok() );

	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:{
			rtmp_resp_cnx_t *	resp_cnx;
			rtmp_err_t	rtmp_err;
			resp_cnx	= nipmem_new rtmp_resp_cnx_t();
			rtmp_err	= resp_cnx->start(this, socket_event.get_cnx_established());
			if( rtmp_err.failed() )	nipmem_delete resp_cnx;
			return true;}
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 *
 * @return a tokeep
 */
bool 	rtmp_resp_t::notify_callback(rtmp_full_t *rtmp_full)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_rtmp_resp_cb(userptr, *this, rtmp_full);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END;




