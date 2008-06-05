/*! \file
    \brief Declaration of the ntudp_npos_watch_t callback
    
*/


#ifndef __NEOIP_NTUDP_NPOS_WATCH_CB_HPP__ 
#define __NEOIP_NTUDP_NPOS_WATCH_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_npos_watch_t;
class ntudp_npos_res_t;

/** \brief the callback class for ntudp_npos_watch_t
 */
class ntudp_npos_watch_cb_t {
public:
	/** \brief callback notified by \ref ntudp_npos_watch_t when the ntudp_npos_res_t changes
	 * 
	 * @param userptr 		the userptr associated with this callback
	 * @param ntudp_npos_watch 	the ntudp_npos_watch_t which notified this callback
	 * @param ntudp_npos_res	notified result - if .is_null(), the upduation failed
	 * @return a tokeep for the ntudp_npos_udpate_t object
	 */
	virtual bool neoip_ntudp_npos_watch_cb(void *cb_userptr
					, ntudp_npos_watch_t &cb_ntudp_npos_watch
					, const ntudp_npos_res_t &ntudp_npos_res)	throw() = 0;
	virtual ~ntudp_npos_watch_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_WATCH_CB_HPP__  */



