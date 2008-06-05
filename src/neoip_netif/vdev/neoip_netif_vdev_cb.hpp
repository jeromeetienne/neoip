/*! \file
    \brief Header of the netif_vdev_t callback
*/


#ifndef __NEOIP_NETIF_VDEV_CB_HPP__ 
#define __NEOIP_NETIF_VDEV_CB_HPP__ 
/* system include */
#include <stdint.h>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class netif_vdev_t;
class pkt_t;

/** \brief the callback class for \ref netif_vdev_t
 */
class netif_vdev_cb_t {
public:
	/** \brief callback notified when netif_vdev_t has an event to report
	 * 
	 * @return a 'tokeep' aka false if the vdev has NOT been deleted
	 */
	virtual bool neoip_netif_vdev_cb(void *cb_userptr, netif_vdev_t &netif_vdev
						, uint16_t ethertype, pkt_t &pkt) throw() = 0;
	virtual ~netif_vdev_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NETIF_VDEV_CB_HPP__  */



