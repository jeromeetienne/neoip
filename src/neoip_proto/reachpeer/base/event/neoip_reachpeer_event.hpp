/*! \file
    \brief Header of the \ref reachpeer_event_t
*/


#ifndef __NEOIP_REACHPEER_EVENT_HPP__ 
#define __NEOIP_REACHPEER_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class reachpeer_full_t;
class pkt_t;
/** \ref class to store the packet type sent by \ref ns-peer
 */
class reachpeer_event_t {
public:
	enum type {
		NONE,
		PEER_UNREACH,		//!< the connection has been refused by the other peer
		PKT_TO_LOWER,		//!< When a packet must be sent asynchronouly to the lower layer
		MAX
	};
	std::string	reason;
	pkt_t *		pkt_ptr;
private:
	reachpeer_event_t::type	type_val;
public:
	reachpeer_event_t()	throw();
	~reachpeer_event_t()	throw();

	reachpeer_event_t::type	get_value()	const throw()	{ return type_val;	}

	bool				is_peer_unreach() 				const throw();
	static reachpeer_event_t	build_peer_unreach(const std::string &reason)	throw();
	const std::string &		get_peer_unreach_reason()			const throw();


	bool				is_pkt_to_lower() 				const throw();
	static reachpeer_event_t	build_pkt_to_lower(pkt_t *pkt)			throw();
	pkt_t *				get_pkt_to_lower()				const throw();
		
	/** \brief return true if the event if fatal (aka if the neoip_reachpeer object is no more usable)
	 */
	bool is_fatal()	const throw() {
		return is_peer_unreach();
	}
friend	std::ostream & operator << (std::ostream & os, const reachpeer_event_t &reachpeer_event) throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_REACHPEER_EVENT_HPP__  */



