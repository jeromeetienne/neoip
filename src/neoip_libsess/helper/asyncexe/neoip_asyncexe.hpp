/*! \file
    \brief Header of the \ref kad_http_t class

*/


#ifndef __NEOIP_ASYNCEXE_HPP__ 
#define __NEOIP_ASYNCEXE_HPP__ 
/* system include */
/* local include */
#include "neoip_asyncexe_cb.hpp"
#include "neoip_asyncexe_profile.hpp"
#include "neoip_asyncexe_wikidbg.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_timeout.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	libsess_err_t;

/** \brief to excecute a executable asynchronously (via fork()) get back the result
 */
class asyncexe_t : NEOIP_COPY_CTOR_DENY, public fdwatch_cb_t, private timeout_cb_t, private zerotimer_cb_t
					, private wikidbg_obj_t<asyncexe_t, asyncexe_wikidbg_init>{
private:
	asyncexe_profile_t	profile;	//!< the current profile for this object
	item_arr_t<std::string>	m_cmdline_arr;
	pid_t			childpid;
	/*************** zerotimer_t	***************************************/
	zerotimer_t	zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** fdwatch_t main callback	***********************/
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond)	throw();

	/*************** stdout stuff	***************************************/
	bytearray_t	stdout_barray;
	fdwatch_t *	stdout_fdwatch;
	bool		stdout_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond)	throw();

	/*************** timeout for the query expiration	***************/
	timeout_t	expire_timeout;
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** callback stuff	***************************************/
	asyncexe_cb_t *	callback;	//!< the callback to notify on completion
	void *		userptr;	//!< the userptr associated with the callback
	bool		notify_callback_err(const libsess_err_t &libsess_err)	throw();
	bool		notify_callback(const libsess_err_t &libsess_err, const bytearray_t &stdout_barray
					, const int &exit_status)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	asyncexe_t()	throw();
	~asyncexe_t()	throw();
	
	/*************** setup function	***************************************/
	asyncexe_t &	set_profile(const asyncexe_profile_t &profile)		throw();
	libsess_err_t	start(const item_arr_t<std::string> &m_cmdline_arr
					, asyncexe_cb_t *callback, void *userptr)	throw();
	
	/*************** query function	***************************************/
	const item_arr_t<std::string> &	cmdline_arr()	const throw()	{ return m_cmdline_arr;	}


	/*************** List of friend class	*******************************/
	friend class	asyncexe_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ASYNCEXE_HPP__  */


 
