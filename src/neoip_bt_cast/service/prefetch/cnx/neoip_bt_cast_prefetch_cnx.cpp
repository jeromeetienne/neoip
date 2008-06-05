/*! \file
    \brief Class to handle the bt_cast_prefetch_cnx_t

*/

/* system include */
/* local include */
#include "neoip_bt_cast_prefetch_cnx.hpp"
#include "neoip_bt_cast_prefetch.hpp"
#include "neoip_http_resp.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_http_client.hpp"
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
bt_cast_prefetch_cnx_t::bt_cast_prefetch_cnx_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	m_cast_prefetch	= NULL;
	m_http_client	= NULL;
	m_socket_full	= NULL;
}

/** \brief Destructor
 */
bt_cast_prefetch_cnx_t::~bt_cast_prefetch_cnx_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the http_client_t if needed
	nipmem_zdelete	m_http_client;		
	// delete the socket_full_t if needed
	nipmem_zdelete	m_socket_full;		
	// unlink this object from the bt_cast_prefetch
	if( m_cast_prefetch )	m_cast_prefetch->cnx_unlink(this);
}

/** \brief Autodelete the object and return false to ease readability
 */
bool	bt_cast_prefetch_cnx_t::autodelete(const std::string &reason)		throw()
{
	// if a reason is given, log it
	if( !reason.empty() )	KLOG_ERR("reason=" << reason); 
	// delete the object
	nipmem_delete	this;
	// return dontkeep
	return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation
 */
bt_err_t	bt_cast_prefetch_cnx_t::start(bt_cast_prefetch_t *m_cast_prefetch
						, const http_uri_t &m_http_uri)	throw()
{
	// log to debug
	KLOG_DBG("enter http_uri=" << m_http_uri);
	// copy the parameter
	this->m_cast_prefetch	= m_cast_prefetch;
	this->m_http_uri	= m_http_uri;
	// link this object to the bt_cast_prefetch
	m_cast_prefetch->cnx_dolink(this);


	// TODO do a LOT of sanity check on the http_uri 
	// - this may be a big source of issue
	// - like ensure this apps is the actual destination
	// - like ensure it wont receive data aka presence of httpo_maxrate uri variable
	//   - issue neoip-oload nested uri has it as outter_uri var and 
	//     neoip-casto uri got it as plain variable 
	
	// create the http_reqhd_t to use with the http_sclient_t
	http_reqhd_t	http_reqhd	= http_reqhd_t().uri(m_http_uri);
	
	// start the http_client_t
	http_err_t	http_err;
	m_http_client	= nipmem_new http_client_t();
	http_err	= m_http_client->start(http_reqhd, this, NULL);
	if( http_err.failed() )	return bt_err_from_http(http_err);

	
	// return no error
	return bt_err_t::OK;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    http_client_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t when to notify an event
 */
bool	bt_cast_prefetch_cnx_t::neoip_http_client_cb(void *cb_userptr, http_client_t &cb_http_client
				, const http_err_t &http_err, const http_rephd_t &http_rephd
				, socket_full_t *socket_full, const bytearray_t &recved_data) throw()
{
	// log to debug
	KLOG_DBG("enter http_err=" << http_err	<< " http_rephd=" << http_rephd
						<< " recved_data=" << recved_data.length());

	// copy the socket_full as it is now owned by this object
	this->m_socket_full	= socket_full;	

	// if the connection failed, notify the caller
	if( http_err.failed() )		return autodelete(http_err.to_string());
	
	// delete the http_client_t
	nipmem_zdelete	m_http_client;

	// setup the socket_full_t - it is already started by http_client_t
	m_socket_full->set_callback(this, NULL);


	// return dontkeep
	return false;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t when a connection is established
 */
bool	bt_cast_prefetch_cnx_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() )	return autodelete();
	
	// NOTE: any other event is plain ignored
	// - this will discard any incoming data (i.e. the http reply header)
	// - TODO may be good to do sanity check on the amount of data transfered
	//   - or about the time the connection stay alive

	// return tokeep
	return true;
}



NEOIP_NAMESPACE_END





