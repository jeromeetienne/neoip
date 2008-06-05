/*! \file
    \brief Definition of the tcp_chargen_server_t class

*/

/* system include */
/* local include */
#include "neoip_tcp_chargen_server.hpp"
#include "neoip_tcp_chargen_server_cnx.hpp"
#include "neoip_tcp.hpp"
#include "neoip_ipport_addr.hpp"
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
tcp_chargen_server_t::tcp_chargen_server_t()	throw()
{
	// zero some field
	tcp_resp	= NULL;
}

/** \brief Destructor
 */
tcp_chargen_server_t::~tcp_chargen_server_t()	throw()
{
	// delete all the established connection
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
	// delete the tcp_resp_t if needed
	nipmem_zdelete	tcp_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
inet_err_t	tcp_chargen_server_t::start(const tcp_resp_arg_t &resp_arg)	throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");
	// init and start tcp_resp_t
	tcp_resp	= nipmem_new tcp_resp_t();
	inet_err	= tcp_resp->start(resp_arg, this, NULL);
	if( inet_err.failed() )	return inet_err;

	// return no error
	return inet_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     tcp_resp_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_resp_t when a connection is established
 */
bool	tcp_chargen_server_t::neoip_tcp_resp_event_cb(void *userptr, tcp_resp_t &cb_tcp_resp
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( tcp_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::CNX_ESTABLISHED:{
			// spawn a tcp_chargen_server_cnx_t with the notified tcp_full_t
			tcp_full_t *	tcp_full 	= tcp_event.get_cnx_established();
			tcp_chargen_server_cnx_t *cnx	= nipmem_new tcp_chargen_server_cnx_t(this);
			inet_err_t	inet_err	= cnx->start(tcp_full);
			if( inet_err.failed() )	nipmem_delete cnx;
			break;}
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END

