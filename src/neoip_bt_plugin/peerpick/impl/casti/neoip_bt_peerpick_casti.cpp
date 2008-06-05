/*! \file
    \brief Class to handle the bt_peerpick_casti_t

- TODO to port to the 'regular' architecture as in bt_peerpick_casto_t/bt_peerpick_plain_t
  - use of bt_peerpick_helper_t
  - use of the reqauth precendence

\par Brief Description
bt_peerpick_vapi_t is handling the peer picking for the bt_swarm_sched_full_t.
bt_peerpick_casti_t is applying the peer picking policy for neoip-casti.
- TODO find a more generic name that 'casti'


\par Possible Improvement
- to make use of the rate_sched_t and rate_prec_t to have a more flexible
  bandwidth allocation.
  
\par About cnx_minrate
cnx_minrate is the minimal rate ensured to be given to every connection.
- cnx_minrate default to 0 and may be set at anytime during the lifetime of this
  object.
- if cnx_minrate == 0, this is ignored
- if the bt_swarm_t has no limited xmit_rate, this is ignored
  - because it is impossible to determine the number of connection to put in 
    HOPE, if the maximal rate is not available.
  - TODO maybe it could be possible to use the bt_swarm_full_t xmit_rate 
    to tune this. but currently i dont know how to do that 
    - this is a limitation as the user may not know the maximal rate and so wont
      be able to configure it.  
- GOAL: with casti, the data MUST be delivered with a garanteed limited
  - else the casto starts to be later and later until they are desynched
  - so here the data are garanteed to be delivered at the cnx_minrate() speed.
- USAGE: currently this is used by neoip-casti
  - neoip-casti estimates the rate of the bt_httpi_t and regularly update 
    the bt_peerpick_casti_t::cnx_minrate() with it
- MONSTER CASE if no cnx_minrate: with casti and many casto
  - say all casto got all the pieces and suddently casti makes one more available
  - then all casto will start downloading this single new piece from the casti
  - they will mutually slow the others down and all casto will download the piece 
    slower than it is delivered and get all desynched

*/

/* system include */
/* local include */
#include "neoip_bt_peerpick_casti.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_sched.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_utmsg_byteacct_cnx.hpp"
#include "neoip_rate_sched.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_peerpick_casti_t::bt_peerpick_casti_t()	throw()
{
	// zero some variable
	m_bt_swarm	= NULL;
}

/** \brief Destructor
 */
