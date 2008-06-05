/*! \file
    \brief Definition of the nlay_bstream2dgram_t

- TODO handle the inner mtu stuff here

\par Implementation Notes
- ABOUT the 'infinite received queue'
  - when a packet is received in pkt_from_lower() it is queued in nlay_bstream2dgram_t::recved_data
    which acts as a infinite received queue.
  - this queue is required because the caller may change the inner_mtu in the callback of the
    pkt_from_lower().
    - if the inner_mtu is different but non zero, pkt_from_lower() will goes on delivering packet
    - if the inner_mtu is now zero, pkt_from_lower will stop delivering packets and keep them queued
      to deliver them as soon as possible (aka inner_mtu > 0)
  - The facts that this queue is infinite in length isn't a trouble in practice because:
    - it is flushed as soon as the caller accepts data (aka inner_mtu > 0) and it is
      flushed as fast as possible.
      - when packet are received
      - when the inner_mtu is changed to a value > 0
    - moreover in the socket layer, if the inner_mtu is set to 0, the otsp connection stops
      read data from the network independtly of the length.
      - so the nlay layer will not receive additionnal packet
    => the only data queued are the ones which remained to deliver while the caller set the 
       inner mtu to 0.
       - so something close to the otsp mtu which is generaly quite small (around 1.5-kbyte or 4-kbyte)

*/

/* system include */
/* local include */
#include "neoip_nlay_bstream2dgram.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_full.hpp"
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
nlay_bstream2dgram_t::nlay_bstream2dgram_t()	throw()
{
}

/** \brief Destructor
 */
nlay_bstream2dgram_t::~nlay_bstream2dgram_t()	throw()
{
}

/** \brief set the outter mtu of this brick
 */
nlay_err_t	nlay_bstream2dgram_t::set_outter_mtu(size_t outter_mtu)		throw()
{
	// logging for debug
	KLOG_DBG("outter mtu = " << outter_mtu);
	// copy the value
	this->outter_mtu = outter_mtu;
	return nlay_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                      function inherited from nlay_full_api_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief register a nlay_full_api_t
 */
void	nlay_bstream2dgram_t::register_handler(nlay_regpkt_t *regpkt_lower)	throw()
{	
	// register it as a default packet handler
	regpkt_lower->set_dfl_handler(this);	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   packet processing from upper/lower
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the upper network layer
 */
nlay_err_t	nlay_bstream2dgram_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	nlay_full_t *	nlay_full	= get_nlay_full();
	
	// if the packet is already <= to the outter mtu, do nothing
	if(pkt.get_len() <= outter_mtu)		return nlay_err_t::OK;

	// fragment the whole packet in fragment of at most outter_mtu byte
	while( pkt.get_len() ){
		pkt_t	fragment;
		size_t	frag_len;
		// compute the fragment length
		if( pkt.get_len() > outter_mtu )	frag_len	= outter_mtu;
		else					frag_len	= pkt.get_len();
		// build the fragment
		fragment.tail_add( pkt.head_consume(frag_len) );
		// send the fragment
		bool tokeep = nlay_full->notify_pkt_to_lower(fragment, get_full_api_lower());
		if( !tokeep )	return nlay_err_t::BOGUS_PKT;	// TODO what error to return
	}
	
	// NOTE: no need to swallow the packet as the fragmentation already consume it
	DBG_ASSERT( pkt.get_len() == 0 );


	return nlay_err_t::OK;
}

/** \brief handle packet from the lower network layer
 * 
 * - this function MUST NEVER be called, it is just a stub for nlay_full_api_t
 */
nlay_err_t	nlay_bstream2dgram_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	nlay_full_t *	nlay_full	= get_nlay_full();
	
	// if the packet is already smaller that the inner mtu and no other packet are queued, do nothing
	if( pkt.get_len() <= inner_mtu && recved_data.get_len() == 0 )	return nlay_err_t::OK;

	// queue the received data
	recved_data.tail_add( pkt.to_datum(datum_t::NOCOPY) );

	// swallow the packet
	pkt = pkt_t();

	// fragment the whole packet in fragment of at most outter_mtu byte
	while( recved_data.get_len() ){
		pkt_t	fragment;
		size_t	frag_len;
		// compute the fragment length
		if( pkt.get_len() > inner_mtu )		frag_len	= inner_mtu;
		else					frag_len	= pkt.get_len();
		// build the fragment
		fragment.tail_add( recved_data.head_consume(frag_len) );
		// send the fragment
		bool tokeep = nlay_full->notify_pkt_to_upper(fragment, get_regpkt_upper());
		if( !tokeep )	return nlay_err_t::BOGUS_PKT;	// TODO what error to return
		// if the inner_mtu is 0, return now
		// - this can happen if the callback modify the inner_mtu to 0, see file's top for detail
		if( inner_mtu == 0 )	return nlay_err_t::OK;
	}

	return nlay_err_t::OK;
}



NEOIP_NAMESPACE_END


