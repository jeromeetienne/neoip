/*! \file
    \brief Declaration of the upnp_call_statusinfo_t
    
*/


#ifndef __NEOIP_UPNP_CALL_STATUSINFO_HPP__ 
#define __NEOIP_UPNP_CALL_STATUSINFO_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_call_statusinfo_cb.hpp"
#include "neoip_upnp_call_cb.hpp"
#include "neoip_delay.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	upnp_disc_res_t;
class	upnp_call_profile_t;

/** \brief handle the listener of the nslan
 */
class upnp_call_statusinfo_t : NEOIP_COPY_CTOR_DENY, private upnp_call_cb_t {
private:
	/*************** Replied fields	***************************************/
	std::string	m_cnx_status;	//!< the replied NewConnectionStatus
	std::string	m_lastcnx_err;	//!< the replied NewLastConnectionError
	delay_t		m_uptime_delay;	//!< the replied NewUptime

	/*************** upnp_call_t	***************************************/
	upnp_call_t *	upnp_call;
	bool 		neoip_upnp_call_cb(void *cb_userptr, upnp_call_t &cb_call, const upnp_err_t &upnp_err
					, const strvar_db_t &replied_var)	throw();

	/*************** callback stuff	***************************************/
	upnp_call_statusinfo_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const upnp_err_t &upnp_err)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	upnp_call_statusinfo_t()	throw();
	~upnp_call_statusinfo_t()	throw();

	/*************** setup function	***************************************/
	upnp_call_statusinfo_t &set_profile(const upnp_call_profile_t &profile)	throw();
	upnp_err_t	start(const upnp_disc_res_t &disc_res, upnp_call_statusinfo_cb_t *callback
							, void * userptr)	throw();

	/*************** query function	***************************************/
	const std::string &	cnx_status()	const throw()	{ return m_cnx_status;	}
	const std::string &	lastcnx_err()	const throw()	{ return m_lastcnx_err;	}
	const delay_t &		uptime_delay()	const throw()	{ return m_uptime_delay;}
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_UPNP_CALL_STATUSINFO_HPP__ 



