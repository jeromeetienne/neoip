/*! \file
    \brief Declaration of the netif_addr_watch_t callback
    
*/


#ifndef __NEOIP_NETIF_ADDR_WATCH_CB_HPP__ 
#define __NEOIP_NETIF_ADDR_WATCH_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class netif_addr_watch_t;
class netif_addr_arr_t;

/** \brief the callback class for netif_addr_watch_t
 */
class netif_addr_watch_cb_t {
public:
	/** \brief callback notified by \ref netif_addr_watch_t when the netif_addr_arr changes
	 * 
	 * @param userptr 		the userptr associated with this callback
	 * @param netif_addr_watch 	the netif_addr_watch_t which notified this callback
	 * @param netif_addr_arr	notified result
	 * 
	 * @return a tokeep for the netif_addr_watch_t object
	 */
	virtual bool neoip_netif_addr_watch_cb(void *cb_userptr
					, netif_addr_watch_t &cb_netif_addr_watch
					, const netif_addr_arr_t &netif_addr_arr)	throw() = 0;
	virtual ~netif_addr_watch_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NETIF_ADDR_WATCH_CB_HPP__  */



