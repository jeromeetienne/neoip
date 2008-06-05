/*! \file
    \brief Definition of the nlay_ordgram_t
   
*/

/* system include */
/* local include */
#include "neoip_nlay_ordgram.hpp"
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
nlay_ordgram_t::nlay_ordgram_t()	throw()
{
}

/** \brief Destructor
 */
nlay_ordgram_t::~nlay_ordgram_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           ?????
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief register a nlay_full_api_t
 */
void	nlay_ordgram_t::register_handler(nlay_regpkt_t *regpkt_lower)	throw()
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
nlay_err_t	nlay_ordgram_t::start()	throw()
{
	// Start the action
	rdgram_err_t	rdgram_err = ordgram.start(this, NULL);
	if( !rdgram_err.succeed() )	return nlay_err_from_ordgram(rdgram_err);
	return nlay_err_t::OK;
}

/** \brief Start the closure for this brick
 */
void	nlay_ordgram_t::start_closure()				throw()
{
	ordgram.stop();
}

/** \brief return the space overhead due to this brick in the mtu
 */
size_t		nlay_ordgram_t::get_mtu_overhead()				const throw()
{
	return ordgram.get_mtu_overhead();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   ordgram callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when \ref ordgram_t send an event.
 */
bool nlay_ordgram_t::neoip_ordgram_event_cb(void *cb_userptr, ordgram_t &cb_ordgram
					, const ordgram_event_t &ordgram_event)	throw()
{
	nlay_full_t *	nlay_full	= get_nlay_full();
	nlay_event_t	nlay_event;
	pkt_t *		pkt;

	// handle the event depending of its type
	switch(ordgram_event.get_value()){
	case ordgram_event_t::MAYSEND_OFF:
			// simply convert the event
			nlay_event = nlay_event_t::build_maysend_off();
			return nlay_full->notify_event(nlay_event);
	case ordgram_event_t::MAYSEND_ON:
			// simply convert the event
			nlay_event = nlay_event_t::build_maysend_on();
			return nlay_full->notify_event(nlay_event);
	case ordgram_event_t::PKT_TO_LOWER:
			// process the packet thru the lower layers of nlay_full_t
			pkt = ordgram_event.get_pkt_to_lower();
			return nlay_full->notify_pkt_to_lower(*pkt, get_full_api_lower());
	case ordgram_event_t::PKT_TO_UPPER:
			// process the packet thru the upper layers of nlay_full_t
			pkt = ordgram_event.get_pkt_to_upper();
			return nlay_full->notify_pkt_to_upper(*pkt, get_regpkt_upper());
	default:	DBG_ASSERT(0);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   packet processing from upper/lower
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the upper network layer
 */
nlay_err_t	nlay_ordgram_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	rdgram_err_t	rdgram_err	= ordgram.pkt_from_upper(pkt);
	// handle the error
	if( !rdgram_err.succeed() )	return nlay_err_from_ordgram(rdgram_err);
	// else report that no error occured
	return nlay_err_t::OK;
}

/** \brief handle packet from the lower network layer
 */
nlay_err_t	nlay_ordgram_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	rdgram_err_t	rdgram_err = ordgram.pkt_from_lower(pkt);
	if( !rdgram_err.succeed() )	return nlay_err_from_ordgram(rdgram_err);
	return nlay_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          maysend_threshold function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

nlay_err_t	nlay_ordgram_t::maysend_set_threshold(size_t threshold)	throw()
{
	// forward the function to nlay_ordgram_t
	rdgram_err_t	rdgram_err = ordgram.maysend_set_threshold(threshold);
	if( rdgram_err.failed() )	return nlay_err_from_rdgram(rdgram_err);
	// return no error
	return nlay_err_t::OK;
}

size_t	nlay_ordgram_t::maysend_get_threshold()			const throw()
{
	return ordgram.maysend_get_threshold();
}

bool	nlay_ordgram_t::maysend_is_set()		const throw()
{
	return ordgram.maysend_is_set();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            sendbuf function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

nlay_err_t	nlay_ordgram_t::sendbuf_set_max_len(size_t sendbuf_max_len)	throw()
{
	// forward the function to nlay_ordgram_t
	rdgram_err_t	rdgram_err = ordgram.sendbuf_set_max_len(sendbuf_max_len);
	if( rdgram_err.failed() )	return nlay_err_from_rdgram(rdgram_err);
	// return no error
	return nlay_err_t::OK;
}

size_t	nlay_ordgram_t::sendbuf_get_max_len()			const throw()
{
	return ordgram.sendbuf_get_max_len();
}

size_t	nlay_ordgram_t::sendbuf_get_used_len()			const throw()
{
	return ordgram.sendbuf_get_used_len();
}

size_t	nlay_ordgram_t::sendbuf_get_free_len()			const throw()
{
	return ordgram.sendbuf_get_free_len();
}

bool	nlay_ordgram_t::sendbuf_is_limited()			const throw()
{
	return ordgram.sendbuf_is_limited();
}


NEOIP_NAMESPACE_END


