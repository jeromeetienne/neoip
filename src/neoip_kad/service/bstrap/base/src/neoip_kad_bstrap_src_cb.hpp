/*! \file
    \brief Declaration of the kad_bstrap_src_t's callback
    
*/


#ifndef __NEOIP_KAD_BSTRAP_SRC_CB_HPP__ 
#define __NEOIP_KAD_BSTRAP_SRC_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class kad_bstrap_src_t;
class kad_addr_t;

/** \brief the callback class for kad_bstrap_src_t
 */
class kad_bstrap_src_cb_t {
public:
	/** \brief callback notified by \ref kad_bstrap_src_t when to notify an ipport_addr_t
	 * 
	 * @param userptr 		the userptr associated with this callback.
	 * @param kad_bstrap_src	the kad_bstrap_src_t which notified this callback.
	 * @param kad_addr		notified kad_addr_ - if .is_null(), no more address 
	 * 				are available. but a get_more() will try to get more
	 *                              either by relaunching another query or looping on previous
	 *                              result.
	 * @return true if the kad_bstrap_src_t is still valid after the callback
	 */
	virtual bool neoip_kad_bstrap_src_cb(void *cb_userptr
					, kad_bstrap_src_t &cb_kad_bstrap_src
					, const kad_addr_t &kad_addr)	throw() = 0;
	virtual ~kad_bstrap_src_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_BSTRAP_SRC_CB_HPP__  */



