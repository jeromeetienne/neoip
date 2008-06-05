/*! \file
    \brief Header of the fdwatch_t
    
*/


#ifndef __NEOIP_FDWATCH_CB_HPP__ 
#define __NEOIP_FDWATCH_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class fdwatch_t;
class fdwatch_cond_t;

/** \brief the callback class for fdwatch_t
 */
class fdwatch_cb_t {
public:
	/** \brief callback to notify when the fdwatch_t has events to report
	 * 
	 * @return return false object has been deleted
	 */
	virtual bool neoip_fdwatch_cb(void *userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond) throw() = 0;
	virtual ~fdwatch_cb_t() {};
};

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_FDWATCH_CB_HPP__  */



