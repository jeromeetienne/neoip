/*! \file
    \brief Header of the oload_mod_flv_t
    
*/


#ifndef __NEOIP_OLOAD_MOD_FLV_HPP__ 
#define __NEOIP_OLOAD_MOD_FLV_HPP__ 
/* system include */
/* local include */
#include "neoip_oload_mod_flv.hpp"
#include "neoip_oload_mod_vapi.hpp"
#include "neoip_oload_mod_type.hpp"
#include "neoip_bt_httpo_resp_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_datum.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	oload_apps_t;

/** \brief Handle the module 'flv' to deliver flv
 */
class oload_mod_flv_t : public oload_mod_vapi_t, NEOIP_COPY_CTOR_DENY 
				, private bt_httpo_resp_cb_t {
private:
	oload_apps_t *		m_oload_apps;
	oload_mod_type_t	m_type;

	/*************** bt_httpo_resp_cb_t	*******************************/
	bt_httpo_resp_t*	m_httpo_resp;
	bool			neoip_bt_httpo_resp_cb(void *cb_userptr, bt_httpo_resp_t &cb_bt_httpo_resp
						, const bt_httpo_event_t &httpo_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	oload_mod_flv_t() 		throw();
	~oload_mod_flv_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(oload_apps_t *m_oload_apps)	throw();

	/*************** query function	***************************************/
	oload_apps_t *	oload_apps()	const throw()	{ return m_oload_apps;	}

	/*************** oload_mod_vapi_t	*******************************/
	const oload_mod_type_t &	type()	const throw()	{ return m_type;	}
	bt_err_t	pre_itor_hook(bt_httpo_full_t *httpo_full)		const throw();
	datum_t		prefix_header(const oload_swarm_t *oload_swarm
					, const bt_httpo_full_t *httpo_full)	const throw();

	/*************** List of friend class	*******************************/
	friend class	oload_mod_flv_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_MOD_FLV_HPP__  */