bt_peerpick_casti_t::~bt_peerpick_casti_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_peerpick_casti_t &bt_peerpick_casti_t::profile(const bt_peerpick_casti_profile_t &m_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( m_profile.check().succeed() );	
	// copy the parameter
	this->m_profile	= m_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_peerpick_casti_t::start(bt_swarm_t *m_bt_swarm)	throw()
{
	// copy the parameters
	this->m_bt_swarm	= m_bt_swarm;
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     peer selection 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/** \brief Update the peer selection
 */
void	bt_peerpick_casti_t::peerpick_update()				throw()
{
	delay_t				expire_delay	= profile().reqauth_hope_timeout();
	std::list<bt_swarm_full_t *> &	swarm_full_db	= bt_swarm()->get_full_db();
	size_t				cur_nbcnx	= 0;
	std::multimap<double, bt_swarm_full_t *>	cand_full_db;
	// go thru the whole swarm_full_db to copy all the candidates
	std::list<bt_swarm_full_t *>::iterator iter;
	for(iter = swarm_full_db.begin(); iter != swarm_full_db.end(); iter++){
		bt_swarm_full_t *	swarm_full	= *iter;
		bt_swarm_full_sched_t *	full_sched	= swarm_full->full_sched();
		bt_swarm_full_utmsg_t *	full_utmsg	= swarm_full->full_utmsg();
		// count the number of bt_swarm_full_t in bt_reqauth_type_t::HOPE
		if( full_sched->reqauth_type() == bt_reqauth_type_t::HOPE )	cur_nbcnx++;
		// if the remote peer doesnt want to request, no need to authorize it
		if( swarm_full->other_dowant_req() == false )			continue;
		// if the full_sched is not in bt_reqauth_type_t::DENY, dont autorize it
		if( full_sched->reqauth_type() != bt_reqauth_type_t::DENY )	continue;
		
		// determine the 'balance' of this bt_swarm_full_t based on bt_utmsg_byteacct_t
		// - balance is 0 IF bt_utmsgtype_t::BYTEACCT is not remotely supported
		// - balance is 0 IF remote peer xmit_rate >= recv_rate
		// - balance is > 0 in all other case
		// - NOTE: this is used for sorting the cand_full_db
		//   - the higher the balance the most likely it is to be selected
		double			balance		= 0;
		bt_utmsg_cnx_vapi_t *	utmsg_cnx	= full_utmsg->cnx_vapi(bt_utmsgtype_t::BYTEACCT);
		while( utmsg_cnx ){
			bt_utmsg_byteacct_cnx_t *byteacct_cnx;
			byteacct_cnx	= dynamic_cast<bt_utmsg_byteacct_cnx_t*>(utmsg_cnx);
			DBG_ASSERT( byteacct_cnx );
			// if no rate is yet available, do nothing
			if( !byteacct_cnx->rate_available() )				break;
			// if byteacct_cnx xmit_rate is <= than recv_rate, do nothing
			if( byteacct_cnx->xmit_rate() <= byteacct_cnx->recv_rate() )	break;
			// set the balance to xmit_rate - recv_rate
			balance	= byteacct_cnx->xmit_rate() - byteacct_cnx->recv_rate();
			// sanity check - balance MUST be >= 0 at this point
			DBG_ASSERT( balance >= 0 );
			break;
		}
		
		// put this swarm_full in the cand_full_db
		cand_full_db.insert( std::make_pair(balance, swarm_full) );
	}

	// if the cand_full_db is empty, return now
	if( cand_full_db.empty() )	return;

// TODO all this part is a mess
// - clean this up

	// go thru all the candidates and pass them as bt_reqauth_type_t::HOPE
	size_t	nbcnx_2launch	= profile().reqauth_hope_max();

	// if the bt_swarm_t got limited xmit_rate, honor the cnx_minrate
	// - aka ensure that every connection have at least cnx_minrate available 
	const bt_swarm_profile_t &	swarm_profile	= bt_swarm()->profile();
	while( swarm_profile.xmit_limit_arg().is_valid() ){
		rate_sched_t *	rate_sched	= swarm_profile.xmit_limit_arg().rate_sched();
		// if rate_sched_t IS NOT limited, do nothing
		if( rate_sched->max_rate() == rate_sched_t::INFINITE_RATE )	break;
		// if cnx_minrate IS NOT set, do nothing
		if( cnx_minrate() == 0 )					break;
		// compute the max_nbcnx based on the rate limits
		size_t nbcnx_4rate;
		nbcnx_4rate	= size_t(rate_sched->max_rate() / cnx_minrate());
		nbcnx_4rate	= std::max(nbcnx_4rate, size_t(1));
		nbcnx_2launch	= std::min(nbcnx_2launch, nbcnx_4rate);
		break;
	}


	// if the current number of cnx is already >= nbcnx_2launch , do nothing
	if( cur_nbcnx >= nbcnx_2launch )	return;
	
	// only launch the amount of connection required
	nbcnx_2launch	-= cur_nbcnx;

	// clamp the nbcnx_2launch by the size of the cand_full_db
	nbcnx_2launch	= std::min(nbcnx_2launch, cand_full_db.size());

	// launch all the bt_reqauth_type_t::HOPE needed
	// - from the greater balance to the lowest
	std::map<double, bt_swarm_full_t *>::reverse_iterator cand_iter = cand_full_db.rbegin();
	for(size_t i = 0; i < nbcnx_2launch; i++, cand_iter++){
		bt_swarm_full_t	* 	swarm_full	= cand_iter->second;
		bt_swarm_full_sched_t *	full_sched	= swarm_full->full_sched();
		// pass the chosen_full in bt_reqauth_type_t::HOPE
		full_sched->reqauth_type(bt_reqauth_type_t::HOPE, expire_delay);	
		full_sched->sync_doauth_req_cmd();
	}
}

NEOIP_NAMESPACE_END





