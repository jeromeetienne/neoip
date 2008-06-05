/*! \file
    \brief Declaration of the bt_peersrc_vapi_t

\par Brief Description
a \ref bt_peersrc_vapi_t is a object which is a source of peers for the bt_swarm_t.
typically a http tracker, but it may be a dht tracker or a udp tracker.
It is launched after the bt_swarm_t and feed it via bt_peersrc_cb_t
    
*/


#ifndef __NEOIP_BT_PEERSRC_VAPI_HPP__ 
#define __NEOIP_BT_PEERSRC_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Virtual API for the bt_peersrc_t plugins
 * 
 * - it MUST be constructed AFTER the bt_swarm_t and deleted BEFORE
 */
class bt_peersrc_vapi_t {
public:
	//! return the number of seeder of this peersrc
	virtual size_t	nb_seeder()		const throw() = 0;
	//! return the number of leecher of this peersrc
	virtual size_t	nb_leecher()		const throw() = 0;
	// virtual destructor
	virtual ~bt_peersrc_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERSRC_VAPI_HPP__  */



