/*! \file
    \brief Declaration of the nslan_rec_src_t's callback
    
*/


#ifndef __NEOIP_NSLAN_REC_SRC_CB_HPP__ 
#define __NEOIP_NSLAN_REC_SRC_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	nslan_rec_src_t;
class	nslan_rec_t;
class	ipport_addr_t;

/** \brief the callback class for nslan_rec_src_t
 */
class nslan_rec_src_cb_t {
public:
	/** \brief callback notified by \ref nslan_rec_src_t when to notify an datum_t
	 * 
	 * @param userptr 	the userptr associated with this callback.
	 * @param nslan_rec_src	the nslan_rec_src_t which notified this callback.
	 * @param nslan_rec	notified record - if nslan_rec_t::is_null(), no more record
	 * 			are available. but a get_more() will try to get more
	 *                      either by relaunching another query or looping on previous
	 *                      result.
	 * @return true if the nslan_rec_src_t is still valid after the callback
	 */
	virtual bool neoip_nslan_rec_src_cb(void *cb_userptr, nslan_rec_src_t &cb_nslan_rec_src
						, const nslan_rec_t &nslan_rec
						, const ipport_addr_t &src_addr)	throw() = 0;
	virtual ~nslan_rec_src_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NSLAN_REC_SRC_CB_HPP__  */



