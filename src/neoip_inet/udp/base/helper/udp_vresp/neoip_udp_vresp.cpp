/*! \file
    \brief Definition of the udp_vresp_t

\par TODO
- TODO there is some issues with the cnx_t and the udp_full->start()
  - the cnx_t start the udp_full and then report it
  - the callback to which it is reported it supposed to do another start()
    to comply to the usual udp_full_t api
    - but if it does, the udp_full is started twice !!!
    - this is a problem
  => WORKAROUND: dont start the udp_full in the last callback
     - unclean
     - unknown issue about packet received meanwhile

*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_udp_vresp.hpp"
#include "neoip_udp_vresp_cnx.hpp"
#include "neoip_udp_resp.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
udp_vresp_t::udp_vresp_t()				throw()
{
	// zero some field
	udp_resp	= NULL;	
}

/** \brief destructor
 */
udp_vresp_t::~udp_vresp_t()				throw()
{
	// close all the cnx_t attached to this udp_vresp_t
	while( !cnx_db.empty() )	nipmem_delete	cnx_db.front();
	// close the udp_resp if needed
	nipmem_zdelete	udp_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       get/set parameter
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief set the listen address
 */
inet_err_t	udp_vresp_t::set_listen_addr(const ipport_addr_t &listen_addr)	throw()
{
	inet_err_t	inet_err;
	// sanity check - the udp_resp_t MUST NOT be already init
	DBG_ASSERT( udp_resp == NULL );
	// create the udp_resp_t	
	udp_resp	= nipmem_new udp_resp_t();
	// set the listen address
	inet_err = udp_resp->set_listen_addr(listen_addr);
	if( inet_err.failed() )		goto close_udp_resp;
	// set the callback
	inet_err = udp_resp->set_callback(this, NULL);
	if( inet_err.failed() )		goto close_udp_resp;
	// start the udp_resp_t
	inet_err = udp_resp->start();
	if( inet_err.failed() )		goto close_udp_resp;
	// return no error
	return inet_err_t::OK;	
	
close_udp_resp:	// log the event
		KLOG_ERR("Can't init udp_vresp_t to " << listen_addr << " due to " << inet_err );
		// close the udp_resp itself
		DBG_ASSERT( udp_resp );
		nipmem_zdelete	udp_resp;
		// forward the error
		return inet_err;	
}

/** \brief return the listen_addr
 */
const ipport_addr_t &	udp_vresp_t::listen_addr()		const throw()
{
	// sanity check - the object MUST NOT be null
	DBG_ASSERT( !is_null() );
	// return the listen address
	return udp_resp->get_listen_addr();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       start
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief start the action
 */
inet_err_t	udp_vresp_t::start()						throw()
{
	// NOTE: this function doesn't do anything except to check that the 
	// udp_resp is properly initialized
	
	// sanity check - udp_resp MUST BE already init (so the caller MUST have
	// already called udp_vresp_t::set_listen_addr()
	DBG_ASSERT( udp_resp != NULL );
	
	// return no error
	return inet_err_t::OK;
}

/** \brief Set the mandatory parameters and start the actions
 * 
 * - This is only a helper function on top of the actual functions
 */
inet_err_t udp_vresp_t::start(const ipport_addr_t &listen_addr)			throw()
{
	inet_err_t	inet_err;
	// set the remote address
	inet_err = set_listen_addr(listen_addr);
	if( inet_err.failed() )	return inet_err;
	// start the action
	return start();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     callback register/unregister
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief register a callback for a given pkttype
 * 
 * - the pkttype MUST NOT be already registered when this function is called
 */
void	udp_vresp_t::register_callback(uint8_t pkttype, udp_vresp_cb_t *callback, void *userptr) throw()
{
	// log to debug
	KLOG_DBG("register the pkttype=" << int(pkttype));
	// sanity check - the pkttype MUST NOT already be registered
	DBG_ASSERT( reg_cb_db.find(pkttype) == reg_cb_db.end() );
	// create the callback
	reg_cb_t	reg_cb(pkttype, callback, userptr);
	// insert the callback into the db
	bool	succeed = reg_cb_db.insert(std::make_pair(pkttype, reg_cb)).second;
	DBG_ASSERT( succeed );
}

/** \brief unregister a callback for a given pkttype
 * 
 * - the pkttype MUST be register when this function is called
 */
void	udp_vresp_t::unregister_callback(uint8_t pkttype)			throw()
{
	// sanity check - the pkttype MUST be registered
	DBG_ASSERT( reg_cb_db.find(pkttype) != reg_cb_db.end() );
	// remove the element itself
	reg_cb_db.erase(pkttype);
}

/** \brief Return the reg_cb_t matching the pkttype, or a null reg_cb_t if none matches
 */
udp_vresp_t::reg_cb_t	udp_vresp_t::reg_cb_get(uint8_t pkttype)	const throw()
{
	// try to find the matching pkttype in the reg_cb_db
	std::map<uint8_t, reg_cb_t>::const_iterator	iter	= reg_cb_db.find(pkttype);
	// if not found, return a null reg_cb_t
	if( iter == reg_cb_db.end() )	return reg_cb_t();
	// NOTE: here the reg_cb_t has been found
	return iter->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     reponder callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_resp_t when a connection is established
 */
bool	udp_vresp_t::neoip_inet_udp_resp_event_cb(void *userptr, udp_resp_t &cb_udp_resp
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
			nipmem_new	udp_vresp_cnx_t(this, udp_full);
			break;}
	default:	DBG_ASSERT( 0 );	
	}
	// return a 'tokeep'
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       to_string() function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	udp_vresp_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return udp_resp->to_string();
}

NEOIP_NAMESPACE_END



