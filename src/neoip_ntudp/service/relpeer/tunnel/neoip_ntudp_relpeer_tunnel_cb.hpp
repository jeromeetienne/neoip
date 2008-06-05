/*! \file
    \brief Declaration of the ntudp_relpeer_tunnel_t
    
*/


#ifndef __NEOIP_NTUDP_RELPEER_TUNNEL_CB_HPP__ 
#define __NEOIP_NTUDP_RELPEER_TUNNEL_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class ntudp_relpeer_tunnel_t;

/** \brief the callback class for ntudp_relpeer_tunnel_t
 */
class ntudp_relpeer_tunnel_cb_t {
public:
	/** \brief callback notified by \ref ntudp_relpeer_tunnel_t when to notify an event
	 * 
	 * @param userptr  	the userptr associated with this callback
	 * @param cb_tunnel 	the ntudp_relpeer_tunnel_t which notified this callback
	 * @param established	true if the tunnel is suceefully established, false otherwise
	 * @return true if the ntudp_relpeer_tunnel_t is still valid after the callback
	 */
	virtual bool neoip_ntudp_relpeer_tunnel_cb(void *cb_userptr
					, ntudp_relpeer_tunnel_t &cb_tunnel
					, const bool established)	throw() = 0;
	virtual ~ntudp_relpeer_tunnel_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RELPEER_TUNNEL_CB_HPP__  */



