/*! \file
    \brief Header of the \ref upnp_apps_t
    
*/


#ifndef __NEOIP_UPNP_APPS_HPP__ 
#define __NEOIP_UPNP_APPS_HPP__ 

/* system include */
/* local include */
#include "neoip_upnp_err.hpp"
#include "neoip_upnp_call_statusinfo_cb.hpp"
#include "neoip_upnp_scanallport_cb.hpp"
#include "neoip_event_hook_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	clineopt_arr_t;
class	upnp_portdesc_arr_t;

/** \brief Handle the http offload capability
 */
class upnp_apps_t : NEOIP_COPY_CTOR_DENY, private event_hook_cb_t
				, private upnp_call_statusinfo_cb_t 
				, private upnp_scanallport_cb_t {
private:
	/*************** display function	*******************************/
	void	disp_initial()								throw();
	void	disp_statusinfo_success()						throw();
	void	disp_statusinfo_failure(const upnp_err_t &upnp_err)			throw();
	void	disp_scanallport_success(const upnp_portdesc_arr_t &portdesc_arr)	throw();
	void	disp_scanallport_failure(const upnp_err_t &upnp_err)			throw();
	void	disp_upnp_isavail()							throw();
	void	disp_upnp_unavail()							throw();
	void	disp_ipaddr_pview_changed()						throw();

	/*************** callback for ndiag_watch_t hook	***************/
	bool		neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook
							, int hook_level)	throw();

	/*************** upnp_call_statusinfo_t	*******************************/
	upnp_call_statusinfo_t	*call_statusinfo;
	bool 		neoip_upnp_call_statusinfo_cb(void *cb_userptr, upnp_call_statusinfo_t &cb_call_statusinfo
						, const upnp_err_t &upnp_err)	throw();
	void		statusinfo_ctor()		throw();
	void		statusinfo_dtor()		throw();

	/*************** upnp_scanallport_t	*******************************/
	upnp_scanallport_t	*upnp_scanallport;
	bool 		neoip_upnp_scanallport_cb(void *cb_userptr, upnp_scanallport_t &cb_upnp_scanallport
						, const upnp_err_t &upnp_err)	throw();
	void		scanallport_ctor()		throw();
	void		scanallport_dtor()		throw();
public:
	/*************** ctor/dtor	***************************************/
	upnp_apps_t()		throw();
	~upnp_apps_t()		throw();
	
	/*************** Setup function	***************************************/
	upnp_err_t		start()			throw();
	
	/*************** Query function	***************************************/
	static clineopt_arr_t	clineopt_arr()		throw();

};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_APPS_HPP__  */










