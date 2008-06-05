/*! \file
    \brief Header of the neoip_asyncop class
    
*/


#ifndef __NEOIP_ASYNCOP_CB_HPP__ 
#define __NEOIP_ASYNCOP_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class asyncop_t;

/** \brief Callback notified to do the work
 * 
 * - IMPORTANT: this function is NOT running in the 'main' thread
 * 
 * @return a pointer on the work_result if one is provided, NULL otherwise
 */
typedef void * (*asyncop_do_work_cb_t)(void *userptr);

/** \brief Callback notified to free a potential the work
 * 
 * - IMPORTANT: this function IS running in the 'main' thread
 */
typedef void (*asyncop_free_work_cb_t)(void *work_result, void *userptr);

	
/** \brief the callback class for asyncop_t
 */
class asyncop_completed_cb_t {
public:
	/** \brief callback notified by asyncop_t when the work is completed
	 * 
	 * - IMPORTANT: this one does run in the 'main thread' (the one who created the asyncop_t)
	 */
	virtual void neoip_asyncop_completed_cb(void *cb_userptr, asyncop_t &asyncop
							, void *work_result)	throw() = 0;
	virtual ~asyncop_completed_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ASYNCOP_CB_HPP__  */



