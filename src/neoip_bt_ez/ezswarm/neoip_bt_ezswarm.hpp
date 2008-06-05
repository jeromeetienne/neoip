/*! \file
    \brief Header of the bt_ezswarm_t
    
*/


#ifndef __NEOIP_BT_EZSWARM_HPP__ 
#define __NEOIP_BT_EZSWARM_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_ezswarm_cb.hpp"
#include "neoip_bt_ezswarm_profile.hpp"
#include "neoip_bt_ezswarm_wikidbg.hpp"
#include "neoip_bt_ezswarm_state.hpp"
#include "neoip_bt_ezswarm_opt.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_swarm_resumedata.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declarations
class	bt_ezsession_t;
class	bt_err_t;
class	bt_io_vapi_t;
class	bt_ezswarm_alloc_t;
class	bt_ezswarm_check_t;
class	bt_ezswarm_share_t;
class	bt_ezswarm_stopping_t;
class	bt_ezswarm_stopped_t;
class	bt_ezswarm_error_t;
class	ipport_addr_t;

/** \brief class definition for bt_ezswarm_t
 */
class bt_ezswarm_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t
		, private wikidbg_obj_t<bt_ezswarm_t, bt_ezswarm_wikidbg_init> {
// TODO to make a duplication of bt_ezswarm_state_t here would make a lot of code
// easier to read by using bt_ezswarm_t::STOPPED and not bt_ezswarm_state_t::STOPPED
// - this may trigger a problem with the static ctor order 
private:
	bt_ezsession_t *	m_ezsession;	//!< bt_ezsession_t attached by this bt_ezswarm_t
	bt_ezswarm_profile_t	m_profile;
	bt_ezswarm_opt_t	m_opt;
	bt_ezswarm_state_t	m_cur_state;	//!< the current state of this bt_ezswarm_t
	bt_ezswarm_state_t	m_end_state;	//!< the end state of this bt_ezswarm_t
	bt_swarm_resumedata_t	m_swarm_resumedata;

	/*************** store the stateobj for each state	***************/
	bt_ezswarm_alloc_t *	ezswarm_alloc;
	bt_ezswarm_check_t *	ezswarm_check;
	bt_ezswarm_share_t *	ezswarm_share;
	bt_ezswarm_stopping_t *	ezswarm_stopping;
	bt_ezswarm_stopped_t *	ezswarm_stopped;
	bt_ezswarm_error_t *	ezswarm_error;
	bool			stateobj_notify_failure(const bt_err_t &bt_err)	throw();
	bool			stateobj_notify_success()			throw();

	bt_ezswarm_state_t	cpu_new_state(const bt_ezswarm_state_t &asked_state)	throw();
	bt_err_t		cur_stateobj_ctor()					throw();
	void			cur_stateobj_dtor()					throw();

	/*************** zerotimer_t	***************************************/
	zerotimer_t		next_state_zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw();

	/*************** bt_io_vapi_t	***************************************/
	bt_io_vapi_t *		m_io_vapi;
	bt_err_t		bt_io_vapi_ctor()	throw();

	/*************** internal function	*******************************/
	bool			change_state_now(const bt_ezswarm_state_t &new_state)	throw();

	/*************** callback stuff	***************************************/
	bt_ezswarm_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_ezswarm_event_t &event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_ezswarm_t() 		throw();
	~bt_ezswarm_t()		throw();
	
	/*************** setup function	***************************************/
	bt_ezswarm_t &	set_profile(const bt_ezswarm_profile_t &profile)	throw();
	bt_err_t	start(const bt_swarm_resumedata_t &swarm_resumedata
						, const bt_ezswarm_opt_t &ezswarm_opt
						, bt_ezsession_t *bt_ezsession
						, bt_ezswarm_cb_t *callback, void *userptr)	throw();
	/*************** query function	***************************************/
	const bt_mfile_t &		mfile()		const throw()	{ return swarm_resumedata().bt_mfile();	}
	const bt_swarm_resumedata_t &	swarm_resumedata()const throw()	{ return m_swarm_resumedata;	}
	const bt_ezswarm_state_t &	cur_state()	const throw()	{ return m_cur_state;		}
	const bt_ezswarm_state_t &	end_state()	const throw()	{ return m_end_state;		}
	const bt_ezswarm_profile_t &	profile()	const throw()	{ return m_profile;		}
	bt_ezsession_t *		ezsession()	const throw()	{ return m_ezsession;		}
	bt_io_vapi_t *			io_vapi()	const throw()	{ return m_io_vapi;		}
	const bt_ezswarm_opt_t &	opt()		const throw()	{ return m_opt;			}
	
	/*************** query for specific state object	***************/
	bt_ezswarm_alloc_t *		alloc()		throw()		{ return ezswarm_alloc;		}
	bt_ezswarm_check_t *		check()		throw()		{ return ezswarm_check;		}
	bt_ezswarm_share_t *		share()		throw()		{ return ezswarm_share;		}
	bt_ezswarm_stopping_t *		stopping()	throw()		{ return ezswarm_stopping;	}
	bt_ezswarm_stopped_t *		stopped()	throw()		{ return ezswarm_stopped;	}
	bt_ezswarm_error_t *		error()		throw()		{ return ezswarm_error;		}

	/*************** query shortcut for current_state()	***************/
	bool				in_alloc()	const throw()	{ return ezswarm_alloc;		}
	bool				in_check()	const throw()	{ return ezswarm_check;		}
	bool				in_share()	const throw()	{ return ezswarm_share;		}
	bool				in_stopping()	const throw()	{ return ezswarm_stopping;	}
	bool				in_stopped()	const throw()	{ return ezswarm_stopped;	}
	bool				in_error()	const throw()	{ return ezswarm_error;		}
	
	/*************** action function	*******************************/
	void		graceful_change_state(const bt_ezswarm_state_t &new_state)	throw();
	void		nextiter_change_state(const bt_ezswarm_state_t &new_state)	throw();
	void		update_listen_pview(const ipport_addr_t &new_listen_pview)	throw();
	
	/*************** list of friend class	*******************************/
	friend class	bt_ezswarm_wikidbg_t;
	friend class	bt_ezswarm_alloc_t;
	friend class	bt_ezswarm_check_t;
	friend class	bt_ezswarm_share_t;
	friend class	bt_ezswarm_stopping_t;
	friend class	bt_ezswarm_stopped_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_HPP__  */



