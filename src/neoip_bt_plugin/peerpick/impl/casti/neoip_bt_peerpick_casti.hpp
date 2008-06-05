/*! \file
    \brief Header of the bt_peerpick_casti_t
    
*/


#ifndef __NEOIP_BT_PEERPICK_CASTI_HPP__ 
#define __NEOIP_BT_PEERPICK_CASTI_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_peerpick_casti_profile.hpp"
#include "neoip_bt_peerpick_vapi.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_err_t;

/** \brief Handle the scheduling for the bt_swarm_t connection
 */
class bt_peerpick_casti_t : NEOIP_COPY_CTOR_DENY, public bt_peerpick_vapi_t {
private:
	bt_swarm_t *		m_bt_swarm;	//!< backpointer to the linked bt_swarm_t
	bt_peerpick_casti_profile_t m_profile;	//!< the profile to use
	double			m_cnx_minrate;	//!< the minimal rate per connection
public:
	/*************** ctor/dtor	***************************************/
	bt_peerpick_casti_t() 		throw();
	~bt_peerpick_casti_t()		throw();

	/*************** Setup function	***************************************/
	bt_peerpick_casti_t&	profile(const bt_peerpick_casti_profile_t &profile)	throw();
	bt_err_t	start(bt_swarm_t *m_bt_swarm)	throw();

	/*************** Query function	***************************************/
	const bt_peerpick_casti_profile_t &profile()	const throw()	{ return m_profile;	}
	bt_swarm_t *	bt_swarm()	const throw()	{ return m_bt_swarm;	}
	double		cnx_minrate()	const throw()	{ return m_cnx_minrate;	}
	
	/*************** action function	*******************************/
	void		cnx_minrate(double new_value)	throw()	{ m_cnx_minrate = new_value;	}

	/*************** bt_peerpick_vapi_t	*******************************/
	void		peerpick_update()	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERPICK_CASTI_HPP__  */



