/*! \file
    \brief Declaration of the ntudp_npos_watch_t
    
*/


#ifndef __NEOIP_NTUDP_NPOS_WATCH_HPP__ 
#define __NEOIP_NTUDP_NPOS_WATCH_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_npos_watch_cb.hpp"
#include "neoip_ntudp_npos_watch_profile.hpp"
#include "neoip_ntudp_npos_watch_wikidbg.hpp"
#include "neoip_ntudp_npos_res.hpp"
#include "neoip_ntudp_npos_eval_cb.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_peer_t;

/** \brief Perform a upd request and notify the result
 */
class ntudp_npos_watch_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t, private ntudp_npos_eval_cb_t
			, private wikidbg_obj_t<ntudp_npos_watch_t, ntudp_npos_watch_wikidbg_init>
			{
private:
	/*************** internal data	***************************************/
	ntudp_peer_t *			ntudp_peer;	//!< backpointer on the attached ntudp_peer_t
	ntudp_npos_res_t		m_npos_res;	//!< the current ntudp_npos_res_t
	ntudp_npos_watch_profile_t	profile;	//!< the profile attached to this object

	/***************  timeout	***************************************/
	delaygen_t		delaygen;	//!< the delay_t generator between the ntudp_npos_eval_t
	timeout_t		timeout;	//!< to trigger the ntudp_npos_eval_t
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** ntudp_npos_eval_t	*******************************/
	ntudp_npos_eval_t *	npos_eval;	//!< pointer on the running ntudp_npos_eval_t (if NULL
						//!< it is not running).
	bool neoip_ntudp_npos_eval_cb(void *cb_userptr, ntudp_npos_eval_t &cb_ntudp_npos_eval
					, const ntudp_npos_res_t &ntudp_npos_res)	throw();

	/*************** Callback	***************************************/
	ntudp_npos_watch_cb_t*	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const ntudp_npos_res_t &ntudp_npos_res)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	ntudp_npos_watch_t()		throw();
	~ntudp_npos_watch_t()		throw();

	/*************** Setup function	***************************************/
	ntudp_npos_watch_t &	set_profile(const ntudp_npos_watch_profile_t &profile)	throw();
	ntudp_err_t	start(ntudp_peer_t *ntudp_peer, ntudp_npos_watch_cb_t *callback
								, void * userptr)	throw();

	/*************** query function	***************************************/
	const ntudp_npos_res_t & npos_res()	const throw() { return m_npos_res;	}

	/*************** action function	*******************************/
	void		trigger_eval_asap()	throw();

	/*************** List of friend classes	*******************************/
	friend class	ntudp_npos_watch_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_WATCH_HPP__  */



