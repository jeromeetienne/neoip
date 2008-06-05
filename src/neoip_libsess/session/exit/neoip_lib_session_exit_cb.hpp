/*! \file
    \brief Declaration of the lib_session_exit_t
    
*/


#ifndef __NEOIP_LIB_SESSION_EXIT_CB_HPP__ 
#define __NEOIP_LIB_SESSION_EXIT_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	lib_session_exit_t;

/** \brief the callback class for lib_session_exit_t
 */
class lib_session_exit_cb_t {
public:
	/** \brief callback notified by \ref lib_session_exit_t when to notify an event
	 */
	virtual bool neoip_lib_session_exit_cb(void *cb_userptr, lib_session_exit_t &session_exit)	throw() = 0;
	// virtual destructor
	virtual ~lib_session_exit_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_LIB_SESSION_EXIT_CB_HPP__  */



