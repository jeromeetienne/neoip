/*! \file
    \brief Definition of the nlay_idletimeout_t

\par Implementation Notes
- this layer is too simple to be coded externally to nlay_full_t

*/

/* system include */
/* local include */
#include "neoip_nlay_idletimeout.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_full.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


// definition of \ref idletimeout_t constant
#if 1
	const delay_t	nlay_idletimeout_t::IDLE_DELAY_DFL	= delay_t(delay_t::INFINITE_VAL);
#else
	// value for debug
	const delay_t	nlay_idletimeout_t::IDLE_DELAY_DFL	= delay_t::from_sec(60);
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_idletimeout_t::nlay_idletimeout_t()	throw()
{
	idle_delay	= IDLE_DELAY_DFL;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  timeout expiration
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	nlay_idletimeout_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)			throw()
{
	nlay_full_t *	nlay_full	= get_nlay_full();
	nlay_event_t	nlay_event;	
	// notify a close event
	nlay_event = nlay_event_t::build_idle_timedout("idle timeout after " + idle_delay.to_string());
	return nlay_full->notify_event(nlay_event);
}

/** \brief handle the reception of a non-idle proof (aka a packet sent/recv by the *caller*)
 */
void	nlay_idletimeout_t::recv_proof()	throw()
{
	// restart the timeout
	idle_timeout.start(idle_delay, this, NULL);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 function inherited from nlay_full_api_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief register a nlay_full_api_t
 */        
void	nlay_idletimeout_t::register_handler(nlay_regpkt_t *regpkt_lower)	throw()
{	
	// register a it as a default packet handler
	regpkt_lower->set_dfl_handler(this);	
}

/** \brief Start the operation for this brick
 */
nlay_err_t	nlay_idletimeout_t::set_parameter(const nlay_profile_t *nlay_profile
					, const nlay_nego_result_t *nego_result)	throw()
{
	// check the profile
	nlay_err_t	err = nlay_profile->idletimeout().check();
	if( err.failed() )	return err;
		
	idle_delay	= nlay_profile->idletimeout().idle_delay();
	return nlay_err_t::OK;
}

/** \brief Start the operation for this brick
 */
nlay_err_t	nlay_idletimeout_t::start()					throw()
{
	// start the timeout
	idle_timeout.start(idle_delay, this, NULL);
	return nlay_err_t::OK;
}

/** \brief Start the closure for this brick
 */
void		nlay_idletimeout_t::start_closure()				throw()
{
	idle_timeout.stop();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  pkt filtering from lower/upper layer
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the upper network layer
 */
nlay_err_t	nlay_idletimeout_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	// NOTE: dont modify the packet
	// considere this packet as a proof of non idle-ness
	recv_proof();
	// return no error
	return nlay_err_t::OK;	
}

/** \brief handle packet from the lower network layer
 */
nlay_err_t	nlay_idletimeout_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	// NOTE: dont modify the packet
	// considere this packet as a proof of non idle-ness
	recv_proof();
	// return no error
	return nlay_err_t::OK;	
}



NEOIP_NAMESPACE_END


