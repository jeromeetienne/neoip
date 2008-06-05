/*! \file
    \brief Declaration of the bt_swarm_sched_cnx_vapi_t

*/


#ifndef __NEOIP_BT_SWARM_SCHED_CNX_VAPI_HPP__ 
#define __NEOIP_BT_SWARM_SCHED_CNX_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_sched_request_t;

/** \brief Virtual API for the connections linked by the bt_swarm_sched_t
 * 
 * - e.g. for bt_ecnx_http_cnx_t or bt_swarm_full_t
 * - NOTE: the bt_swarm_sched_request_t is owned by the bt_swarm_sched_cnx_vapi_t
 *   - aka when the bt_swarm_sched_cnx_vapi_t is deleted, it MUST delete all the 
 *     linked bt_swarm_sched_request_t as well.
 * - TODO find a better name it is clearly 
 */
class bt_swarm_sched_cnx_vapi_t {
public:
	//! link and init a request to this connection 
	virtual	void	sched_req_dolink_start(bt_swarm_sched_request_t *sched_request)	throw()=0;
	//! unlink a request to this connection
	virtual	void	sched_req_unlink(bt_swarm_sched_request_t *sched_request)	throw()=0;
	//! cancel a previously started request (without unlinking it)
	virtual void	sched_req_cancel(bt_swarm_sched_request_t *sched_request)	throw()=0;
	// virtual destructor
	virtual ~bt_swarm_sched_cnx_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_SCHED_CNX_VAPI_HPP__  */



