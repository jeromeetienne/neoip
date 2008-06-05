/*! \file
    \brief Header of the \ref kad_http_t class

*/


#ifndef __NEOIP_ASYNCOP_HPP__ 
#define __NEOIP_ASYNCOP_HPP__ 
/* system include */
#include <glib.h>
/* local include */
#include "neoip_asyncop_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
struct asyncop_thread_ctx_t;

/** \brief To handle the service of asyncop
 */
class asyncop_t : NEOIP_COPY_CTOR_DENY {
private:
	asyncop_completed_cb_t *callback;	//!< the callback to notify on completion
	void *			userptr;	//!< the userptr associated with the callback
	
	asyncop_thread_ctx_t *	thread_ctx;	//!< pointer on the created thread_ctx, == NULL
						//!< when the thread is completed
	void			notify_completion(void *work_result)	throw();
public:
	/*************** ctor/dtor	***************************************/
	asyncop_t()	throw();
	~asyncop_t()	throw();
	
	/*************** setup function	***************************************/
	bool		start(asyncop_completed_cb_t *callback, void *userptr
				, asyncop_do_work_cb_t  do_work_cb
				, asyncop_free_work_cb_t free_work_cb = NULL )	throw();

// put the C callback for glib as friend	
friend	gpointer	asyncop_thread_glib_cb(gpointer data);	
friend	gboolean	asyncop_idle_glib_cb(gpointer data);
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ASYNCOP_HPP__  */



