/*! \file
    \brief Header of the callback of zerotimer_t

*/


#ifndef __NEOIP_ZEROTIMER_CB_HPP__ 
#define __NEOIP_ZEROTIMER_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	zerotimer_t;

/** \brief the callback class for \ref zerotimer_t
 */
class zerotimer_cb_t {
public:
	/** \brief callback called when the \ref zerotimer_t expire
	 * 
	 * @param userptr	the userptr associated with the callback
	 * @return true to keep the zerotimer_t, false when it is nomore valid (e.g. has been deleted
	 *         during the callback)
	 */
	virtual bool neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr) throw() = 0;
	virtual ~zerotimer_cb_t() {};
};

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_ZEROTIMER_CB_HPP__  */



