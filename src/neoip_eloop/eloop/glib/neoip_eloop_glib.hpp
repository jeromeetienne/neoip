/*! \file
    \brief Header of the eloop_t
    
*/


#ifndef __NEOIP_ELOOP_GLIB_HPP__ 
#define __NEOIP_ELOOP_GLIB_HPP__ 
/* system include */
#include <glib.h>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

#ifndef USE_ELOOP_GLIB
#	error	"including a .hpp from a improper eloop implementation"
#endif

NEOIP_NAMESPACE_BEGIN;

/** \brief class definition for eloop_t
 */
class eloop_t : NEOIP_COPY_CTOR_DENY {
private:
	GMainLoop *	glib_mainloop;		//!< the glib loop stuff
public:
	/*************** ctor/dtor	***************************************/
	eloop_t()			throw();
	~eloop_t()			throw();
	
	/*************** Action function	*******************************/
	void		loop_run()	throw();
	void		loop_stop()	throw();
};

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_ELOOP_GLIB_HPP__  */



