/*! \file
    \brief Definition of the tcp_chargen_server_cnx_t class

*/

/* system include */
/* local include */
#include "neoip_tcp_chargen_server_cnx.hpp"
#include "neoip_tcp_chargen_server.hpp"
#include "neoip_tcp.hpp"
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
tcp_chargen_server_cnx_t::tcp_chargen_server_cnx_t(tcp_chargen_server_t *chargen_server)	throw()
{
	// copy the parameter
	this->chargen_server	= chargen_server;
	// zero some field
	tcp_full		= NULL;
	
	// link it to the tcp_chargen_server_t
	chargen_server->cnx_dolink(this);
}

/** \brief Destructor
 */
tcp_chargen_server_cnx_t::~tcp_chargen_server_cnx_t()	throw()
{
	// unlink it from the tcp_chargen_server_t
	chargen_server->cnx_unlink(this);
	// delete the tcp_full_t if needed
	nipmem_zdelete	tcp_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
inet_err_t	tcp_chargen_server_cnx_t::start(tcp_full_t *tcp_full)	throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");

	// copy the parameter
	this->tcp_full	= tcp_full;
	// start the tcp_full
	inet_err	= tcp_full->start(this, NULL);	
	if( inet_err.failed() )	return inet_err;

	// dont read anything on this tcp_full
	counter		= 0;
	tcp_full->xmitbuf_maxlen( 50*1024 );
	fill_xmitbuf();	

	// return no error
	return inet_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     tcp_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_full_t when a connection is established
 */
bool	tcp_chargen_server_cnx_t::neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( tcp_event.is_full_ok() );

	// handle the fatal events
	if( tcp_event.is_fatal() ){
		nipmem_delete	this;
		return false;
	}	
	
	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::RECVED_DATA:	nipmem_delete	this;
					return false;
	case tcp_event_t::MAYSEND_ON:	fill_xmitbuf();
					break;
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         fill_xmitbuf
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief full the tcp buffer
 */
void tcp_chargen_server_cnx_t::fill_xmitbuf()			throw()
{
	inet_err_t	inet_err;
	size_t		len = 70;
	while( tcp_full->xmitbuf_freelen() >= len ){
		std::string	str;
		size_t		i;
		// build the string to send
		for( str="", i = std::string("\n").size(); i < len; i++ ){
			char	c = 'a' + (i + counter) % 26;
			str += c;
		}
		str += "\n";
		DBG_ASSERT( str.size() == len );
		// send the data to the connection
		size_t	written;
		written	= tcp_full->send( str.c_str(), len );
		// log to debug
		KLOG_DBG("write " << len << "-byte. now xmitbuf_freelen=" << tcp_full->xmitbuf_freelen() );
		// increase the counter
		counter++;
	}
	// setup the maysend limit
	tcp_full->maysend_tshold( tcp_full->xmitbuf_maxlen()/2 );
	// log to debug
	KLOG_DBG("stopped to write with xmitbuf_free_len=" << tcp_full->xmitbuf_freelen() );
	KLOG_DBG("next write bunch when maysend_threshold=" << tcp_full->maysend_tshold() );
}

NEOIP_NAMESPACE_END

