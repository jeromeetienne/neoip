/*! \file
    \brief Header of the \ref lib_session_exit_t
    
*/


#ifndef __NEOIP_LIB_SESSION_EXIT_HPP__ 
#define __NEOIP_LIB_SESSION_EXIT_HPP__ 
/* system include */
/* local include */
#include "neoip_lib_session_exit_cb.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	lib_session_t;

/** \brief Handle the callback to be called when the lib_session_t exit
 */
class lib_session_exit_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default exit_order for bt_swarm_t - aka inside an apps
	static const size_t	EXIT_ORDER_BT_SWARM;
	//! the default exit_order for the apps - aka just before shutting down the library
	static const size_t	EXIT_ORDER_APPS;
	//! the default exit_order for upnp_watch_t - aka inside the library
	static const size_t	EXIT_ORDER_UPNP_WATCH;
private:
	lib_session_t *	lib_session;	//!< backpointer to the attached lib_session_t
	bool		m_in_exiting;	//!< true if begin_exiting() has been called
	bool		m_is_notified;	//!< true if the caller has been notified already
	size_t		m_exit_order;	//!< the exit_order of this lib_session_exit_t 

	/*************** exit_zerotimer	***************************************/
	zerotimer_t	zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** callback stuff	***************************************/
	lib_session_exit_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback()	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	lib_session_exit_t()				throw();
	~lib_session_exit_t()				throw();

	/*************** setup function	***************************************/ 
	void		start(lib_session_t *lib_session, size_t _exit_order
				, lib_session_exit_cb_t *callback, void *userptr)	throw();

	/*************** query function	***************************************/
	bool		in_exiting()		const throw()	{ return m_in_exiting;	}
	bool		is_notified()		const throw()	{ return m_is_notified;	}
	size_t		exit_order()		const throw()	{ return m_exit_order;	}

	/*************** Action function	*******************************/
	void		begin_exiting()		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LIB_SESSION_EXIT_HPP__  */










