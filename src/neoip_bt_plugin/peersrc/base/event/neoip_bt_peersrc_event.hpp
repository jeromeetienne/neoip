/*! \file
    \brief Header of the \ref bt_peersrc_event_t
*/


#ifndef __NEOIP_BT_PEERSRC_EVENT_HPP__ 
#define __NEOIP_BT_PEERSRC_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_peersrc_peer.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the event from bt_peersrc_t
 */
class bt_peersrc_event_t {
public:
	enum type {
		NONE,
		DOREGISTER,
		UNREGISTER,
		NEW_PEER,
		MAX
	};
private:
	bt_peersrc_event_t::type	type_val;

	// all event parameters
	bt_peersrc_peer_t *	peersrc_peer;
public:
	/*************** ctor/dtor	***************************************/
	bt_peersrc_event_t()	throw();
	~bt_peersrc_event_t()	throw();
	
	/*************** query function	***************************************/
	bt_peersrc_event_t::type	get_value()	const throw()	{ return type_val;	}

	/*************** specific event ctor/dtor/query	***********************/
	bool				is_doregister()				const throw();
	static bt_peersrc_event_t 	build_doregister()			throw();
		
	bool				is_unregister()				const throw();
	static bt_peersrc_event_t 	build_unregister()			throw();
		
	bool				is_new_peer() 				const throw();
	static bt_peersrc_event_t 	build_new_peer(bt_peersrc_peer_t *peersrc_peer)	throw();
	bt_peersrc_peer_t *		get_new_peer()				const throw();

	/*************** display function	*******************************/
	std::string			to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_peersrc_event_t &event) throw()
					{ return oss << event.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERSRC_EVENT_HPP__  */



