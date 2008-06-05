/*! \file
    \brief Declaration of the ntudp_npos_inetreach_t
    
*/


#ifndef __NEOIP_NTUDP_NPOS_SERVER_PROBE_CB_HPP__ 
#define __NEOIP_NTUDP_NPOS_SERVER_PROBE_CB_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_npos_server_t;
class ntudp_npos_event_t;

/** \brief the callback class for ntudp_npos_inetreach_t probe stuff
 * 
 * - it is used by ntudp_npos_inetreach_t to know if it is reachable from the 
 *   public internet.
 */
class ntudp_npos_server_probe_cb_t {
public:
	/** \brief callback notified by \ref ntudp_npos_inetreach_t when to notify an event
	 * 
	 * @param userptr  		the userptr associated with this callback
	 * @param ntudp_npos_server	the ntudp_npos_server_t which notified this callback
	 */
	virtual void neoip_ntudp_npos_server_probe_event_cb(void *cb_userptr
					, ntudp_npos_server_t &cb_ntudp_npos_server)	throw() = 0;
	virtual ~ntudp_npos_server_probe_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_SERVER_PROBE_CB_HPP__  */



