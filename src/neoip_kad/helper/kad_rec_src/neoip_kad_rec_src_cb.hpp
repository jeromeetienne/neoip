/*! \file
    \brief Declaration of the kad_rec_src_t's callback
    
*/


#ifndef __NEOIP_KAD_REC_SRC_CB_HPP__ 
#define __NEOIP_KAD_REC_SRC_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class kad_rec_src_t;
class kad_rec_t;

/** \brief the callback class for kad_rec_src_t
 */
class kad_rec_src_cb_t {
public:
	/** \brief callback notified by \ref kad_rec_src_t when to notify an kad_rec_t
	 * 
	 * @param userptr 	the userptr associated with this callback.
	 * @param kad_rec_src	the kad_rec_src_t which notified this callback.
	 * @param record	notified record - if kad_rec_t::is_null(), no more record
	 * 			are available. but a get_more() will try to get more
	 *                      either by relaunching another query or looping on previous
	 *                      result.
	 * @return true if the kad_rec_src_t is still valid after the callback
	 */
	virtual bool neoip_kad_rec_src_cb(void *cb_userptr, kad_rec_src_t &cb_kad_rec_src
							, const kad_rec_t &kad_rec)	throw() = 0;
	virtual ~kad_rec_src_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_REC_SRC_CB_HPP__  */



