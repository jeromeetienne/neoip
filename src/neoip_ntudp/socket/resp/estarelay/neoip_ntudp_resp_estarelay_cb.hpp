/*! \file
    \brief Declaration of the ntudp_resp_estarelay_cb_t
    
*/


#ifndef __NEOIP_NTUDP_RESP_ESTARELAY_CB_HPP__ 
#define __NEOIP_NTUDP_RESP_ESTARELAY_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_resp_estarelay_t;
class udp_full_t;
class pkt_t;


/** \brief the callback class for ntudp_resp_estarelay_t
 */
class ntudp_resp_estarelay_cb_t {
public:
	/** \brief callback notified by \ref ntudp_resp_estarelay_t when a connection is established
	 * 
	 * @param userptr  		the userptr associated with this callback
	 * @param ntudp_resp_estarelay 	the sub resp which notified this callback
	 * @param udp_full		pointer on the established udp_full_t on suceess. NULL on faillure
	 * @param estapkt_in		the incoming packet to expect for packet of cnx establishement
	 *                      	(may be null. if so estapkt_in/out are both null)
	 * @param estapkt_out		the outgoing packet to reply when the estapkt_in is received
	 *                      	(may be null. if so estapkt_in/out are both null)
	 * @return a tokeep aka false if the notifier has been deleted during the callback, true otherwise
	 */
	virtual bool neoip_ntudp_resp_estarelay_cb(void *cb_userptr
				, ntudp_resp_estarelay_t &cb_ntudp_resp_estarelay, udp_full_t *udp_full
				, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw() = 0;
	virtual ~ntudp_resp_estarelay_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RESP_ESTARELAY_CB_HPP__  */



