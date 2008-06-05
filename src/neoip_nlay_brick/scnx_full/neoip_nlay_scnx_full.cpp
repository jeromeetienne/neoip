/*! \file
    \brief Definition of the nlay_scnx_full_t

\par about scnx_full_t having it own packet dispatcher
scnx_full_t dont register any packet even if scnx_full_t has several of them
- as scnx_full_t encrypts the packets, the pkttype isnt in clear and 
  can't be read by nlay_full_t.
- so scnx_full_t has its one packet dispatcher
   
*/

/* system include */
/* local include */
#include "neoip_nlay_scnx_full.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_full.hpp"
#include "neoip_scnx_full.hpp"
#include "neoip_scnx_pkttype.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_scnx_full_t::nlay_scnx_full_t()	throw()
{
	// zero some field
	scnx_full	= NULL;
}

/** \brief Destructor
 */
nlay_scnx_full_t::~nlay_scnx_full_t()	throw()
{
	nipmem_zdelete	scnx_full;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                      Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the scnx_full_t pointer
 * 
 * - it is MANDATORY and it MUST be done before start()
 * - this is a special function for this brick only
 */
nlay_err_t	nlay_scnx_full_t::set_scnx_full(scnx_full_t *scnx_full)			throw()
{
	// sanity check - the scnx_full MUST not be set before
	DBG_ASSERT(!this->scnx_full);
	// copy the pointer
	this->scnx_full	= scnx_full;
	return nlay_err_t::OK;
}
/** \brief register a nlay_full_api_t
 */        
void	nlay_scnx_full_t::register_handler(nlay_regpkt_t *regpkt_lower)	throw()
{	
	// register it as a default packet handler
	regpkt_lower->set_dfl_handler(this);	
}

/** \brief Start the operation for this brick
 */
nlay_err_t	nlay_scnx_full_t::start()		throw()
{
	// sanity check - the scnx_full_t MUST be set
	DBG_ASSERT( scnx_full );
	
	// Start the action
	scnx_err_t	scnx_err = scnx_full->start(this, NULL);
	if( !scnx_err.succeed() )	return nlay_err_from_scnx(scnx_err);
	return nlay_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the space overhead due to this brick in the mtu
 */
size_t		nlay_scnx_full_t::get_mtu_overhead()				const throw()
{
	return scnx_full->get_mtu_overhead();
}

/** \brief Return the local_idname of the scnx_full_t
 * 
 * - function of nlay_scnx_full_api_t
 */
const std::string &	nlay_scnx_full_t::get_local_idname()	const throw()
{
	return scnx_full->get_local_idname();
}

/** \brief Return the remote_idname of the scnx_full_t
 * 
 * - function of nlay_scnx_full_api_t
 */
const std::string &	nlay_scnx_full_t::get_remote_idname()	const throw()
{
	return scnx_full->get_remote_idname();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           scnx event callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief receive \ref scnx_event_t from \ref scnx_full_t
 */
bool nlay_scnx_full_t::neoip_scnx_full_event_cb(void *cb_userptr, scnx_full_t &cb_scnx_full
							, const scnx_event_t &scnx_event) throw()
{
	nlay_full_t *	nlay_full	= get_nlay_full();
	nlay_event_t	nlay_event;
	pkt_t *		pkt;
	// log to debug
	KLOG_ERR("received scnx_event=" << scnx_event);

	// sanity check - check the scnx_event is allowed for a full
	DBG_ASSERT( scnx_event.is_full_ok() );

	// handle each possible events from its type
	switch( scnx_event.get_value() ){
	case scnx_event_t::PKT_TO_LOWER:
			// process the packet thru the lower layers of nlay_full_t
			pkt = scnx_event.get_pkt_to_lower();
			return nlay_full->notify_pkt_to_lower(*pkt, get_full_api_lower());	
	default:	DBG_ASSERT( 0 );
	}
	// return tokeep
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   packet processing from upper/lower
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the upper network layer
 */
nlay_err_t	nlay_scnx_full_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	scnx_err_t	scnx_err;
	// ask scnx_full_t to process the packet
	scnx_err	= scnx_full->pkt_from_upper(pkt);
	// if an error occured, report it
	if( !scnx_err.succeed() )	return nlay_err_from_scnx(scnx_err);
	// return no error
	return nlay_err_t::OK;
}

/** \brief handle packet from the lower network layer
 */
nlay_err_t	nlay_scnx_full_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	scnx_err_t	scnx_err;
	// ask scnx_full_t to process the packet
	scnx_err	= scnx_full->pkt_from_lower(pkt);
	// if an error occured, report it
	if( !scnx_err.succeed() )	return nlay_err_from_scnx(scnx_err);
	// return no error
	return nlay_err_t::OK;
}



NEOIP_NAMESPACE_END


