/*! \file
    \brief Declaration of the ntudp_npos_natlback_t
    
*/


#ifndef __NEOIP_NTUDP_NPOS_NATLBACK_CB_HPP__ 
#define __NEOIP_NTUDP_NPOS_NATLBACK_CB_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_npos_natlback_t;
class ntudp_npos_event_t;

/** \brief the callback class for ntudp_npos_natlback_t
 */
class ntudp_npos_natlback_cb_t {
public:
	/** \brief callback notified by \ref ntudp_npos_natlback_t when to notify an event
	 * 
	 * @param userptr 		 	the userptr associated with this callback
	 * @param ntudp_npos_natlback 	the ntudp_npos_natlback_t which notified this callback
	 * @param ntudp_npos_event		notified event
	 * @return true if the ntudp_npos_natlback_t is still valid after the callback
	 */
	virtual bool neoip_ntudp_npos_natlback_event_cb(void *cb_userptr
					, ntudp_npos_natlback_t &cb_ntudp_npos_natlback
					, const ntudp_npos_event_t &ntudp_npos_event)	throw() = 0;
	virtual ~ntudp_npos_natlback_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_NATLBACK_CB_HPP__  */



