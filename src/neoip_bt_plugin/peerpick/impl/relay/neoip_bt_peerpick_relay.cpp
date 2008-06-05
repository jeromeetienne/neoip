/*! \file
    \brief Class to handle the bt_peerpick_relay_t

\par Brief Description
bt_peerpick_vapi_t is handling the peer picking for the bt_swarm_sched_full_t.
bt_peerpick_relay_t puts all bt_swarm_full_t with other_dowant_req() in
bt_reqauth_type_t::GIVE

- TODO comment a lot about the chosen policy
  - what is it
  - why has this been chosen  

\par Usage Description
bt_peerpick_relay_t is used in neoip-btrelay.

*/

/* system include */
/* local include */
#include "neoip_bt_peerpick_relay.hpp"
#include "neoip_bt_peerpick_helper.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_sched.hpp"
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
bt_peerpick_relay_t::bt_peerpick_relay_t()	throw()
{
	// zero some variable
	m_bt_swarm	= NULL;
}

/** \brief Destructor
 */
bt_peerpick_relay_t::~bt_peerpick_relay_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
bt_peerpick_relay_t &bt_peerpick_relay_t::profile(const bt_peerpick_relay_profile_t &m_profile)	throw()
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
bt_err_t	bt_peerpick_relay_t::start(bt_swarm_t *m_bt_swarm)	throw()
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
void	bt_peerpick_relay_t::peerpick_update()				throw()
{
	std::list<bt_swarm_full_t *> &	swarm_full_db	= bt_swarm()->get_full_db();
	size_t				cur_nb_give	= 0;
	size_t				cur_nb_fstart	= 0;
	size_t				cur_nb_idle	= 0;
	// count the number of bt_swarm_full_t per bt_reqauth_type_t
	std::list<bt_swarm_full_t *>::iterator iter;
	for(iter = swarm_full_db.begin(); iter != swarm_full_db.end(); iter++){
		bt_swarm_full_sched_t * full_sched	= (*iter)->full_sched();
		switch( full_sched->reqauth_type().get_value() ){
		case bt_reqauth_type_t::GIVE:	cur_nb_give++;		break;
		case bt_reqauth_type_t::FSTART:	cur_nb_fstart++;	break;
		case bt_reqauth_type_t::IDLE:	cur_nb_idle++;		break;
		default:	break;
		}
	}

	// sanity check - cur_nb_give MUST BE <= profile().reqauth_give_max()
	DBG_ASSERT( cur_nb_give <= profile().reqauth_give_max() );
	// select all the new needed remote peers in bt_reqauth_type_t::GIVE
	size_t	nb_give_2launch		= profile().reqauth_give_max() - cur_nb_give;
	bt_peerpick_helper_t::pick_many_cnx(bt_reqauth_type_t::GIVE
				, profile().reqauth_give_timeout()
				, profile().reqauth_give_prec()
				, nb_give_2launch, bt_swarm()
				, bt_peerpick_helper_t::pick_highest_giver);

	// sanity check - cur_nb_fstart MUST BE <= profile().reqauth_fstart_max()
	DBG_ASSERT( cur_nb_fstart <= profile().reqauth_fstart_max() );
	// select all the new needed remote peers in bt_reqauth_type_t::FSTART
	size_t	nb_fstart_2launch	= profile().reqauth_fstart_max() - cur_nb_fstart;
	bt_peerpick_helper_t::pick_many_cnx(bt_reqauth_type_t::FSTART
				, profile().reqauth_fstart_timeout()
				, profile().reqauth_fstart_prec()
				, nb_fstart_2launch, bt_swarm()
				, bt_peerpick_helper_t::pick_random_fstart);

	// sanity check - cur_nb_idle MUST BE <= profile().reqauth_idle_max()
	DBG_ASSERT( cur_nb_idle <= profile().reqauth_idle_max() );
	// select all the new needed remote peers in bt_reqauth_type_t::IDLE
	size_t	nb_idle_2launch		= profile().reqauth_idle_max() - cur_nb_idle;
	bt_peerpick_helper_t::pick_many_cnx(bt_reqauth_type_t::IDLE
				, profile().reqauth_idle_timeout()
				, profile().reqauth_idle_prec()
				, nb_idle_2launch, bt_swarm()
				, bt_peerpick_helper_t::pick_random_any);
}

NEOIP_NAMESPACE_END





