/*! \file
    \brief Declaration of the router_itor_cnx_cb_t
    
*/


#ifndef __NEOIP_ROUTER_ITOR_CNX_CB_HPP__ 
#define __NEOIP_ROUTER_ITOR_CNX_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	router_itor_cnx_t;
class	socket_full_t;
class	ip_addr_t;
class	pkt_t;

/** \brief the callback class for router_itor_cnx_t
 */
class router_itor_cnx_cb_t {
public:
	/** \brief callback notified by \ref router_itor_cnx_t when a connection is established
	 * 
	 * @param userptr  		the userptr associated with this callback
	 * @param router_itor_cnx 	the router_itor_t which notified this callback
	 * @param socket_full		pointer on the established socket_full_t on suceess. NULL on faillure
	 * @param local_addr		the negociated inner local ip_addr_t (null on faillure)
	 * @param remote_addr		the negociated inner remote ip_addr_t (null on faillure)
	 * @param estapkt_in		the incoming packet to expect for packet of cnx establishement
	 *                      	(may be null. if so estapkt_in/out are both null)
	 * @param estapkt_out		the outgoing packet to reply when the estapkt_in is received
	 *                      	(may be null. if so estapkt_in/out are both null)
	 * @return a tokeep aka false if the notifier has been deleted during the callback, true otherwise
	 */
	virtual bool neoip_router_itor_cnx_cb(void *cb_userptr, router_itor_cnx_t &cb_itor_cnx
				, socket_full_t *socket_full
				, const ip_addr_t &local_addr, const ip_addr_t &inner_remote_addr
				, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw() = 0;
	virtual ~router_itor_cnx_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ITOR_CNX_CB_HPP__  */



