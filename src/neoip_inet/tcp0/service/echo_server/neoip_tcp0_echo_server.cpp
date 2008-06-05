/*! \file
    \brief Implementation of the neoip_tcp_echo

- TODO implement the maysend on the echo 

*/

/* system include */
/* local include */
#include "neoip_tcp_echo_server.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor of neoip_tcp_accept
 */
tcp_echo_server_t::tcp_echo_server_t()					throw()
{
	inet_err_t	inet_err;
	// start the tcp_resp_t
	inet_err	= tcp_resp.start("127.0.0.1:3007", this, NULL);
	if( inet_err.failed() )	KLOG_ERR("Cant start tcp_resp due to " << inet_err );
	DBG_ASSERT( inet_err.succeed() );	
}

/** \brief destructor of neoip_tcp_accept
 */
tcp_echo_server_t::~tcp_echo_server_t()				throw()
{
	// delete the whole database
	while( tcp_full_list.empty() == false ){
		tcp_full_t	*tcp_full = tcp_full_list.front();
		// delete the tcp_full
		nipmem_delete tcp_full;
		// remove the tcp socket from the list
		tcp_full_list.remove( tcp_full );
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            TCP_RESP callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref neoip_tcp_resp_t when a connection is established
 */
bool tcp_echo_server_t::neoip_tcp_resp_event_cb(void *cb_userptr, tcp_resp_t &cb_tcp_resp
							, const tcp_event_t &tcp_event )
							throw()
{
	// sanity check - neoip_tcp_resp_event_cb MUST only report CNX_ESTABLISHED
	DBG_ASSERT( tcp_event.is_cnx_established() );
	// get the established connection
	tcp_full_t	*tcp_full = tcp_event.get_cnx_established();
	// some logging
	KLOG_ERR("received a tcp full connection = " << *tcp_full << ". add it to the "
				<< tcp_full_list.size() << " others" );
	// add the tcp_full to the list
	tcp_full_list.push_front( tcp_full );
	// set up the tcp_full
	tcp_full->set_callback( this, NULL );
	tcp_full->start();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            TCP_FULL callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback used by neoip_socket to report an error
 */
bool tcp_echo_server_t::neoip_tcp_full_event_cb(void *cb_userptr, tcp_full_t &cb_tcp_full
						, const tcp_event_t &tcp_event ) throw()
{
	tcp_full_t	*tcp_full = &cb_tcp_full;
	KLOG_ERR("enter tcp_event=" << tcp_event);
	
	if( tcp_event.is_recved_data() ){
		pkt_t	*pkt = tcp_event.get_recved_data();
		ssize_t	sendlen = cb_tcp_full.send( pkt->get_data(), pkt->get_len() );
		DBG_ASSERT( sendlen == (ssize_t)pkt->get_len() );
		return true;	
	}
	
	if( tcp_event.is_fatal() ){
		// remove the tcp socket from the list
		tcp_full_list.remove( tcp_full );
		// delete the tcp_full
		nipmem_delete tcp_full;
	}
	return false;
}

NEOIP_NAMESPACE_END

