/*! \file
    \brief Declaration of the ntudp_npos_eval_t callback
    
*/


#ifndef __NEOIP_NTUDP_NPOS_EVAL_CB_HPP__ 
#define __NEOIP_NTUDP_NPOS_EVAL_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_npos_eval_t;
class ntudp_npos_res_t;

/** \brief the callback class for ntudp_npos_eval_t
 */
class ntudp_npos_eval_cb_t {
public:
	/** \brief callback notified by \ref ntudp_npos_eval_t when to notify an event
	 * 
	 * @param userptr 		the userptr associated with this callback
	 * @param ntudp_npos_eval 	the ntudp_npos_eval_t which notified this callback
	 * @param ntudp_npos_res	notified result - if .is_null(), the evaluation failed
	 * @return true if the ntudp_npos_eval_t is still valid after the callback (in fact
	 *         it MUST always return false as the ntudp_npos_eval_t MUST be deleted in the 
	 *         callback)
	 */
	virtual bool neoip_ntudp_npos_eval_cb(void *cb_userptr, ntudp_npos_eval_t &cb_ntudp_npos_eval
					, const ntudp_npos_res_t &ntudp_npos_res)	throw() = 0;
	virtual ~ntudp_npos_eval_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_EVAL_CB_HPP__  */



