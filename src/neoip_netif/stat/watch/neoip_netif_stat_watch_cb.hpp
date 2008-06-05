/*! \file
    \brief Declaration of the netif_stat_watch_t callback
    
*/


#ifndef __NEOIP_NETIF_STAT_WATCH_CB_HPP__ 
#define __NEOIP_NETIF_STAT_WATCH_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class netif_stat_watch_t;
class netif_stat_arr_t;

/** \brief the callback class for netif_stat_watch_t
 */
class netif_stat_watch_cb_t {
public:
	/** \brief callback notified by \ref netif_stat_watch_t when the notifying a result
	 * 
	 * @return a tokeep for the netif_stat_watch_t object
	 */
	virtual bool neoip_netif_stat_watch_cb(void *cb_userptr, netif_stat_watch_t &cb_netif_stat_watch
					, const netif_stat_arr_t &netif_stat_arr)	throw() = 0;
	virtual ~netif_stat_watch_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NETIF_STAT_WATCH_CB_HPP__  */



