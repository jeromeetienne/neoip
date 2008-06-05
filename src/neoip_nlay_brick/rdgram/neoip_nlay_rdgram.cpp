/*! \file
    \brief Definition of the nlay_rdgram_t
   
*/

/* system include */
/* local include */
#include "neoip_nlay_rdgram.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_full.hpp"
#include "neoip_rdgram_pkttype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_rdgram_t::nlay_rdgram_t()	throw()
{
}

/** \brief Destructor
 */
nlay_rdgram_t::~nlay_rdgram_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           ?????
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief register a nlay_full_api_t
 */
void	nlay_rdgram_t::register_handler(nlay_regpkt_t *regpkt_lower)	throw()
{	
	rdgram_pkttype_t	pkttype;
	// register a packet type
	pkttype	= rdgram_pkttype_t::DATA_PKT;
	regpkt_lower->register_handler(pkttype.get_value(), this);	
	// register a packet type
	pkttype	= rdgram_pkttype_t::ACK_PKT;
	regpkt_lower->register_handler(pkttype.get_value(), this);	
}

/** \brief Start the operation for this brick
 */
nlay_err_t	nlay_rdgram_t::start()	throw()
{
	// Start the action
	rdgram_err_t	rdgram_err = rdgram.start(this, NULL);
	if( !rdgram_err.succeed() )	return nlay_err_from_ordgram(rdgram_err);
	return nlay_err_t::OK;
}

/** \brief Start the closure for this brick
 */
void	nlay_rdgram_t::start_closure()				throw()
{
	rdgram.stop();
}

/** \brief return the space overhead due to this brick in the mtu
 */
size_t		nlay_rdgram_t::get_mtu_overhead()				const throw()
{
	return rdgram.get_mtu_overhead();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   rdgram callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when \ref rdgram_t send an event.
 */
bool nlay_rdgram_t::neoip_rdgram_event_cb(void *cb_userptr, rdgram_t &cb_rdgram
					, const rdgram_event_t &rdgram_event )	throw()
{
	nlay_full_t *	nlay_full	= get_nlay_full();
	nlay_event_t	nlay_event;
	pkt_t *		pkt;

	// handle the event depending of its type
	switch(rdgram_event.get_value()){
	case rdgram_event_t::MAYSEND_OFF:
			// simply convert the event
			nlay_event = nlay_event_t::build_maysend_off();
			return nlay_full->notify_event(nlay_event);
	case rdgram_event_t::MAYSEND_ON:
			// simply convert the event
			nlay_event = nlay_event_t::build_maysend_on();
			return nlay_full->notify_event(nlay_event);
	case rdgram_event_t::PKT_TO_LOWER:
			// process the packet thru the lower layers of nlay_full_t
			pkt = rdgram_event.get_pkt_to_lower();
			return nlay_full->notify_pkt_to_lower(*pkt, get_full_api_lower());
	default:	DBG_ASSERT(0);
	}
	return true;
}

/** \brief callback notified when \ref rdgram_t just receive a packet and wish to know
 *         if this packet must be acknowledged or not.
 */
bool nlay_rdgram_t::neoip_rdgram_should_acked_pkt_cb(void *cb_userptr, rdgram_t &cb_rdgram
					, const pkt_t &pkt, const wai32_t &seqnb)	throw()
{
	// nlay_rdgram_t has no receive buffer, So it always acknowledge the packet
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   packet processing from upper/lower
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the upper network layer
 */
nlay_err_t	nlay_rdgram_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	rdgram_err_t	rdgram_err	= rdgram.pkt_from_upper(pkt);
	// handle the error
	if( !rdgram_err.succeed() )	return nlay_err_from_rdgram(rdgram_err);
	// else report that no error occured
	return nlay_err_t::OK;
}

/** \brief handle packet from the lower network layer
 */
nlay_err_t	nlay_rdgram_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	wai32_t		seqnb_out;
	rdgram_err_t	rdgram_err = rdgram.pkt_from_lower(pkt, seqnb_out);
	if( !rdgram_err.succeed() )	return nlay_err_from_rdgram(rdgram_err);
	return nlay_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          maysend_threshold function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

nlay_err_t	nlay_rdgram_t::maysend_set_threshold(size_t threshold)	throw()
{
	// forward the function to nlay_rdgram_t
	rdgram_err_t	rdgram_err = rdgram.maysend_set_threshold(threshold);
	if( rdgram_err.failed() )	return nlay_err_from_rdgram(rdgram_err);
	// return no error
	return nlay_err_t::OK;
}

size_t	nlay_rdgram_t::maysend_get_threshold()			const throw()
{
	return rdgram.maysend_get_threshold();
}

bool	nlay_rdgram_t::maysend_is_set()		const throw()
{
	return rdgram.maysend_is_set();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            sendbuf function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

nlay_err_t	nlay_rdgram_t::sendbuf_set_max_len(size_t sendbuf_max_len)	throw()
{
	// forward the function to nlay_rdgram_t
	rdgram_err_t	rdgram_err = rdgram.sendbuf_set_max_len(sendbuf_max_len);
	if( rdgram_err.failed() )	return nlay_err_from_rdgram(rdgram_err);
	// return no error
	return nlay_err_t::OK;
}

size_t	nlay_rdgram_t::sendbuf_get_max_len()			const throw()
{
	return rdgram.sendbuf_get_max_len();
}

size_t	nlay_rdgram_t::sendbuf_get_used_len()			const throw()
{
	return rdgram.sendbuf_get_used_len();
}

size_t	nlay_rdgram_t::sendbuf_get_free_len()			const throw()
{
	return rdgram.sendbuf_get_free_len();
}

bool	nlay_rdgram_t::sendbuf_is_limited()			const throw()
{
	return rdgram.sendbuf_is_limited();
}


NEOIP_NAMESPACE_END


