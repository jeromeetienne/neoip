/*! \file
    \brief Header of the bt_swarm_sched_full_t
    
*/


#ifndef __NEOIP_BT_SWARM_SCHED_FULL_HPP__ 
#define __NEOIP_BT_SWARM_SCHED_FULL_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_swarm_full_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_sched_t;
class	bt_swarm_sched_piece_t;
class	bt_peerpick_vapi_t;
class	bt_reqauth_type_t;
class	delay_t;

/** \brief Handle the scheduling for the bt_swarm_full_t connection
 */
class bt_swarm_sched_full_t : NEOIP_COPY_CTOR_DENY, public bt_swarm_full_cb_t {
private:
	bt_swarm_sched_t *	swarm_sched;	//!< backpointer to the linked bt_swarm_t
	bt_peerpick_vapi_t *	m_peerpick_vapi;//!< the bt_peerpick_vapi_t to use
	
	/*************** Internal function	*******************************/
	void		try_send_request(bt_swarm_full_t &swarm_full)		throw();

	/*************** peer selection	***************************************/
	void		peer_select_update()					throw();

	/*************** bt_swarm_full_t	*******************************/
	bool 		neoip_bt_swarm_full_cb(void *cb_userptr, bt_swarm_full_t &swarm_full
						, const bt_swarm_full_event_t &full_event)	throw();
	/*************** parse bt_swarm_full_event_t	***********************/
	bool 		parse_cnx_opened(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_cnx_closed(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_reqauth_timedout(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_unauth_req(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_doauth_req(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_unwant_req(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_dowant_req(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_piece_isavail(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_piece_unavail(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_piece_bfield(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_block_rep(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)		throw();
	bool 		parse_pwish_doindex(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_pwish_noindex(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
	bool 		parse_pwish_dofield(bt_swarm_full_t &swarm_full, const bt_swarm_full_event_t &full_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_sched_full_t(bt_swarm_sched_t *bt_swarm_sched) 	throw();
	~bt_swarm_sched_full_t()					throw();


	/*************** query function	***************************************/
	bt_peerpick_vapi_t *	peerpick_vapi()	const throw()	{ return m_peerpick_vapi;	}
	void			peerpick_vapi(bt_peerpick_vapi_t *new_peerpick)	throw();

	/*************** action function	*******************************/
	void		try_send_request_on_idle_cnx()			throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_SCHED_FULL_HPP__  */



