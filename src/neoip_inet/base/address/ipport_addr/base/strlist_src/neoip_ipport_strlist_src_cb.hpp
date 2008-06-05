/*! \file
    \brief Declaration of the ipport_strlist_src_t's callback
    
*/


#ifndef __NEOIP_IPPORT_STRLIST_SRC_CB_HPP__ 
#define __NEOIP_IPPORT_STRLIST_SRC_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ipport_strlist_src_t;
class ipport_addr_t;

/** \brief the callback class for ipport_strlist_src_t
 */
class ipport_strlist_src_cb_t {
public:
	/** \brief callback notified by \ref ipport_strlist_src_t when to notify an ipport_addr_t
	 * 
	 * @param userptr 		the userptr associated with this callback.
	 * @param ipport_strlist_src	the ipport_strlist_src_t which notified this callback.
	 * @param ipport_addr		notified ipport_addr_t - if .is_null(), no more address 
	 * 				are available. but a get_more() will try to get more
	 *                              either by relaunching another query or looping on previous
	 *                              result.
	 * @return true if the ipport_strlist_src_t is still valid after the callback
	 */
	virtual bool neoip_ipport_strlist_src_cb(void *cb_userptr
					, ipport_strlist_src_t &cb_ipport_strlist_src
					, const ipport_addr_t &ipport_addr)	throw() = 0;
	virtual ~ipport_strlist_src_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IPPORT_STRLIST_SRC_CB_HPP__  */



