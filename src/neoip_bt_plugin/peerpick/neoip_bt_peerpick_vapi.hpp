/*! \file
    \brief Declaration of the bt_peerpick_vapi_t

\par Brief Description
a \ref bt_peerpick_vapi_t is a object which handling the peer pick for the
bt_swarm_t. aka the one defining the bt_reqauth_type_t for the bt_swarm_full_t
    
*/

#ifndef __NEOIP_BT_PEERPICK_VAPI_HPP__ 
#define __NEOIP_BT_PEERPICK_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Virtual API for the tracker plugins
 * 
 * - it MUST be constructed BEFORE the bt_swarm_t and deleted AFTER.
 */
class bt_peerpick_vapi_t {
public:
	//! update the peerpick
	virtual void	peerpick_update()		throw() = 0;
	// virtual destructor
	virtual ~bt_peerpick_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERPICK_VAPI_HPP__  */



