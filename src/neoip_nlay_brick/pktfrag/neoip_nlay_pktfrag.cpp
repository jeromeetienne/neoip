/*! \file
    \brief Definition of the nlay_pktfrag_t

\par Implementation Notes
- the pkt_from_upper() fragments the packet and send it immediatly via
  nlay_full_t::notify_pkt_to_lower()
  - the packet orders is unchanged
  - the fragments are sent immediatly so if a buffering layer is lower (e.g. rdgram), it
    will receive and update its buffer immediatly

*/

/* system include */
/* local include */
#include "neoip_nlay_pktfrag.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_full.hpp"
#include "neoip_pktfrag_pkttype.hpp"
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
nlay_pktfrag_t::nlay_pktfrag_t()	throw()
{
}

/** \brief Destructor
 */
nlay_pktfrag_t::~nlay_pktfrag_t()	throw()
{
}

/** \brief set the outter mtu of this brick
 */
nlay_err_t	nlay_pktfrag_t::set_outter_mtu(size_t outter_mtu)		throw()
{
	// logging for debug
	KLOG_DBG("outter mtu = " << outter_mtu);
	// forward the new mtu to pktfrag
	pktfrag_err_t	pktfrag_err = pktfrag.set_outter_mtu(outter_mtu);
	if( pktfrag_err.failed() )	return nlay_err_from_pktfrag(pktfrag_err);
	return nlay_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                      function inherited from nlay_full_api_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief register a nlay_full_api_t
 */
void	nlay_pktfrag_t::register_handler(nlay_regpkt_t *regpkt_lower)	throw()
{	
	pktfrag_pkttype_t	pkttype;
	// register a packet type
	pkttype	= pktfrag_pkttype_t::PKT_FRAGMENT;
	regpkt_lower->register_handler(pkttype.get_value(), this);	
}

/** \brief Start the operation for this brick
 */
nlay_err_t	nlay_pktfrag_t::set_parameter(const nlay_profile_t *nlay_profile
					, const nlay_nego_result_t *nego_result)	throw()
{
	pktfrag_err_t	err = pktfrag.set_from_profile(nlay_profile->pktfrag());
	if( err.failed() )	return nlay_err_from_pktfrag(err);
	return nlay_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   packet processing from upper/lower
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the upper network layer
 */
nlay_err_t	nlay_pktfrag_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	nlay_full_t *		nlay_full	= get_nlay_full();
	pktfrag_fraglist_t	fraglist;
	size_t			i;
	
	// if the packet is already <= to the outter mtu, do nothing
	if( pkt.get_len() <= pktfrag.get_outter_mtu() )	return nlay_err_t::OK;

	// else fragment it
	fraglist = pktfrag.pkt_from_upper(pkt);
	// sanity check - it MUST result in at least 2 fragments
	DBG_ASSERT( fraglist.size() > 1 );
	
	// swallow the packet (to stop this pkt_from_upper)
	pkt = pkt_t();

	// send all the fragment immediatly thru notify_pkt_to_lower()
	for( i = 0; i < fraglist.size(); i++ ){
		// send the fragment
		bool tokeep = nlay_full->notify_pkt_to_lower(fraglist[i], get_full_api_lower());
		if( !tokeep )	return nlay_err_t::BOGUS_PKT;	// TODO what error to return
	}

	return nlay_err_t::OK;
}

/** \brief handle packet from the lower network layer
 * 
 * - this function MUST NEVER be called, it is just a stub for nlay_full_api_t
 */
nlay_err_t	nlay_pktfrag_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	pktfrag_err_t	pktfrag_err = pktfrag.pkt_from_lower(pkt);
	if( !pktfrag_err.succeed() )	return nlay_err_from_pktfrag(pktfrag_err);
	return nlay_err_t::OK;
}



NEOIP_NAMESPACE_END


