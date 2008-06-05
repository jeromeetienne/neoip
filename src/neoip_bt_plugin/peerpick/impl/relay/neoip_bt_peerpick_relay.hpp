/*! \file
    \brief Header of the bt_peerpick_relay_t
    
*/


#ifndef __NEOIP_BT_PEERPICK_RELAY_HPP__ 
#define __NEOIP_BT_PEERPICK_RELAY_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_peerpick_relay_profile.hpp"
#include "neoip_bt_peerpick_vapi.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_err_t;

/** \brief Handle the scheduling for the bt_swarm_t connection
 */
class bt_peerpick_relay_t : NEOIP_COPY_CTOR_DENY, public bt_peerpick_vapi_t {
private:
	bt_swarm_t *			m_bt_swarm;	//!< backpointer to the linked bt_swarm_t
	bt_peerpick_relay_profile_t	m_profile;	//!< the profile to use
	/*************** peer selection	***************************************/
	void		peer_select_update()						throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_peerpick_relay_t() 		throw();
	~bt_peerpick_relay_t()		throw();

	/*************** Setup function	***************************************/
	bt_peerpick_relay_t&	profile(const bt_peerpick_relay_profile_t &profile)	throw();
	bt_err_t		start(bt_swarm_t *m_bt_swarm)				throw();

	/*************** Query function	***************************************/
	bt_swarm_t *		bt_swarm()		const throw()	{ return m_bt_swarm;	}
	const bt_peerpick_relay_profile_t &profile()	const throw()	{ return m_profile;	}

	/*************** bt_peerpick_vapi_t	*******************************/
	void		peerpick_update()	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERPICK_RELAY_HPP__  */



