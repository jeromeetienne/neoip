/*! \file
    \brief Declaration of the upnp_scanallport_t
    
*/


#ifndef __NEOIP_UPNP_SCANALLPORT_HPP__ 
#define __NEOIP_UPNP_SCANALLPORT_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_scanallport_cb.hpp"
#include "neoip_upnp_disc_res.hpp"
#include "neoip_upnp_call_scanport_cb.hpp"
#include "neoip_upnp_portdesc_arr.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief handle the listener of the nslan
 */
class upnp_scanallport_t : NEOIP_COPY_CTOR_DENY, private upnp_call_scanport_cb_t {
private:
	upnp_disc_res_t		m_disc_res;
	upnp_portdesc_arr_t	m_portdesc_arr;
	size_t			scanidx;

	/*************** upnp_scanport_t	***************************************/
	upnp_call_scanport_t *	call_scanport;
	bool 			neoip_upnp_call_scanport_cb(void *cb_userptr
						, upnp_call_scanport_t &cb_call_scanport
						, const upnp_err_t &upnp_err)	throw();
	upnp_err_t		launch_next_scanport()				throw();
	
	/*************** callback stuff	***************************************/
	upnp_scanallport_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const upnp_err_t &upnp_err)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	upnp_scanallport_t()	throw();
	~upnp_scanallport_t()	throw();

	/*************** setup function	***************************************/
	upnp_err_t		start(const upnp_disc_res_t &m_disc_res
					, upnp_scanallport_cb_t *callback, void * userptr)throw();
						
	/*************** query function	***************************************/
	const upnp_disc_res_t &		disc_res()	const throw()	{ return m_disc_res;	}
	const upnp_portdesc_arr_t &	portdesc_arr()	const throw()	{ return m_portdesc_arr;}
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_UPNP_SCANALLPORT_HPP__ 



