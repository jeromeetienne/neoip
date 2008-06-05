/*! \file
    \brief Class to handle the bt_peerpick_plain_t

\par Brief Description
bt_peerpick_vapi_t is handling the peer picking for the bt_swarm_sched_full_t.
bt_peerpick_plain_t is applying the peer picking policy of the 'plain' bittorrent.

*/

/* system include */
/* local include */
#include "neoip_bt_peerpick_plain.hpp"
#include "neoip_bt_peerpick_helper.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_sched.hpp"
#include "neoip_bt_swarm.hpp"
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
bt_peerpick_plain_t::bt_peerpick_plain_t()	throw()
{
	// zero some variable
	m_bt_swarm	= NULL;
}

/** \brief Destructor
 */
bt_peerpick_plain_t::~bt_peerpick_plain_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_peerpick_plain_t &bt_peerpick_plain_t::profile(const bt_peerpick_plain_profile_t &m_profile)	throw()
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
bt_err_t	bt_peerpick_plain_t::start(bt_swarm_t *m_bt_swarm)	throw()
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

/** \brief Update the peer selection when this bt_swarm_t is leeching
 */
void	bt_peerpick_plain_t::peer_select_update_leech()				throw()
{
	std::list<bt_swarm_full_t *> &	swarm_full_db	= bt_swarm()->get_full_db();
	size_t				cur_nb_coop	= 0;
	size_t				cur_nb_hope	= 0;
	// count the number of bt_swarm_full_t in bt_reqauth_type_t::COOP 
	// or bt_reqauth_type_t::HOPE
	std::list<bt_swarm_full_t *>::iterator iter;
	for(iter = swarm_full_db.begin(); iter != swarm_full_db.end(); iter++){
		bt_swarm_full_sched_t * full_sched	= (*iter)->full_sched();
		switch( full_sched->reqauth_type().get_value() ){
		case bt_reqauth_type_t::COOP:	cur_nb_coop++;		break;
		case bt_reqauth_type_t::HOPE:	cur_nb_hope++;		break;
		default:	break;
		}
	}
	// sanity check - cur_nb_coop MUST BE <= profile().reqauth_coop_max()
	DBG_ASSERT( cur_nb_coop <= profile().reqauth_coop_max() );
	// sanity check - cur_nb_hope MUST BE <= profile().reqauth_hope_max()
	DBG_ASSERT( cur_nb_hope <= profile().reqauth_hope_max() );

	// select all the new needed remote peers in bt_reqauth_type_t::COOP
	size_t	nb_coop_2launch		= profile().reqauth_coop_max() - cur_nb_coop;
	bt_peerpick_helper_t::pick_many_cnx(bt_reqauth_type_t::COOP
				, profile().reqauth_coop_timeout()
				, profile().reqauth_coop_prec()
				, nb_coop_2launch, bt_swarm()
				, bt_peerpick_helper_t::pick_fastest_recv);

	// select all the new needed remote peers in bt_reqauth_type_t::HOPE
	size_t	nb_hope_2launch		= profile().reqauth_hope_max() - cur_nb_hope;
	bt_peerpick_helper_t::pick_many_cnx(bt_reqauth_type_t::HOPE
				, profile().reqauth_hope_timeout()
				, profile().reqauth_hope_prec()
				, nb_hope_2launch, bt_swarm()
				, bt_peerpick_helper_t::pick_random_any);
}

/** \brief Update the peer selection when this bt_swarm_t is seeding
 */
void	bt_peerpick_plain_t::peer_select_update_seed()				throw()
{
	std::list<bt_swarm_full_t *> &	swarm_full_db	= bt_swarm()->get_full_db();
	size_t				cur_nb_give	= 0;
	// sanity check - the bt_swarm_t MUST be is_seed
	DBG_ASSERT( bt_swarm()->is_seed() );
	// count the number of bt_swarm_full_t in bt_reqauth_type_t::HOPE
	std::list<bt_swarm_full_t *>::iterator iter;
	for(iter = swarm_full_db.begin(); iter != swarm_full_db.end(); iter++){
		bt_swarm_full_sched_t * full_sched	= (*iter)->full_sched();
		switch( full_sched->reqauth_type().get_value() ){
		case bt_reqauth_type_t::HOPE:	cur_nb_give++;	break;
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
				, bt_peerpick_helper_t::pick_random_any);
}

/** \brief Update the peer selection
 */
void	bt_peerpick_plain_t::peerpick_update()				throw()
{
	// do the peer selection differently depending on the local peer is fulldata or partdata
	if( bt_swarm()->is_seed() ){
		// if this bt_swarm_t is seeding, select the peers by peer_select_update_seed()
		peer_select_update_seed();
	}else{
		// if this bt_swarm_t is leaching, select the peers by peer_select_update_seed()
		peer_select_update_leech();
	}
}

NEOIP_NAMESPACE_END





