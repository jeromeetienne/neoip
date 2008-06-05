/*! \file
    \brief Header of the \ref socket_event_t
*/


#ifndef __NEOIP_SOCKET_EVENT_HPP__ 
#define __NEOIP_SOCKET_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class socket_full_t;
class pkt_t;
/** \ref class to store the packet type sent by \ref ns-peer
 */
class socket_event_t {
public:
	enum type {
		NONE,
		CNX_CLOSED,		//!< the remote peer explicitly closed the connection
		IDLE_TIMEDOUT,		//!< the connection has been idle too long
		REMOTE_PEER_UNREACHABLE,//!< the remote peer is declared unreachable
		NETWORK_ERROR,		//!< the otsp connection reported an fatal error
					//!< TODO what is this ?!?!? why not a CNX_CLOSED ?
		CNX_REFUSED,		//!< the connection has been refused by the other peer
		CNX_ESTABLISHED,	//!< the connection is now established
		RECVED_DATA,		//!< data has been received by the connection
		MAYSEND_ON,		//!< now data may be sent over the connection
					//!< only for socket_full_t with socket_type_t::is_reliable
		NEW_MTU,		//!< when a new mtu is set. only on socket_type_t::is_datagram()
		MAX
	};
private:
	socket_event_t::type	type_val;

	// all event parameters
	std::string	reason;
	socket_full_t *	socket_full;
	pkt_t *		pkt_ptr;
	size_t		mtu_size;
public:
	socket_event_t()	throw();
	~socket_event_t()	throw();
	
	socket_event_t::type	get_value()	const throw()	{ return type_val;	}

	bool			is_cnx_refused() 				const throw();
	static socket_event_t	build_cnx_refused(const std::string &reason)	throw();
	const std::string &	get_cnx_refused_reason()			const throw();

	bool			is_cnx_closed() 				const throw();
	static socket_event_t	build_cnx_closed(const std::string &reason)	throw();
	const std::string &	get_cnx_closed_reason()				const throw();

	bool			is_idle_timedout() 				const throw();
	static socket_event_t	build_idle_timedout(const std::string &reason)	throw();
	const std::string &	get_idle_timedout_reason()				const throw();

	bool			is_remote_peer_unreachable() 				const throw();
	static socket_event_t	build_remote_peer_unreachable(const std::string &reason)	throw();
	const std::string &	get_remote_peer_unreachable_reason()				const throw();

	bool			is_network_error() 				const throw();
	static socket_event_t	build_network_error(const std::string &reason)	throw();
	const std::string &	get_network_error_reason()				const throw();
			
	bool			is_cnx_established() 				const throw();
	static socket_event_t	build_cnx_established(socket_full_t *socket_full)	throw();
	socket_full_t *		get_cnx_established()				const throw();

	bool			is_recved_data() 		const throw();
	static socket_event_t	build_recved_data(pkt_t *pkt)	throw();
	pkt_t *			get_recved_data()		const throw();

	bool			is_maysend_on() 		const throw();
	static socket_event_t	build_maysend_on()		throw();

	bool			is_new_mtu() 					const throw();
	static socket_event_t	build_new_mtu(size_t mtu_size)			throw();
	size_t			get_new_mtu()					const throw();
		
	/** \brief return true if the event if fatal (aka if the neoip_socket object is no more usable)
	 */
	bool is_fatal()	const throw() {
		return is_cnx_closed() || is_cnx_refused() || is_idle_timedout()
				|| is_remote_peer_unreachable() || is_network_error();
	}

	//! return true is the event is allowed to be returned by a resp
	bool is_resp_ok() const throw() {
		return is_cnx_established();
	}
	//! return true is the event is allowed to be returned by a itor
	bool is_itor_ok() const throw() {
		return is_cnx_established() || is_cnx_refused();
	}
	//! return true is the event is allowed to be returned by a full
	bool is_full_ok() const throw() {
		return is_cnx_closed() || is_recved_data() || is_new_mtu() 
			|| is_maysend_on() || is_idle_timedout()
			|| is_remote_peer_unreachable() || is_network_error();
	}
	//! return true is the event is allowed to be returned by a client
	bool is_client_ok() const throw() {
		return is_full_ok() || is_itor_ok();
	}

	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const socket_event_t &socket_event) throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_EVENT_HPP__  */



