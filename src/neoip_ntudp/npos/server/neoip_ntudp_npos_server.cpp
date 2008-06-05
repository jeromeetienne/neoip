/*! \file
    \brief Definition of the ntudp_npos_server_t
    
*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_ntudp_npos_server.hpp"
#include "neoip_ntudp_npos_server_cnx.hpp"
#include "neoip_ntudp_npos_pkttype.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_udp.hpp"
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
ntudp_npos_server_t::ntudp_npos_server_t()				throw()
{
	// zero some field
	udp_vresp	= NULL;
}

/** \brief destructor
 */
ntudp_npos_server_t::~ntudp_npos_server_t()				throw()
{
	// if the udp_vresp has been setup, unregister the callbacks
	if( udp_vresp ){
		for(size_t i = 0; i < ntudp_npos_pkttype_t::size(); i++ ){
			udp_vresp->unregister_callback(ntudp_npos_pkttype_t::at(i).get_value());
		}
	}
	// close all the cnx_t attached to this udp_vresp_t
	while( !cnx_list.empty() )	nipmem_delete	cnx_list.front();	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     start function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 * 
 * @return true on error, false otherwise
 */
ntudp_err_t	ntudp_npos_server_t::start(udp_vresp_t *udp_vresp)	throw()
{
	// copy the udp_vresp pointer
	this->udp_vresp	= udp_vresp;
	// register all the packet types in the udp_vresp_t
	for(size_t i = 0; i < ntudp_npos_pkttype_t::size(); i++ )
		udp_vresp->register_callback(ntudp_npos_pkttype_t::at(i).get_value(), this, NULL);
	// return no error
	return ntudp_err_t::OK;
}

/** \brief return the local listen address of this ntudp_npos_server_t
 */
ipport_addr_t	ntudp_npos_server_t::listen_addr()	const throw()
{
	return udp_vresp->get_listen_addr();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     reponder callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_resp_t when a connection is established
 */
bool	ntudp_npos_server_t::neoip_inet_udp_vresp_event_cb(void *userptr, udp_vresp_t &cb_udp_vresp
							, const udp_event_t &udp_event)	throw()
{
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( udp_event.is_resp_ok() );	
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:{
			// if an established connection is notified by the udp_resp_t, create a cnx_t to handle it
			udp_full_t *	udp_full = udp_event.get_cnx_established();
			// create the connection - it links itself to 'this' udp_vresp_t
			nipmem_new	cnx_t(this, udp_full);
			break;}
	default:	DBG_ASSERT( 0 );	
	}
	// return a 'tokeep'
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    inetreach_probe callback storage
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief register a callback for a given ntudp_nonce_t
 * 
 */
void	ntudp_npos_server_t::inetreach_probe_register(ntudp_nonce_t &nonce, ntudp_npos_server_probe_cb_t *callback
							, void * userptr)	throw()
{
	// sanity check - the nonce MUST NOT be null
	DBG_ASSERT( nonce.is_null() == false );
	// sanity check - the nonce MUST NOT already be registered
	DBG_ASSERT( probe_db.find(nonce) == probe_db.end() );
	// create the probe_cb_t
	probe_cb_t	probe_cb(nonce, callback, userptr);
	// insert the callback into the db
	bool	succeed = probe_db.insert(std::make_pair(nonce, probe_cb)).second;
	DBG_ASSERT( succeed );
}

/** \brief unregister a callback for a given nonce
 * 
 */
void	ntudp_npos_server_t::inetreach_probe_unregister(ntudp_nonce_t &nonce)	throw()
{
	// sanity check - the nonce MUST be registered
	DBG_ASSERT( probe_db.find(nonce) != probe_db.end() );
	// remove the element itself
	probe_db.erase(nonce);
}

/** \brief Return the reg_cb_t matching the pkttype, or a null reg_cb_t if none matches
 */
ntudp_npos_server_t::probe_cb_t	ntudp_npos_server_t::inetreach_probe_find(ntudp_nonce_t &nonce)	const throw()
{
	// try to find the matching pkttype in the reg_cb_db
	std::map<ntudp_nonce_t, probe_cb_t>::const_iterator	iter = probe_db.find(nonce);
	// if not found, return a null probe_cb_t
	if( iter == probe_db.end() )	return probe_cb_t();
	// NOTE: here the reg_cb_t has been found
	return iter->second;
}


NEOIP_NAMESPACE_END



