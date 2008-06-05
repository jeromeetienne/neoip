/*! \file
    \brief Class to handle the bt_peerpick_casto_t

- TODO should i add stuff for the GIVE ? 
  - like to spread more data to the giver ?

\par Brief Description
bt_peerpick_vapi_t is handling the peer picking for the bt_swarm_sched_full_t.
bt_peerpick_casto_t is applying the peer picking policy of the 'casto' bittorrent.
  
\par About the policy
- it uses 4 differents bt_reqauth_type_t which are COOP/HOPE/FSTART/IDLE
- they are allocated in order, aka first all the ones in COOP are picked
  then all the HOPE (but only from the ones not yet picked) and so one
- the usual policy for 'leech in the plain' is to have 2 type of bt_reqauth_type_t
  - bt_reqauth_type_t::COOP for the connection with which there is a cooperation
  - bt_reqatuh_type_t::HOPE for the connection with which there is a hopefull attempt
    to establish a cooperation
- the bt_peerpick_casto_t reproduces this basic ideas and add some specific
  bt_reqauth_type_t to fit its specific needs
  - one needs is the fact that casto requires a quite constant download rate
    while the plain bittorrent has no such requirement
  - one needs is to reduce the initialization latency to avoid making the 
    user wait.
- it add the bt_reqauth_type_t::FSTART which is done to satisfy the 'lower
  init latency' requirement. in close relation with the bt_utmsg_fstart_t
- it add the bt_reqauth_type_t::IDLE to provide data to remote peer even
  if they dont have any piece wanted by the local peer.
  - but it has a low precedence. thus the remote peer in bt_reqauth_type_t::IDLE
    will consume bandwidth only if other remote peer dont consume it
  - it may be seen as acting as a seed when all piece got downloaded 
- the order of allocation is COOP/HOPE/FSTART/IDLE
  - COOP and HOPE are first in order to mimic the 'plain' policy as close as possible
  - FSTART comes next because 'lower init latency' impact directly the users
    experience so it is important. moreover the remote peer stays in FSTART only
    a limited amount of time.
  - IDLE is to allow download from peers which have no piece locally wanted but
    the local peer has bandwidht availlable.

*/

/* system include */
/* local include */
#include "neoip_bt_peerpick_casto.hpp"
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
bt_peerpick_casto_t::bt_peerpick_casto_t()	throw()
{
	// zero some variable
	m_bt_swarm	= NULL;
}

/** \brief Destructor
 */
bt_peerpick_casto_t::~bt_peerpick_casto_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_peerpick_casto_t &bt_peerpick_casto_t::profile(const bt_peerpick_casto_profile_t &m_profile)	throw()
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
bt_err_t	bt_peerpick_casto_t::start(bt_swarm_t *m_bt_swarm)	throw()
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
void	bt_peerpick_casto_t::peerpick_update()				throw()
{
	std::list<bt_swarm_full_t *> &	swarm_full_db	= bt_swarm()->get_full_db();
	size_t				cur_nb_coop	= 0;
	size_t				cur_nb_hope	= 0;
	size_t				cur_nb_fstart	= 0;
	size_t				cur_nb_idle	= 0;
	// count the number of bt_swarm_full_t per bt_reqauth_type_t
	std::list<bt_swarm_full_t *>::iterator iter;
	for(iter = swarm_full_db.begin(); iter != swarm_full_db.end(); iter++){
		bt_swarm_full_sched_t * full_sched	= (*iter)->full_sched();
		switch( full_sched->reqauth_type().get_value() ){
		case bt_reqauth_type_t::COOP:	cur_nb_coop++;		break;
		case bt_reqauth_type_t::HOPE:	cur_nb_hope++;		break;
		case bt_reqauth_type_t::FSTART:	cur_nb_fstart++;	break;
		case bt_reqauth_type_t::IDLE:	cur_nb_idle++;		break;
		default:	break;
		}
	}

	// sanity check - cur_nb_coop MUST BE <= profile().reqauth_coop_max()
	DBG_ASSERT( cur_nb_coop <= profile().reqauth_coop_max() );
	// select all the new needed remote peers in bt_reqauth_type_t::COOP
	size_t	nb_coop_2launch		= profile().reqauth_coop_max() - cur_nb_coop;
	bt_peerpick_helper_t::pick_many_cnx(bt_reqauth_type_t::COOP
				, profile().reqauth_coop_timeout()
				, profile().reqauth_coop_prec()
				, nb_coop_2launch, bt_swarm()
				, bt_peerpick_helper_t::pick_fastest_recv);

	// sanity check - cur_nb_hope MUST BE <= profile().reqauth_hope_max()
	DBG_ASSERT( cur_nb_hope <= profile().reqauth_hope_max() );
	// select all the new needed remote peers in bt_reqauth_type_t::HOPE
	size_t	nb_hope_2launch		= profile().reqauth_hope_max() - cur_nb_hope;
	bt_peerpick_helper_t::pick_many_cnx(bt_reqauth_type_t::HOPE
				, profile().reqauth_hope_timeout()
				, profile().reqauth_hope_prec()
				, nb_hope_2launch, bt_swarm()
				, bt_peerpick_helper_t::pick_random_any);

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





