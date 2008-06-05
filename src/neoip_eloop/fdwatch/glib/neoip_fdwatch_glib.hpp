/*! \file
    \brief Header of the fdwatch_t
    
*/


#ifndef __NEOIP_FDWATCH_GLIB_HPP__ 
#define __NEOIP_FDWATCH_GLIB_HPP__ 
/* system include */
#include <iostream>
#include <string>
#include <glib.h>
/* local include */
#include "neoip_fdwatch_cb.hpp"
#include "neoip_fdwatch_cond.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

#ifndef USE_ELOOP_GLIB
#	error	"including a .hpp from a improper eloop implementation"
#endif

NEOIP_NAMESPACE_BEGIN;

/** \brief class definition for fdwatch_t
 */
class fdwatch_t : NEOIP_COPY_CTOR_DENY {
public:	/////////////// constant definition	////////////////////////////////
	static const fdwatch_cond_t NONE;	//!< the default fdwatch_cond_t
	static const fdwatch_cond_t INPUT;	//!< to watch the input on this fd
	static const fdwatch_cond_t OUTPUT;	//!< to watch the output on this fd
	static const fdwatch_cond_t ERROR;	//!< to watch the error on this fd
private:
	fdwatch_cond_t	cur_cond;	//!< the current fdwatch_cond_t

	/*************** glib specific	***************************************/
	GIOChannel *	glib_iochannel;
	guint		glib_iowatch;

	/*************** callback stuff	***************************************/
	fdwatch_cb_t *	callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const fdwatch_cond_t &cond)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	fdwatch_t()	throw();
	~fdwatch_t()	throw();

	/*************** setup function	***************************************/
	fdwatch_t &	start(int fd, const fdwatch_cond_t &cond, fdwatch_cb_t * callback
							, void *userptr)	throw();

	/*************** query function	***************************************/
	const fdwatch_cond_t &	cond()		const throw()	{ return cur_cond;	}
	int			get_fd()	const throw();
	
	/*************** action function	*******************************/
	fdwatch_t &		change_callback(fdwatch_cb_t * callback, void *userptr)	throw();
	fdwatch_t &		cond(const fdwatch_cond_t &cond)			throw();
	
	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const fdwatch_t &fdwatch)	throw()
						{ return oss << fdwatch.to_string();	}	

	/*************** list of friend function	***********************/
	friend	gboolean fdwatch_glib_cb( GIOChannel *source, GIOCondition glib_cond, gpointer userptr)	throw();
};

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_FDWATCH_GLIB_HPP__  */



