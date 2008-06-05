/*! \file
    \brief Header of the \ref event_hook_reg_t
    
*/


#ifndef __NEOIP_EVENT_HOOK_REG_HPP__ 
#define __NEOIP_EVENT_HOOK_REG_HPP__ 
/* system include */
/* local include */
#include "neoip_event_hook_reg_cb.hpp"
#include "neoip_event_hook_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Handle the callback to be called when the lib_session_t exit
 */
class event_hook_reg_t : NEOIP_COPY_CTOR_DENY, private event_hook_cb_t {
private:
	event_hook_t *	m_event_hook;	//!< backpointer to the attache event_hook_t
	int		m_hook_level;	//!< the level of this hook

	/*************** event_hook callback	*******************************/
	bool		neoip_event_hook_notify_cb(void *userptr
						, const event_hook_t *cb_event_hook
						, int hook_level)		throw();
	/*************** callback stuff	***************************************/
	event_hook_reg_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback()			throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	event_hook_reg_t()				throw();
	~event_hook_reg_t()				throw();

	/*************** setup function	***************************************/ 
	void		start(int p_hook_level, event_hook_t *p_event_hook
				, event_hook_reg_cb_t *callback, void *userptr)	throw();

	/*************** query function	***************************************/
	event_hook_t *	event_hook()		const throw()	{ return m_event_hook;;	}
	int		hook_level()		const throw()	{ return m_hook_level;	}

	/*************** Action function	*******************************/
	void		begin_exiting()		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_EVENT_HOOK_REG_HPP__  */










