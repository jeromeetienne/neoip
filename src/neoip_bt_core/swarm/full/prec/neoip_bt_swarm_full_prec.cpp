/*! \file
    \brief Class to handle the bt_swarm_full_t

\par TODO
- TODO make the profile to tune all the weight
  - issue as bt_swarm_full_prec_profile_t is nowhere
  - some weird stuff with the profile and bt_swarm_t
    - e.g. bt_swarm_full_profile_t doesnt exist
    - all seems mixed in bt_swarm_profile_t
    - dirty bad... bouh bouh caca
- TODO look at the issue with the 'ctor' or dtor
  - how to set the original rate_sched_t
  - how to set it back to 0 when in DENY
- TODO how to test this code
- DONE code the formula to produce a single rate_prec_t

\par Brief Description
bt_swarm_full_prec_t handle the precedence of the attached bt_swarm_full_t.
This is used to interface with the rate_prec_t for the rate_sched_xmit/recv
and thus being able to prioritize the bandwidth allocated to each bt_swarm_full_t.
This provide a flexibility thanks to the 'allocate all what is available' feature
of rate_sched_t, thus if a connection got a high precedence but dont use it, the
unused resource is allocated to the other connections with lower precedence.

\par Implementation Note
- each factor is tuned externally by the various 'interested parts' with a 
  number from 0 to 1.0 included
  - it act as a sub precedence local to this parts
- then those various factors are unified according to tunable weights into 
  a single rate_prec_t which is set in the proper rate_limit_t/rate_sched_t
- thus the 'interested parts' only have to care about their own local precedence
  and may ignored how all the parts are unified into a single rate_prec_t

\par Ideas About the factor which influence the rate_prec_t computation
- the identity of the remote peer
  - thus the friends of the local peer are prefered
- by bt_reqauth_type_t
  - e.g. if bt_reqauth_type_t::COOP is more prefered than bt_requath_type_t::HOPE
  - NOTE: this one is coded
- by the requested piece
  - in xmit, the rarest piece are prefered to the more frequent (to dillute the data
    more efficiently)
    - remote_pwish / remote_pfreq ?
  - in recv, the higher local bt_pieceprec_t got prefered 

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_full_prec.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_swarm_full_prec_t::bt_swarm_full_prec_t(bt_swarm_full_t *swarm_full)	throw()
{
	// copy the parameters
	this->swarm_full	= swarm_full;
	
	// zero some variables
	m_reqauth_prec_xmit	= 0.0;
	m_reqauth_prec_recv	= 0.0;
}

/** \brief Destructor
 */
bt_swarm_full_prec_t::~bt_swarm_full_prec_t()					throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the current
 */
rate_prec_t	bt_swarm_full_prec_t::rate_prec_xmit()	const throw()
{
	bt_swarm_t *				bt_swarm= swarm_full->get_swarm();
	const bt_swarm_full_prec_profile_t &	profile	= bt_swarm->profile().full_prec();
	// compute the precedence
	double	prec = 0.0;
	prec	+= profile.xmit_prec_base();
	prec	+= profile.xmit_reqauth_range() * reqauth_prec_xmit();
	// return the rate_prec_t
	return rate_prec_t(size_t(prec));
}

/** \brief Return the current
 */
rate_prec_t	bt_swarm_full_prec_t::rate_prec_recv()	const throw()
{
	bt_swarm_t *				bt_swarm= swarm_full->get_swarm();
	const bt_swarm_full_prec_profile_t &	profile	= bt_swarm->profile().full_prec();
	// compute the precedence
	double	prec = 0.0;
	prec	+= profile.recv_prec_base();
	prec	+= profile.recv_reqauth_range() * reqauth_prec_recv();
	// return the rate_prec_t
	return rate_prec_t(size_t(prec));
}

/** \brief Update the precendence of this bt_swarm_full_t in the xmit rate_sched_t
 */
void	bt_swarm_full_prec_t::update_prec_xmit()		throw()
{
	socket_full_t *	socket_full	= swarm_full->socket_full;
	DBG_ASSERT( socket_full );
	// compute the rate_prec_t for the xmit
	rate_prec_t	rate_prec	= rate_prec_xmit();
	// if no rate_limit_t is set for xmit, do nothing and return now 
	if( socket_full->xmit_limit_is_set() == false )	return;
	// set the new rate_prec_t for the xmit rate_limit_t of the socket
	socket_full->xmit_limit().maxi_prec(rate_prec);
}

/** \brief Update the precendence of this bt_swarm_full_t in the recv rate_sched_t
 */
void	bt_swarm_full_prec_t::update_prec_recv()		throw()
{
	socket_full_t *	socket_full	= swarm_full->socket_full;
	DBG_ASSERT( socket_full );
	// compute the rate_prec_t for the recv
	rate_prec_t	rate_prec	= rate_prec_recv();
	// if no rate_limit_t is set for recv, do nothing and return now 
	if( socket_full->recv_limit_is_set() == false )	return;
	// set the new rate_prec_t for the recv rate_limit_t of the socket	
	socket_full->recv_limit().maxi_prec(rate_prec);
}

NEOIP_NAMESPACE_END





