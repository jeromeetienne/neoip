/*! \file
    \brief Definition of the \ref dnsgrab_t class

\par Possible Improvement
- to port on top of socket_t once it works well
- to handle the maysend_on/off in transmit
  - this is the weird linger issue right there

*/

/* system include */
/* local include */
#include "neoip_dnsgrab_cnx.hpp"
#include "neoip_tcp_full.hpp"
#include "neoip_string.hpp"
#include "neoip_pkt.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
dnsgrab_cnx_t::dnsgrab_cnx_t(dnsgrab_t *dnsgrab, tcp_full_t *tcp_full)
										throw()
{
	// copy parameter
	this->dnsgrab	= dnsgrab;
	this->tcp_full	= tcp_full;
	// allocate a slot_id for this connection
	slot_id		= dnsgrab->cnx_slotpool.allocate(this);	
	// start the tcp_full
	inet_err_t	inet_err;
	inet_err	= tcp_full->start(this, NULL);
	DBG_ASSERT( inet_err.succeed() );		// TODO poor error management
	// log to debug
	KLOG_DBG("create a dnsgrab_cnx_t. slot_id=" << slot_id
						<< " local=" << tcp_full->get_local_addr()
						<< " remote=" << tcp_full->get_remote_addr() );	
	// link the connection to the cnx_db
	dnsgrab->cnx_link( this );
}

/** \brief Desstructor
 */
dnsgrab_cnx_t::~dnsgrab_cnx_t()			throw()
{
	// log to debug
	KLOG_DBG("Destroy a dnsgrab_cnx_t slot_id=" << slot_id);
	// release the slot_id of this connection
	dnsgrab->cnx_slotpool.release(slot_id);
	// close the full connection
	nipmem_delete	tcp_full;
	// unlink the connection to the cnx_db
	dnsgrab->cnx_unlink( this );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   tcp full callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_full_t when to notify an event
 */
bool	dnsgrab_cnx_t::neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( tcp_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::CNX_CLOSED:	// autodelete
					nipmem_delete	this;
					return false;
	case tcp_event_t::RECVED_DATA:{	pkt_t *	pkt	= tcp_event.get_recved_data();
					return handle_recved_data(*pkt);}
	case tcp_event_t::MAYSEND_ON:	// NOTE: used only as a lame linger
					// - maysend is configure to be trigger iif the reply is 
					//   fully in kernel
					nipmem_delete	this;
					return false;
	default:	DBG_ASSERT(0);
	}	
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         handle_recved_data
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handler RECVED_DATA from the tcp_full_t
 */
bool	dnsgrab_cnx_t::handle_recved_data(const pkt_t &pkt)	throw()
{
	dnsgrab_request_t	request;
	bool			failed, tokeep;

	// queue the received data to the one already received
	recved_data << string_t::from_datum(pkt.to_datum(datum_t::NOCOPY));
	// if the pending_data are too long, close the connection
	if( recved_data.str().length() > 5*1024 )	goto error;

	// log to debug
	KLOG_DBG("recved data " << recved_data.str().length() << "-byte");
	KLOG_DBG("recved data " << recved_data.str() << "-byte");
	// if all the received data forms a request, process it
	if( contain_request(recved_data) )		return true;
	// parse the pending queue and try to build a request
	failed	= parse_request(recved_data.str(), request);
	if( failed )					goto error;

	// notify the caller
	tokeep	= dnsgrab->notify_callback(request);
	// if the dnsgrab_t (and NOT dnsgrab_cnx_t) has been deleted, return false
	if( !tokeep )	goto error;

	// if the callback provided the answer immediatly, send it back thru the connection
	// - if the reply_present is false, the connection is kept and the callback will notify
	//   the answer later
	if( request.get_reply_present() )	notify_reply(request);
	
	// if the request will be answered later, keep the connection
	return true;
error:;
	nipmem_delete	this;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         utility function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if a request has been received (aka a single line terminated by a \n)
 */
bool dnsgrab_cnx_t::contain_request(const std::stringstream &ss) throw()
{
	// if the string contains the '\r', return true
	if( ss.str().rfind("\r") != std::string::npos )	return true;
	// else return false
	return false;
}


/** \brief parse a input stream into a dnsgrab_request_t
 * 
 * @return false if no error occurs, and true otherwise
 */
bool dnsgrab_cnx_t::parse_request(const std::string &request_str, dnsgrab_request_t &request_out)
										throw()
{
	// parse the request
	request_out = dnsgrab_request_t(slot_id, request_str);
	// if the parsing failed, return an error
	if( request_out.is_null() )	return true;
	// else return no error
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         notify_reply
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief used by the caller to notify the reply of a previously delivered request
 */
void	dnsgrab_cnx_t::notify_reply(const dnsgrab_request_t &dnsgrab_request)		throw()
{
	// log to debug
	KLOG_DBG("notication of reply for slot_id=" << slot_id);
	// sanity check - the dnsgrab_request slot_id MUST match the local one
	DBG_ASSERT( dnsgrab_request.get_cnx_slot_id() == get_slot_id() );
	// build the reply string
	std::string reply = dnsgrab_request.get_reply_str();
	
	// log to debug
	KLOG_DBG("Notifying the reply_str=["<< reply << "] " << reply.size() << "-byte");
	
	// send the built reply thru the socket
	tcp_full->send( reply.c_str(), reply.size() );
	 
#if 1
	// ask to be notified when the sendbuf is empty
	// - lame way to do a linger
	inet_err_t	inet_err;
	inet_err = tcp_full->sendbuf_set_max_len(tcp_full->sendbuf_get_used_len());
	DBG_ASSERT( inet_err.succeed() );
	inet_err = tcp_full->maysend_set_threshold(tcp_full->sendbuf_get_max_len());
	DBG_ASSERT( inet_err.succeed() );
#else
	// delete the connection
	nipmem_delete	this;
	// TODO here the connection MUST be closed
	// - but the delete will not let the time to actually transmit the data
	//   - it comes directly from the tcp_full to have its own window
	// - here the choise is to handle a linger in the neoip_socket_full
#endif
	 
}
NEOIP_NAMESPACE_END









