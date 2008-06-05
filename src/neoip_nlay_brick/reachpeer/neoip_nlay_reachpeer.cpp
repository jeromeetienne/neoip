/*! \file
    \brief Definition of the nlay_reachpeer_t
    
\par Note about the delay negociation
- MOTIVATION: if a given peer has many idle connections, to have a short silent_delay will
  produce a lot of overhead, potentially useless.
  - in order to avoid this, one peer should be able to restrict the silent_delay
    of the other.
- MEAN: 
  - thru connection negociation ?
  - thru a established connection ?
- QUESTIONS:
  - which parameter should be tuned ?
    - the silent_delay ?
    - the unreach_delay ?
    - the number of probe ?
- NOTE:
  - a range negociation is already coded in negorange_t
    - e.g. negorange_t<delay_t>	silent_delay

*/

/* system include */
/* local include */
#include "neoip_nlay_reachpeer.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_full.hpp"
#include "neoip_reachpeer_pkttype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_reachpeer_t::nlay_reachpeer_t()	throw()
{
}

/** \brief Destructor
 */
nlay_reachpeer_t::~nlay_reachpeer_t()	throw()
{
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    ??????
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief define if the outter connection is reliable or not
 */
nlay_err_t nlay_reachpeer_t::set_reliable_outter_cnx(bool value)		throw()
{
	// Start the action
	reachpeer_err_t	reachpeer_err = reachpeer.set_reliable_outter_cnx(value);
	if( !reachpeer_err.succeed() )	return nlay_err_from_reachpeer(reachpeer_err);
	return nlay_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          nlay_full_api_t functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief register a nlay_full_api_t
 */
void	nlay_reachpeer_t::register_handler(nlay_regpkt_t *regpkt_lower)	throw()
{	
	reachpeer_pkttype_t	pkttype;
	// register a packet type
	pkttype	= reachpeer_pkttype_t::ECHO_REQUEST;
	regpkt_lower->register_handler(pkttype.get_value(), this);	
	// register a packet type
	pkttype	= reachpeer_pkttype_t::ECHO_REPLY;
	regpkt_lower->register_handler(pkttype.get_value(), this);	
	// register a it as a default packet handler
	// - special case to handle any incoming packet as a proof of reachability
	regpkt_lower->set_dfl_handler(this);
}

/** \brief Start the operation for this brick
 */
nlay_err_t	nlay_reachpeer_t::set_parameter(const nlay_profile_t *nlay_profile
					, const nlay_nego_result_t *nego_result)	throw()
{
	reachpeer_err_t	err = reachpeer.set_from_profile(nlay_profile->reachpeer());
	if( err.failed() )	return nlay_err_from_reachpeer(err);
	return nlay_err_t::OK;
}

/** \brief Start the operation for this brick
 */
nlay_err_t	nlay_reachpeer_t::start()		throw()
{
	// Start the action
	reachpeer_err_t	reachpeer_err = reachpeer.start(this, NULL);
	if( !reachpeer_err.succeed() )	return nlay_err_from_reachpeer(reachpeer_err);
	return nlay_err_t::OK;
}

/** \brief Start the closure for this brick
 */
void	nlay_reachpeer_t::start_closure()				throw()
{
	reachpeer.stop();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    reachpeer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief the callback class to notify event from \ref reachpeer_t
 */
bool	nlay_reachpeer_t::neoip_reachpeer_event_cb(void *cb_userptr
					, reachpeer_t &cb_reachpeer
					, const reachpeer_event_t &reachpeer_event ) throw()
{
	nlay_full_t *	nlay_full	= get_nlay_full();
	nlay_event_t	nlay_event;
	nlay_err_t	nlay_err;
	pkt_t *		pkt;
	
	// handle the event depending of its type
	switch(reachpeer_event.get_value()){
	case reachpeer_event_t::PEER_UNREACH:
			// simply convert the event
			nlay_event = nlay_event_t::build_remote_peer_unreachable("Remote peer unreachable");
			return nlay_full->notify_event(nlay_event);
	case reachpeer_event_t::PKT_TO_LOWER:
			// process the packet thru the lower layers of nlay_full_t
			pkt = reachpeer_event.get_pkt_to_lower();
			return nlay_full->notify_pkt_to_lower(*pkt, get_full_api_lower());
	default:	DBG_ASSERT(0);
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  pkt filtering from lower/upper layer
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the lower network layer
 * 
 * - this function handle the special case of reachpeer aka
 *   - it may receive packet for reachpeer itself (for the reachpeer protocol)
 *   - it may receive foreign packet for other layers but which are proof of reachability
 */
nlay_err_t	nlay_reachpeer_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	reachpeer_pkttype_t	pkttype;
	reachpeer_err_t		reachpeer_err;
	// read the pkttype (without consuming)	
	try {	pkt.unserial_peek( pkttype );
	}catch(serial_except_t &e){
		return nlay_err_t(nlay_err_t::BOGUS_PKT, "Cant read the packet type due to " + e.what());
	}	
	// switch to determine if the packet are for the reachpeer protocol or not
	switch( pkttype.get_value() ){
	case reachpeer_pkttype_t::ECHO_REQUEST:
	case reachpeer_pkttype_t::ECHO_REPLY:
			// if the packet are for the reachpeer protocol, send it to reachpeer
			reachpeer_err = reachpeer.pkt_from_lower(pkt);
			if( !reachpeer_err.succeed() )
				return nlay_err_from_reachpeer(reachpeer_err);
			break;
	default:	// if the packet are not for reach peer, handle it as a proof
			reachpeer.recv_proof(); 
			break;
	}

	return nlay_err_t::OK;
}



NEOIP_NAMESPACE_END


