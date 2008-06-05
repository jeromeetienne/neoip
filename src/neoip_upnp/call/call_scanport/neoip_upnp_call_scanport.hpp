/*! \file
    \brief Declaration of the upnp_call_scanport_t
    
*/


#ifndef __NEOIP_UPNP_CALL_SCANPORT_HPP__ 
#define __NEOIP_UPNP_CALL_SCANPORT_HPP__ 
/* system include */
/* local include */
#include "neoip_upnp_call_scanport_cb.hpp"
#include "neoip_upnp_call_cb.hpp"
#include "neoip_upnp_portdesc.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	upnp_disc_res_t;
class	upnp_call_profile_t;

/** \brief handle the upnp_call_scanport_t
 */
class upnp_call_scanport_t : NEOIP_COPY_CTOR_DENY, private upnp_call_cb_t {
private:
	bool		m_revendian;	//!< true if the endianness MUST be reverse, false otherwise
	upnp_portdesc_t	m_portdesc;	//!< the replied port description
	
	/*************** upnp_call_t	***************************************/
	upnp_call_t *	upnp_call;
	bool 		neoip_upnp_call_cb(void *cb_userptr, upnp_call_t &cb_call, const upnp_err_t &upnp_err
					, const strvar_db_t &replied_var)	throw();

	/*************** callback stuff	***************************************/
	upnp_call_scanport_cb_t *callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const upnp_err_t &upnp_err)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	upnp_call_scanport_t()	throw();
	~upnp_call_scanport_t()	throw();

	/*************** setup function	***************************************/
	upnp_call_scanport_t &revendian(bool new_value) 	throw()	{ m_revendian = new_value; return *this;}
	upnp_call_scanport_t &set_profile(const upnp_call_profile_t &profile)	throw();	
	upnp_err_t	start(const upnp_disc_res_t &disc_res, uint32_t scanidx
				, upnp_call_scanport_cb_t *callback, void * userptr)	throw();

	/*************** query function	***************************************/
	const upnp_portdesc_t &	portdesc()	const throw()	{ return m_portdesc;	}
	bool			revendian()	const throw()	{ return m_revendian;	}		
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_UPNP_CALL_SCANPORT_HPP__ 



