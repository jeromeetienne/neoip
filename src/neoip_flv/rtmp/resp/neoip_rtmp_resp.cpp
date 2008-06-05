/*! \file
    \brief Definition of the \ref rtmp_resp_t

*/

/* system include */
/* local include */
#include "neoip_rtmp_resp.hpp"
#include "neoip_socket_resp.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
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
flv_err_t	rtmp_resp_t::start(const socket_resp_arg_t &resp_arg)	throw()
{
	socket_err_t	socket_err;
	// start the socket_resp_t
	socket_resp	= nipmem_new socket_resp_t();
	socket_err	= socket_resp->start(resp_arg, this, NULL);
	if( socket_err.failed() )	return flv_err_from_socket(socket_err);	
	
	// return no error
	return flv_err_t::OK;
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
			socket_full_t *	socket_full	= socket_event.get_cnx_established();
			nipmem_zdelete	socket_full;
			return true;}
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END;




