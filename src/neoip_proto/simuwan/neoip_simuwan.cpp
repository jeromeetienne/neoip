/*! \file
    \brief Definition of simuwan_t

- TODO make all the parameters set/get

*/

/* system include */
/* local include */
#include "neoip_simuwan.hpp"
#include "neoip_rand.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref simuwan_t constant
#if 1
	//! default value for rate of dropped packets
	const double	simuwan_t::DROP_RATE_DFL	= 0;
	//! default value for rate of delayed packets
	const double	simuwan_t::DELAYED_RATE_DFL	= 0;
	//! default value for minimum delay for delayed packets
	const delay_t	simuwan_t::DELAYED_MIN_DFL	= delay_t::from_sec(0);
	//! default value for maximum delay for delayed packets
	const delay_t	simuwan_t::DELAYED_MAX_DFL	= delay_t::from_sec(0);
	//! default value for rate of duplicate packets
	const double	simuwan_t::DUPLICATE_RATE_DFL	= 0;
	//! default value for minimum of duplicate for one packet
	const int	simuwan_t::DUPLICATE_MIN_DFL	= 0;
	//! default value for maximum of duplicate for one packets
	const int	simuwan_t::DUPLICATE_MAX_DFL	= 0;
#else
	// value for debug
	const double	simuwan_t::DROP_RATE_DFL	= 0.2;
	const double	simuwan_t::DELAYED_RATE_DFL	= 0;
	const delay_t	simuwan_t::DELAYED_MIN_DFL	= delay_t::from_sec(0);
	const delay_t	simuwan_t::DELAYED_MAX_DFL	= delay_t::from_sec(5);
	const double	simuwan_t::DUPLICATE_RATE_DFL	= 0;
	const int	simuwan_t::DUPLICATE_MIN_DFL	= 2;
	const int	simuwan_t::DUPLICATE_MAX_DFL	= 5;
#endif
// end of constants definition


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                      ctor/dtor
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
simuwan_t::simuwan_t()	throw()
{
	// set defaul values
	drop_rate	= DROP_RATE_DFL;
	delayed_rate	= DELAYED_RATE_DFL;
	delayed_min	= DELAYED_MIN_DFL;
	delayed_max	= DELAYED_MAX_DFL;
	duplicate_rate	= DUPLICATE_RATE_DFL;
	duplicate_min	= DUPLICATE_MIN_DFL;
	duplicate_max	= DUPLICATE_MAX_DFL;
}

/** \brief Destructor
 */
simuwan_t::~simuwan_t()	throw()
{
	// delete all the pending packet
	while( pending_pkt_db.empty() == false )	nipmem_delete	pending_pkt_db.front();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       set functions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the parameters
 */
simuwan_err_t simuwan_t::set_from_profile(const simuwan_profile_t &profile)	throw()
{
	// check the profile
	simuwan_err_t	err = profile.check();
	if( err.failed() )	return err;
	// copy the values
	drop_rate	= profile.drop_rate();
	delayed_rate	= profile.delayed_rate();
	delayed_min	= profile.delayed_min();
	delayed_max	= profile.delayed_max();
	duplicate_rate	= profile.duplicate_rate();
	duplicate_min	= profile.duplicate_min();
	duplicate_max	= profile.duplicate_max();
	return simuwan_err_t::OK;
}

/** \brief set the event callback
 */
simuwan_err_t simuwan_t::set_callback(simuwan_cb_t * callback, void *userptr)	throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
	return simuwan_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       start() functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief start the action
 */
simuwan_err_t	simuwan_t::start()					throw()
{
	// sanity check - the callback MUST be set
	DBG_ASSERT( callback );
	
	return simuwan_err_t::OK;
}

/** \brief Set the mandatory parameters and start the actions
 * 
 * - This is only a helper function on top of the actual functions
 */
simuwan_err_t	simuwan_t::start(simuwan_cb_t *callback, void *userptr)	throw()
{
	simuwan_err_t	simuwan_err;
	// set the callback
	simuwan_err = set_callback(callback, userptr);
	if( simuwan_err.failed() )	return simuwan_err;
	// start the action
	return start();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//                      from upper/lower
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/** \brief receive packet from the upper layer
 */
simuwan_err_t	simuwan_t::pkt_from_upper(pkt_t &pkt)				throw()
{
	// should this packet be dropped
	if( drop_rate > neoip_rand() )	goto pkt_intercepted;

	// should this packet be delayed
	if( delayed_rate > neoip_rand() ){
		delay_t	delayed_time = delay_t((uint32_t)neoip_rand(delayed_min.to_msec_32bit()
							, delayed_max.to_msec_32bit()));
		nipmem_new pending_pkt_t(this, pkt, delayed_time);		
		goto pkt_intercepted;
	}

	// should this packet be duplicated
	if( duplicate_rate > neoip_rand() ){
		int	nb_duplicate = (int)neoip_rand(duplicate_min, duplicate_max);
		for( int i = 0; i < nb_duplicate; i++ ){
			delay_t	delayed_time = delay_t((uint32_t)neoip_rand(delayed_min.to_msec_32bit()
							, delayed_max.to_msec_32bit()));
			nipmem_new pending_pkt_t(this, pkt, delayed_time);
		}
		// the packet goes thru (so no goto pkt_intercepted)
	}

	// return no error
	return simuwan_err_t::OK;

pkt_intercepted:;	// mark the packet as swallowed
			pkt	= pkt_t();
			// return no error
			return simuwan_err_t::OK;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//	            PENDING PKT
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
simuwan_t::pending_pkt_t::pending_pkt_t(simuwan_t *simuwan, const pkt_t &pkt, const delay_t &delay)throw()
{
	// set local variables
	this->simuwan	= simuwan;
	this->pkt	= pkt;
	// start the expiration timeout
	expire_timeout.start(delay, this, NULL);
	// link it to the simuwan_t
	simuwan->pending_pkt_link(this);
}

/** \brief Destructor
 */
simuwan_t::pending_pkt_t::~pending_pkt_t()		throw()
{
	// unlink it to the simuwan_t
	simuwan->pending_pkt_unlink(this);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   expire_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	simuwan_t::pending_pkt_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// notify an event to send the packet
	simuwan_event_t	simuwan_event = simuwan_event_t::build_pkt_to_lower(&pkt);
	bool to_keep = simuwan->callback->neoip_simuwan_event_cb(simuwan->userptr,*simuwan,simuwan_event);
	if( !to_keep )	return false;
	
	// autodelete
	nipmem_delete this;
	return false;
}



NEOIP_NAMESPACE_END



