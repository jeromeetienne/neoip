/*! \file
    \brief Declaration of the bt_ecnx_vapi_t
    
*/


#ifndef __NEOIP_BT_ECNX_VAPI_HPP__ 
#define __NEOIP_BT_ECNX_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_object_slotid.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_sched_cnx_vapi_t;
class	bt_pieceavail_t;

/** \brief Virtual API for the connections externals of bt_swarm_t
 * 
 * - e.g. to download content from http or ftp
 * - it MUST be constructed AFTER the bt_swarm_t and deleted BEFORE
 */
class bt_ecnx_vapi_t : public object_slotid_t {
public:
	//! return the maximum amount of data to request on this connection at a given time
	virtual size_t			req_queue_maxlen()		const throw() = 0;

	//! return the current number of pending request on this connection
	virtual	size_t			nb_pending_request()		const throw() = 0;

	//! return the bt_swarm_sched_cnx_vapi_t pointer for this bt_ecnx_vapi_t
	virtual	bt_swarm_sched_cnx_vapi_t *	sched_cnx_vapi()	throw() = 0;

	//! return the bt_pieceavail_t for this connection
	virtual const bt_pieceavail_t &	remote_pavail()		const throw() = 0;
	// FIXME another function like is_range_avail(totfile_beg, totfile_end) may be provided later
	// - it would allow to provide available with a byte granularity instead of a piece one

	//! return the average recv rate for this connection
	virtual double			recv_rate_avg()			const throw() = 0;
	
	// virtual destructor
	virtual ~bt_ecnx_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_ECNX_VAPI_HPP__  */



