/*! \file
    \brief Declaration of the upnp_watch_t
    
*/


#ifndef __NEOIP_UPNP_WATCH_HPP__ 
#define __NEOIP_UPNP_WATCH_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_watch_wikidbg.hpp"
#include "neoip_upnp_watch_profile.hpp"
#include "neoip_upnp_watch_cb.hpp"
#include "neoip_upnp_disc_cb.hpp"
#include "neoip_upnp_disc_res.hpp"
#include "neoip_upnp_call_delport_cb.hpp"
#include "neoip_upnp_call_extipaddr_cb.hpp"
#include "neoip_lib_session_exit_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	upnp_err_t;
class	upnp_call_t;
class	upnp_call_delport_t;
class	upnp_bindport_t;
class	upnp_sockfam_t;
class	upnp_portcleaner_t;

/** \brief handle the client of upnp
 */
class upnp_watch_t : NEOIP_COPY_CTOR_DENY, private upnp_disc_cb_t
				, private upnp_call_extipaddr_cb_t
				, private upnp_call_delport_cb_t
				, private timeout_cb_t, private lib_session_exit_cb_t
				, private wikidbg_obj_t<upnp_watch_t, upnp_watch_wikidbg_init>{
private:
	upnp_watch_profile_t	profile;
	upnp_disc_res_t		m_current_disc_res;
	ip_addr_t		m_extipaddr;
	upnp_portcleaner_t *	upnp_portcleaner;
			
	/*************** main timeout_t callback	***********************/
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
					
	/*************** upnp_disc_t timeout	*******************************/
	delaygen_t	disc_delaygen;
	timeout_t	disc_timeout;
	bool 		disc_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** upnp_disc_t itself	*******************************/
	upnp_disc_t *	upnp_disc;
	bool 		neoip_upnp_disc_cb(void *cb_userptr, upnp_disc_t &cb_disc, const upnp_err_t &upnp_err
						, const upnp_disc_res_t &upnp_disc_res)	throw();	

	/*************** upnp_call_extipaddr_t timeout	***********************/
	delaygen_t	extipaddr_delaygen;
	timeout_t	extipaddr_timeout;
	bool 		extipaddr_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();
	/*************** upnp_call_extipaddr_t	*******************************/
	upnp_call_extipaddr_t *	call_extipaddr;					
	bool 		neoip_upnp_call_extipaddr_cb(void *cb_userptr, upnp_call_extipaddr_t &cb_call_extipaddr
					, const upnp_err_t &upnp_err
					, const ip_addr_t &extipaddr)			throw();	

							
	/*************** call_delport_db	***************************************/
	std::list<upnp_call_delport_t *>	call_delport_db;
	bool		neoip_upnp_call_delport_cb(void *cb_userptr, upnp_call_delport_t &cb_call_delport
						, const upnp_err_t &upnp_err)	throw();
	upnp_err_t	launch_delport(uint16_t port_pview, const upnp_sockfam_t &sock_fam
					, const upnp_disc_res_t &disc_res_4del)	throw();

	/*************** bindport_db	***************************************/
	std::list<upnp_bindport_t *>	bindport_db;
	void bindport_dolink(upnp_bindport_t *bindport)	throw()	{ bindport_db.push_back(bindport);	}
	void bindport_unlink(upnp_bindport_t *bindport)	throw()	{ bindport_db.remove(bindport);		}

	/*************** lib_session_exit_t	*******************************/
	lib_session_exit_t *	lib_session_exit;
	bool			neoip_lib_session_exit_cb(void *cb_userptr
						, lib_session_exit_t &session_exit)	throw();

	/*************** callback stuff	***************************************/
	upnp_watch_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const upnp_watch_event_t &watch_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	upnp_watch_t()		throw();
	~upnp_watch_t()		throw();

	/*************** setup function	***************************************/
	upnp_watch_t&	set_profile(const upnp_watch_profile_t &profile)	throw();
	upnp_err_t	start(upnp_watch_cb_t *callback, void * userptr)	throw();
	
	/*************** query function	***************************************/
	const upnp_watch_profile_t &get_profile()	const throw()	{ return profile;		}
	bool			upnp_isavail()		const throw()	{ return !upnp_unavail();	}
	bool			upnp_unavail()		const throw()	{ return current_disc_res().is_null();	}
	const upnp_disc_res_t &	current_disc_res()	const throw()	{ return m_current_disc_res;	}
	const ip_addr_t &	extipaddr()		const throw()	{ return m_extipaddr;		}
	
	/*************** list of friend class	*******************************/
	friend class	upnp_watch_wikidbg_t;
	friend class	upnp_bindport_t;
	friend class	upnp_portcleaner_t;
	friend class	upnp_getportendian_test_t;
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_UPNP_WATCH_HPP__ 



