/*! \file
    \brief Header of the \ref bt_ezswarm_event_t
*/


#ifndef __NEOIP_BT_EZSWARM_EVENT_HPP__ 
#define __NEOIP_BT_EZSWARM_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_bt_ezswarm_state.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_event_t;

/** \ref class to store the event from bt_swarm_t
 */
class bt_ezswarm_event_t: NEOIP_COPY_CTOR_ALLOW {
public:
	enum type {
		NONE,
		SWARM_EVENT,
		LEAVE_STATE_PRE,
		LEAVE_STATE_POST,
		ENTER_STATE_PRE,
		ENTER_STATE_POST,
		MAX
	};
private:
	bt_ezswarm_event_t::type	type_val;

	// all event parameters
	const bt_swarm_event_t *swarm_event;	
	bt_err_t		bt_err;
	bt_ezswarm_state_t	ezswarm_state;
public:
	/*************** ctor/dtor	***************************************/
	bt_ezswarm_event_t()		throw();
	~bt_ezswarm_event_t()		throw();
	
	/*************** query function	***************************************/
	bt_ezswarm_event_t::type	get_value()	const throw()	{ return type_val;}

	/*************** specific event ctor/dtor/query	***********************/
	bool				is_swarm_event() 				const throw();
	static bt_ezswarm_event_t	build_swarm_event(const bt_swarm_event_t *swarm_event)throw();
	const bt_swarm_event_t *	get_swarm_event()				const throw();

	bool				is_leave_state_pre() 				const throw();
	static bt_ezswarm_event_t	build_leave_state_pre(const bt_ezswarm_state_t &state)throw();
	const bt_ezswarm_state_t &	get_leave_state_pre()				const throw();

	bool				is_leave_state_post() 				const throw();
	static bt_ezswarm_event_t	build_leave_state_post(const bt_ezswarm_state_t &state)throw();
	const bt_ezswarm_state_t &	get_leave_state_post()				const throw();

	bool				is_enter_state_pre() 				const throw();
	static bt_ezswarm_event_t	build_enter_state_pre(const bt_ezswarm_state_t &state)throw();
	const bt_ezswarm_state_t &	get_enter_state_pre()				const throw();

	bool				is_enter_state_post() 				const throw();
	static bt_ezswarm_event_t	build_enter_state_post(const bt_ezswarm_state_t &state)throw();
	const bt_ezswarm_state_t &	get_enter_state_post()				const throw();

	/*************** display function	*******************************/
	std::string			to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_ezswarm_event_t &event) throw()
					{ return oss << event.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_EVENT_HPP__  */



