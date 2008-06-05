/*! \file
    \brief Declaration of the upnp_bindport_t
    
*/


#ifndef __NEOIP_UPNP_BINDPORT_HPP__ 
#define __NEOIP_UPNP_BINDPORT_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_bindport_profile.hpp"
#include "neoip_upnp_bindport_wikidbg.hpp"
#include "neoip_upnp_bindport_cb.hpp"
#include "neoip_upnp_call_getport_cb.hpp"
#include "neoip_upnp_call_addport_cb.hpp"
#include "neoip_upnp_sockfam.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	upnp_watch_t;

/** \brief to bind a port in the upnp router via upnp_watch_t
 */
class upnp_bindport_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t, private upnp_call_getport_cb_t
				, private upnp_call_addport_cb_t
				, private wikidbg_obj_t<upnp_bindport_t, upnp_bindport_wikidbg_init>{
private:
	upnp_bindport_profile_t	profile;	//!< the profile to use for this object
	upnp_watch_t *		upnp_watch;	//!< backpointer to the upnp_watch_t
	ipport_addr_t		m_ipport_lview;	//!< the local view of the ipport_addr_t to bind
	ipport_addr_t		m_ipport_pview;	//!< the public view of the ipport_addr_t to bind
	upnp_sockfam_t		m_upnp_sockfam;	//!< the upnp_sockfam_t for this port
	std::string		description_str;//!< the description_str for the upnp_call_addport_t 
	bool			m_is_bound;	//!< true if the port has been successfully bound 

	/*************** Internal function	*******************************/
	upnp_err_t		launch_getport()	throw();
	ipport_addr_t		pick_random_pview()	const throw();

	/*************** upnp_getport_t	***************************************/
	size_t			getport_nbretry;//!< the number of getport which have been started
	upnp_call_getport_t *	call_getport;
	bool 		neoip_upnp_call_getport_cb(void *cb_userptr, upnp_call_getport_t &cb_call_getport
						, const upnp_err_t &upnp_err)	throw();

	/*************** timeout_t main callback	***********************/
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
							
	/*************** upnp_call_addport_t timeout	***********************/
	delaygen_t	addport_delaygen;
	timeout_t	addport_timeout;
	bool 		addport_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** upnp_addport_t	***************************************/
	upnp_call_addport_t *	call_addport;
	bool 		neoip_upnp_call_addport_cb(void *cb_userptr, upnp_call_addport_t &cb_call_addport
						, const upnp_err_t &upnp_err)	throw();
	size_t		addport_counter;

	/*************** itor_expire_timeout	*******************************/
	timeout_t	itor_expire_timeout;
	bool 		itor_expire_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** callback stuff	***************************************/
	upnp_bindport_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const upnp_err_t &upnp_err
					, const ipport_addr_t &ipport_pview)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	upnp_bindport_t()	throw();
	~upnp_bindport_t()	throw();

	/*************** setup function	***************************************/
	upnp_bindport_t&set_profile(const upnp_bindport_profile_t &profile)	throw();
	upnp_err_t	start(upnp_watch_t *upnp_watch, const ipport_addr_t &m_ipport_lview
				, const upnp_sockfam_t &m_upnp_sockfam, const ipport_addr_t &m_ipport_pview
				, const std::string &description_str
				, upnp_bindport_cb_t *callback, void * userptr)	throw();	

	/*************** query function	***************************************/
	const ipport_addr_t &	ipport_lview()	const throw()	{ return m_ipport_lview;	}
	const upnp_sockfam_t &	upnp_sockfam()	const throw()	{ return m_upnp_sockfam;	}
	const ipport_addr_t &	ipport_pview()	const throw()	{ return m_ipport_pview;	}
	bool			is_bound()	const throw()	{ return m_is_bound;		}

	/*************** List of friend class	*******************************/
	friend class	upnp_bindport_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_UPNP_BINDPORT_HPP__ 



