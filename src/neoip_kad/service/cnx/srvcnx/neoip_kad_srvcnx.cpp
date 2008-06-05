/*! \file
    \brief Declaration of the udp_resp_t
    
*/

/* system include */
/* local include */
#include "neoip_kad_srvcnx.hpp"
#include "neoip_kad_srvcnx_cnx.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_udp.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_srvcnx_t::kad_srvcnx_t()					throw()
{
	// zero some field
	udp_vresp	= NULL;
}

/** \brief Desstructor
 */
kad_srvcnx_t::~kad_srvcnx_t()		throw()
{
	// unregister the packet type in the udp_vresp
	// - dont detruct the udp_vresp_t as it is owned by the caller
	if( udp_vresp ){
		kad_pkttype_t	pkttype(pkttype_profile);
		for(size_t i = 0; i < kad_pkttype_t::size(); i++ ){
			udp_vresp->unregister_callback(pkttype.at(i).get_value());
		}
	}
	// delete all the pending connection
	while( !cnx_list.empty() )	nipmem_delete	cnx_list.front();	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         start functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
kad_err_t kad_srvcnx_t::start(udp_vresp_t *udp_vresp, const pkttype_profile_t &pkttype_profile
				, kad_srvcnx_cb_t *callback, void *userptr)	throw()
{
	// copy some parameter
	this->udp_vresp		= udp_vresp;
	this->pkttype_profile	= pkttype_profile;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// register all the packet types in the udp_vresp_t
	kad_pkttype_t	pkttype(pkttype_profile);
	for(size_t i = 0; i < kad_pkttype_t::size(); i++ )
		udp_vresp->register_callback(pkttype.at(i).get_value(), this, NULL);

	// return no error
	return kad_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the listen outter address
 */
const ipport_addr_t &	kad_srvcnx_t::get_listen_oaddr()				const throw()
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
bool	kad_srvcnx_t::neoip_inet_udp_vresp_event_cb(void *userptr, udp_vresp_t &cb_udp_vresp
							, const udp_event_t &udp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( udp_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:{
			// spawn a cnx_t which gonna do handle this client
			udp_full_t *	udp_full= udp_event.get_cnx_established();
			cnx_t *		cnx	= nipmem_new cnx_t(this, udp_full);
			cnx->start();
			break;}
	default:	DBG_ASSERT(0);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	kad_srvcnx_t::notify_callback(pkt_t &pkt, const ipport_addr_t &local_oaddr
					, const ipport_addr_t &remote_oaddr)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_kad_srvcnx_cb(userptr, *this, pkt, local_oaddr, remote_oaddr);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END


