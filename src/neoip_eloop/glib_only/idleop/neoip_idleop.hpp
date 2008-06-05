/*! \file
    \brief Header of the idleop_t
    
*/


#ifndef __NEOIP_IDLEOP_HPP__ 
#define __NEOIP_IDLEOP_HPP__ 
/* system include */
#include <glib.h>
/* local include */
#include "neoip_idleop_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for idleop
 */
class idleop_t : NEOIP_COPY_CTOR_DENY {
private:
	size_t		priority;
	guint		glib_tagid;
	
	/*************** callback stuff	***************************************/
	idleop_cb_t *		callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback()	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	idleop_t() 		throw();
	~idleop_t()		throw();

	/*************** Setup function	***************************************/
	idleop_t &	set_priority(size_t priority)			throw();
	bool		start(idleop_cb_t *callback, void *userptr)	throw();

	/*************** List of friend function	***********************/
	friend	gboolean	idleop_idle_glib_cb(gpointer data);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IDLEOP_HPP__  */



