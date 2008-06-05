/*! \file
    \brief Class to handle the http_listener_cnx_t

*/

/* system include */
/* local include */
#include "neoip_http_listener_cnx.hpp"
#include "neoip_http_listener.hpp"
#include "neoip_http_resp.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
http_listener_cnx_t::http_listener_cnx_t()		throw()
{
	// zero some fields
	http_listener	= NULL;
	socket_full	= NULL;
}

/** \brief Destructor
 */
http_listener_cnx_t::~http_listener_cnx_t()		throw()
{
	// delete the socket_full_t if needed
	nipmem_zdelete	socket_full;		
	// unlink this object from the http_listener
	if( http_listener )	http_listener->cnx_unlink(this);
}

/** \brief Autodelete the object and return false to ease readability
 */
bool	http_listener_cnx_t::autodelete(const std::string &reason)		throw()
{
	// if reason is specified, log it
	if( !reason.empty() )	KLOG_ERR("autodelete due to " << reason);
	// autodelete the object itself
	nipmem_delete	this;
	// return dontkeep for convenience
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation
 */
http_err_t	http_listener_cnx_t::start(http_listener_t *http_listener, socket_full_t *socket_full)	throw()
{
	const http_listener_profile_t &	profile	= http_listener->get_profile();
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->http_listener	= http_listener;
	this->socket_full	= socket_full;
	// link this object to the http_listener
	http_listener->cnx_dolink(this);
	// start the socket_full
	socket_err_t	socket_err;
	socket_err	= socket_full->start(this, NULL);	
	if( socket_err.failed() )	return http_err_from_socket(socket_err);
	// start the expire_timeout
	expire_timeout.start(profile.cnx_timeout_delay(), this, NULL);
	// return no error
	return http_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       expire_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	http_listener_cnx_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	return autodelete();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t when a connection is established
 */
bool	http_listener_cnx_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() )	return autodelete();
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:
			// handle the received packet
			return handle_recved_data(*socket_event.get_recved_data());
	case socket_event_t::MAYSEND_ON:
			// the only data sent by http_listener_cnx_t is the error http_rephd_t
			// - if MAYSEND_ON is received, the whole header have been sent
			// - so autodelete
			return autodelete();
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}


/** \brief Handle received data on the cnx_t
 * 
 * @return a 'tokeep/dontkeep' for the socket_full_t
 */
bool	http_listener_cnx_t::handle_recved_data(pkt_t &pkt)	throw()
{
	const http_listener_profile_t &	profile	= http_listener->get_profile();	
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);
	// queue the received data to the one already received
	recved_data.append(pkt.to_datum(datum_t::NOCOPY));

	// log to debug
	KLOG_DBG("recved_data=" << recved_data.to_datum(datum_t::NOCOPY).to_stdstring());
	
	// if the request header is not yet fully received, return true - rfc2616.4.1
	std::string	recved_str	= recved_data.to_datum(datum_t::NOCOPY).to_stdstring();
	size_t		end_of_header	= recved_str.find("\r\n\r\n");
	if( end_of_header == std::string::npos ){
		// if the recved_data is larger than the maximum, autodelete this connection
		// - it is a DOS protection to avoid large memory consumption by attacker
		//   sending malformed requests.
		if( recved_data.size() > profile.cnx_reqhd_maxlen() )	return autodelete();
		// else just return tokeep and wait to receive more data
		return true;
	}
	// update the end_of_header to include the end marker "\r\n\r\n"
	end_of_header	+= std::string("\r\n\r\n").size();

	// extract the header part - +2 to add a \r\n for the last line of the header
	std::string	header_str	= recved_str.substr(0, end_of_header);
	// convert it to its internal format
	http_reqhd_t	http_reqhd	= http_reqhd_t::from_http(header_str);
	// if the parsing of the http_reqhd_t fails, autodelete the connection
	if( http_reqhd.is_null() )	return autodelete();
	// consume the header in the recved_data
	recved_data.head_consume(end_of_header);

	// if the http_reqhd_uri.is_scrambled, unscramble it
	if( http_reqhd.uri().is_scrambled() )	http_reqhd.uri() = http_reqhd.uri().unscramble(); 

	// log to debug
	KLOG_DBG("http_reqhd=" << http_reqhd);
	
	// find the http_resp_t matching the received http_reqhd_t
	http_resp_t *	http_resp	= http_listener->find_resp(http_reqhd);
	// if no http_resp_t matches, send back a 404 Not Found
	if( http_resp == NULL ){
		http_rephd_t	http_rephd;
		// build the http_rephd_t to reply - a 404 as in rfc2616.10.4.5
		http_rephd.version(http_version_t::V1_1).status_code(404).reason_phrase("Not Found");
		// TODO apparently the replied header is not compliant or there is another thing wrong
		// - because no browser display a not found
		// send the data of the http_rephd_t
		socket_full->send( datum_t(http_rephd.to_http()) );
		// log to debug
		KLOG_ERR("http_rephd=" << datum_t(http_rephd.to_http()));
		// init the socket_full_t to trigger a MAYSEND_ON when the sendbuf is empty
		socket_full->sendbuf_set_max_len(socket_full->sendbuf_get_used_len());
		socket_full->maysend_set_threshold(socket_full->sendbuf_get_max_len());
		// return tokeep
		return true;
	}
	
	// backup the object_slotid of the socket_full_t - to be able to return its tokeep value
	slot_id_t	socket_full_slotid	= socket_full->get_object_slotid();
	// steal the socket_full_t - not to delete it when http_listener_cnx_t gonna be destroyed
	socket_full_t *	stolen_socket_full	= socket_full;
	socket_full	= NULL;

	// notify the new socket_full_t to the matching http_resp_t
	http_resp->notify_new_cnx(http_reqhd, stolen_socket_full, recved_data);

	// http_listener_cnx_t is no more usefull, autodelete
	nipmem_delete	this;

	// if the socket_full_t has no been deleted, so 'tokeep' else return 'dontkeep'
	return object_slotid_tokeep(socket_full_slotid);
}

NEOIP_NAMESPACE_END





