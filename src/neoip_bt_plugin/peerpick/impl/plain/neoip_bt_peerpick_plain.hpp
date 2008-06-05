/*! \file
    \brief Header of the bt_peerpick_plain_t
    
*/


#ifndef __NEOIP_BT_PEERPICK_PLAIN_HPP__ 
#define __NEOIP_BT_PEERPICK_PLAIN_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_peerpick_vapi.hpp"
#include "neoip_bt_peerpick_plain_profile.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_err_t;
class	bt_reqauth_type_t;
class	delay_t;

/** \brief Handle the scheduling for the bt_swarm_t connection
 */
class bt_peerpick_plain_t : NEOIP_COPY_CTOR_DENY, public bt_peerpick_vapi_t {
private:
	bt_swarm_t *		m_bt_swarm;	//!< backpointer to the linked bt_swarm_t
	bt_peerpick_plain_profile_t m_profile;	//!< the profile to use
	
	/*************** peer selection	***************************************/
	void		peer_select_update_leech()	throw();
	void		peer_select_update_seed()	throw();
	void		peer_select_update()		throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_peerpick_plain_t() 		throw();
	~bt_peerpick_plain_t()		throw();

	/*************** Setup function	***************************************/
	bt_peerpick_plain_t&	profile(const bt_peerpick_plain_profile_t &profile)	throw();
	bt_err_t	start(bt_swarm_t *m_bt_swarm)	throw();

	/*************** Query function	***************************************/
	const bt_peerpick_plain_profile_t &	profile()	const throw()	{ return m_profile;	}
	bt_swarm_t *				bt_swarm()	const throw()	{ return m_bt_swarm;	}

	/*************** bt_peerpick_vapi_t	*******************************/
	void		peerpick_update()	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERPICK_PLAIN_HPP__  */



