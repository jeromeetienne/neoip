/*! \file
    \brief Declaration of the upnp_portcleaner_t
    
*/


#ifndef __NEOIP_UPNP_PORTCLEANER_HPP__ 
#define __NEOIP_UPNP_PORTCLEANER_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_portcleaner_profile.hpp"
#include "neoip_upnp_portcleaner_wikidbg.hpp"
#include "neoip_upnp_portcleaner_item_arr.hpp"
#include "neoip_upnp_scanallport_cb.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_timeout.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	upnp_watch_t;
class	upnp_portdesc_arr_t;

/** \brief handle the listener of the nslan
 */
class upnp_portcleaner_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
				, private upnp_scanallport_cb_t
				, private wikidbg_obj_t<upnp_portcleaner_t, upnp_portcleaner_wikidbg_init>{
private:
	upnp_watch_t *			upnp_watch;	//!< backpointer to the upnp_watch_t
	upnp_portcleaner_profile_t	m_profile;	//!< the profile to use for this object
	upnp_portcleaner_item_arr_t	cur_db;		//!< the currrent database

	/*************** Internal function	*******************************/
	void		process_new_db(const upnp_portdesc_arr_t &new_db)	throw();

	/*************** upnp_call_addport_t timeout	***********************/
	delaygen_t	probe_delaygen;
	timeout_t	probe_timeout;
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** upnp_scanallport_t	*******************************/
	upnp_scanallport_t	*upnp_scanallport;
	bool 		neoip_upnp_scanallport_cb(void *cb_userptr, upnp_scanallport_t &cb_upnp_scanallport
						, const upnp_err_t &upnp_err)	throw();
public:
	/*************** ctor/dtor	***************************************/
	upnp_portcleaner_t()	throw();
	~upnp_portcleaner_t()	throw();

	/*************** setup function	***************************************/
	upnp_portcleaner_t&profile(const upnp_portcleaner_profile_t &a_profile)	throw();
	upnp_err_t	start(upnp_watch_t *upnp_watch)				throw();	

	/*************** query function	***************************************/
	const upnp_portcleaner_profile_t &profile()	const throw()	{ return m_profile;	}

	/*************** static function to build/parse tag_desc	*******/
	static	std::string	build_tag_desc(const std::string desc_orig, const delay_t &portcleaner_delay
					, const std::string &portcleaner_nonce)	throw();
	static	std::string	parse_tag_desc(const std::string desc_str, delay_t &portcleaner_lease
					, std::string &portcleaner_nonce)	throw();

	/*************** List of friend class	*******************************/
	friend class	upnp_portcleaner_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_UPNP_PORTCLEANER_HPP__ 



