/*! \file
    \brief Definition of the \ref asyncop_t class

\par PERFORMANCE COST
- this object init the gthread and once it is initialized the main loop event
  will go pthread_mutex_lock/unlock a LOT which produce a huge performance cost
  even when no thread are running
  - for example on transfering data, it spent 60% of the cpu only on the mutex!!!!

\par Introduction
This object allows some simple asynchronous operation to be performed in 
thread. it uses 2 callbacks:
-# asyncop_do_work_cb_t() which perform the work itself and IMPORTANT is
   running in the created thread. This callback returns a pointer that 
   may be used for returning a 'work_result'
   - as it is NOT running in the 'main' thread, GREAT CARE should be taken
     when coding this function in order not to use structure which are used
     in the 'main' thread too. it would create some very hard to debug 
     race conditions.
-# asyncop_free_work_cb_t() which is used to free the 'work_result'
   IMPORTANT: it is running in the 'main' thread.

\par Implementation notes
- it is impossible to kill a gthread
  - this implies a quite unusual struct for this object
- there is a kind of 2 stages context
  - asyncop_t is the object created/destroyed by the caller when it want to
  - asyncop_thread_ctx_t which is created by the asyncop_t::start()
    and destroyed by asyncop_idle_glib_cb()
- if the asyncop_t is deleted before the thread completion:
  - asyncop_thread_ctx_t->asyncop is set to NULL to warn the thread not to notify
    the asyncop_t on completion
- if the asyncop_t is not yet deleted when the thread completes
  - the asyncop_t::notify_completion() is called and notify the caller

- there is a small trick to communicate the completion between the created thread
  , called the 'secondary' thread, and the 'main' thread.
  - g_idle_add() is used in the secondary thread to add a idle task in the main
    thread. and this idle task is doing the notification work for asyncop_thread_ctx_t
- in fact there is only one function running in the 'secondary' thread, it 
  is asyncop_thread_glib_cb()
  

*/

/* system include */
/* local include */
#include "neoip_asyncop.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief the struct passed to the secondary thread
 */
struct asyncop_thread_ctx_t {
	asyncop_do_work_cb_t	do_work_cb;	//!< pointer on the callback to do the work
						//!< called every time - so potentially after
						//!< the asyncop_t has been deleted.
	asyncop_free_work_cb_t 	free_work_cb;	//!< pointer on the callback to do the work
						//!< called every time - so potentially after
						//!< the asyncop_t has been deleted.
	void *			userptr;	//!< the userptr provided by the caller
	void *			result_ptr;	//!< pointer on the result provided by do_work_cb
	asyncop_t *		asyncop;	//!< pointer on the asyncop_t which created this
						//!< this asyncop_thread_ctx_t. if == NULL
						//!< the creator asyncop_t has been deleted.
};

gboolean	asyncop_idle_glib_cb(gpointer data);
gpointer	asyncop_thread_glib_cb(gpointer data);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
asyncop_t::asyncop_t()	throw()
{
	thread_ctx	= NULL;
}

/** \brief Destructor
 */
asyncop_t::~asyncop_t()	throw()
{
	// if the thread_ctx is still running, mark the thread_ctx not to notify asyncop_t
	if( thread_ctx )	thread_ctx->asyncop	= NULL;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    START function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 * 
 * @return true on error, false otherwise
 */
bool	asyncop_t::start(asyncop_completed_cb_t *callback, void *userptr
				, asyncop_do_work_cb_t do_work_cb
				, asyncop_free_work_cb_t free_work_cb )	throw()
{
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;

	// allocate the thread_ctx - it gonna be freed in t
	thread_ctx	= nipmem_new asyncop_thread_ctx_t();
	// fill the struct
	thread_ctx->do_work_cb	= do_work_cb;
	thread_ctx->free_work_cb= free_work_cb;
	thread_ctx->result_ptr	= NULL;
	thread_ctx->userptr	= userptr;
	thread_ctx->asyncop	= this;

	// initialize glib thread if needed
	if(!g_thread_supported ())	g_thread_init (NULL);
	// create the thread
	GError *gerr	= NULL;
	GThread*thread	= g_thread_create(asyncop_thread_glib_cb, thread_ctx, TRUE, &gerr);
	if( gerr != NULL ){
		KLOG_ERR("can't create a thread for asyncop_t due to gerr=" << gerr->message);
		g_error_free(gerr);
		return true;
	}
	// sanity check - the thread MUST be non NULL
	DBG_ASSERT( thread );

	// return no error
	return false;
}

/** \brief callback called when asyncop_t is not yet deleted when the thread is completed
 * 
 * - NOTE: this callback run in the 'main' thread
 */
void asyncop_t::notify_completion(void *work_result)	throw()
{
	// mark the notification as done
	thread_ctx = NULL;	
	// notify the asyncop_completed_cb_t
	// NOTE: this may delete the asyncop_t object
	callback->neoip_asyncop_completed_cb(userptr, *this, work_result);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      thread function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback of g_thread_create() used by glib 
 * 
 * - IMPORTANT it is running in the 'secondary' thread
 *   - it is the only function which run the 'secondary' thread
 */
gpointer asyncop_thread_glib_cb(gpointer data)
{
	// get the thread_ctx - NOTE: it is now owned by the thread
	asyncop_thread_ctx_t *thread_ctx	= (asyncop_thread_ctx_t *)data;
	// call the work_cb
	thread_ctx->result_ptr	= thread_ctx->do_work_cb( thread_ctx->userptr );
	// warn the main thread that the asyncop_t is completed
	// - use g_idle_add() as its callback will be called in the 'main' thread
	g_idle_add(asyncop_idle_glib_cb, thread_ctx);
	// dont return any value for g_thread_join()
	return NULL;
}

/** \brief callback of g_idle_add() used by glib
 * 
 * - IMPORTANT it is running in the 'main' thread
 */
gboolean	asyncop_idle_glib_cb(gpointer data)
{
	asyncop_thread_ctx_t *thread_ctx	= (asyncop_thread_ctx_t *)data;
	// notify the asyncopt of the completion if it isnt yet deleted
	if( thread_ctx->asyncop )
		thread_ctx->asyncop->notify_completion(thread_ctx->result_ptr);
	// call the free_cb if needed
	// - it may not be needed if the user didnt provide a asyncop_free_work_cb_t
	if( thread_ctx->free_work_cb )
		thread_ctx->free_work_cb(thread_ctx->result_ptr, thread_ctx->userptr);
	// free the thread_ctx which has been allocated in the asyncop_t::start()
	nipmem_delete thread_ctx;
	// return a 'dontkeep'
	return FALSE;
}

NEOIP_NAMESPACE_END

