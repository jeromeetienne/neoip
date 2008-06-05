/*! \file
    \brief Header of the bt_swarm_sched_ecnx_t
    
*/


#ifndef __NEOIP_BT_SWARM_SCHED_ECNX_HPP__ 
#define __NEOIP_BT_SWARM_SCHED_ECNX_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_bt_ecnx_vapi_cb.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_sched_t;
class	bt_swarm_sched_piece_t;

/** \brief Handle the scheduling for the bt_ecnx_vapi_t
 */
class bt_swarm_sched_ecnx_t : NEOIP_COPY_CTOR_DENY, public bt_ecnx_vapi_cb_t {
private:
	bt_swarm_sched_t *	swarm_sched;	//!< backpointer on the bt_swarm_sched_ecnx_t

	/*************** Internal function	*******************************/
	void		try_send_request(bt_ecnx_vapi_t &ecnx_vapi)		throw();

	/*************** bt_ecnx_vapi_t	***************************************/
	bool		neoip_bt_ecnx_vapi_cb(void *cb_userptr, bt_ecnx_vapi_t &cb_ecnx_vapi
						, const bt_ecnx_event_t &ecnx_event)	throw();

	/*************** event parsing	***************************************/
	bool 		parse_cnx_opened(bt_ecnx_vapi_t &ecnx_vapi, const bt_ecnx_event_t &ecnx_event)	throw();
	bool 		parse_cnx_closed(bt_ecnx_vapi_t &ecnx_vapi, const bt_ecnx_event_t &ecnx_event)	throw();
	bool 		parse_block_rep(bt_ecnx_vapi_t &ecnx_vapi, const bt_ecnx_event_t &ecnx_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_sched_ecnx_t(bt_swarm_sched_t *swarm_sched) 		throw();
	~bt_swarm_sched_ecnx_t()					throw();

	/*************** action function	*******************************/
	void		try_send_request_on_idle_cnx()			throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_SCHED_ECNX_HPP__  */



