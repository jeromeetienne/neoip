/*! \file
    \brief Implementation of the neoip_tcp_chargen

\par TODO
- port this on top of non reliable socket_type
  - put a 0 timer which keep sending packet
  - how large ?

*/

/* system include */
/* local include */
#include "neoip_tcp_chargen_server.hpp"
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
tcp_chargen_server_t::tcp_chargen_server_t()					throw()
{
	inet_err_t	inet_err;
	// start the tcp_resp_t
	inet_err	= tcp_resp.start("127.0.0.1:3007", this, NULL);
	if( inet_err.failed() )	KLOG_ERR("Cant start tcp_resp due to " << inet_err );
	DBG_ASSERT( inet_err.succeed() );
}

/** \brief destructor of neoip_tcp_accept
 */
tcp_chargen_server_t::~tcp_chargen_server_t()					throw()
{
	// delete all the pending connection
	while( tcp_full_list.empty() == false ){
		tcp_full_t	*tcp_full = tcp_full_list.front();
		// remove the tcp socket from the list
		tcp_full_list.remove( tcp_full );
		// delete the tcp_full
		nipmem_delete tcp_full;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            TCP_RESP callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_resp_t when a connection is established
 */
bool tcp_chargen_server_t::neoip_tcp_resp_event_cb(void *cb_userptr, tcp_resp_t &cb_tcp_resp
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
	// init the counter
	counter = 0;
	// add the tcp_full to the list
	tcp_full_list.push_front( tcp_full );
	// set up the tcp_full
	tcp_full->set_callback( this, NULL );
	tcp_full->start();
	// dont read anything on this tcp_full
	tcp_full->recv_max_len_set( 0 );
	tcp_full->sendbuf_set_max_len( 10*1024 );
	fill_sendbuf(tcp_full);
	return true;
}

/** \brief full the socket buffer
 */
void tcp_chargen_server_t::fill_sendbuf( tcp_full_t *tcp_full )			throw()
{
	ssize_t		len = 70;
	while( tcp_full->sendbuf_get_free_len() >= len ){
		std::string	str;
		int		i;
		// build the string to send
		for( str="", i = std::string("\n").size(); i < len; i++ ){
			char	c = 'a' + (i + counter) % 26;
			str += c;
		}
		str += "\n";
		DBG_ASSERT( (ssize_t)str.size() == len );
		// send the data to the connection
		ssize_t	sendlen = tcp_full->send( str.c_str(), len );
		DBG_ASSERT( sendlen == len );
		// increase the counter
		counter++;
	}
	// setup the maysend limit
	tcp_full->maysend_set_threshold( tcp_full->sendbuf_get_max_len()/2 );
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            TCP_FULL callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback used by neoip_socket to report an error
 */
bool tcp_chargen_server_t::neoip_tcp_full_event_cb(void *cb_userptr, tcp_full_t &cb_tcp_full
							, const tcp_event_t &tcp_event )
							throw()
{
	tcp_full_t	*tcp_full = &cb_tcp_full;
	KLOG_DBG("enter tcp_event=" << tcp_event);
	// if the event is MAYSEND_ON, fill the queue
	if( tcp_event.is_maysend_on() )	fill_sendbuf( tcp_full );

	if( tcp_event.is_fatal() ){
		KLOG_ERR("enter tcp_event=" << tcp_event);
		// remove the tcp socket from the list
		tcp_full_list.remove( tcp_full );
		// delete the tcp_full
		nipmem_delete tcp_full;
		return false;
	}
	return true;
}

NEOIP_NAMESPACE_END

