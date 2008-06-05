/*! \file
    \brief Declaration of the ndiag_aview_t
    
*/


#ifndef __NEOIP_NDIAG_AVIEW_HPP__ 
#define __NEOIP_NDIAG_AVIEW_HPP__ 
/* system include */
/* local include */
#include "neoip_ndiag_aview_cb.hpp"
#include "neoip_ndiag_aview_wikidbg.hpp"
#include "neoip_ipport_aview.hpp"
#include "neoip_upnp_sockfam.hpp"
#include "neoip_upnp_bindport_cb.hpp"
#include "neoip_event_hook_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ndiag_err_t;

/** \brief handle the ndiag_aview_t
 */
class ndiag_aview_t : NEOIP_COPY_CTOR_DENY, private upnp_bindport_cb_t, private event_hook_cb_t
					, private wikidbg_obj_t<ndiag_aview_t, ndiag_aview_wikidbg_init> {
private:
	ipport_aview_t	m_ipport_aview_init;
	upnp_sockfam_t	m_sockfam;
	ipport_addr_t	m_ipport_pview_curr;
	std::string	m_desc_str;

	/*************** Internal function	*******************************/
	bool		update_pview_curr()	throw();
	ipport_addr_t	generate_pview_curr()	throw();	

	/*************** upnp_bindport_t	*******************************/
	upnp_bindport_t*upnp_bindport;
	bool		neoip_upnp_bindport_cb(void *cb_userptr, upnp_bindport_t &cb_upnp_bindport
				, const upnp_err_t &upnp_err, const ipport_addr_t &ipport_pview)	throw();
	bool		is_bindport_needed()	const throw();
	ndiag_err_t	launch_upnp_bindport()	throw();

	/*************** callback for ndiag_watch_t hook	***************/
	bool		neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
								, int hook_level) throw();
								
	/*************** callback stuff	***************************************/
	ndiag_aview_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const ipport_addr_t &ipport_pview)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	ndiag_aview_t()	throw();
	~ndiag_aview_t()	throw();

	/*************** setup function	***************************************/
	ndiag_err_t	start(const ipport_aview_t &m_ipport_aview_init, const upnp_sockfam_t &m_sock_fam
				, const std::string &m_desc_str
				, ndiag_aview_cb_t *callback, void * userptr)	throw();
	
	/*************** query function	***************************************/
	const ipport_aview_t &	ipport_aview_init()	const throw()	{ return m_ipport_aview_init;	}
	const upnp_sockfam_t &	sockfam()		const throw()	{ return m_sockfam;		}
	const ipport_addr_t &	ipport_pview_curr()	const throw()	{ return m_ipport_pview_curr;	}
	const std::string &	desc_str()		const throw()	{ return m_desc_str;		}

	const ipport_addr_t &	ipport_pview()		const throw()	{ return ipport_pview_curr();	}
	const ipport_addr_t &	ipport_lview()		const throw()	{ return m_ipport_aview_init.lview();}

	/*************** List of friend function	***********************/
	friend	class	ndiag_aview_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_NDIAG_AVIEW_HPP__ 



