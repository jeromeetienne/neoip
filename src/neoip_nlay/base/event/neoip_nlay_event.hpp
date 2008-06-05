/*! \file
    \brief Header of the \ref nlay_event_t
*/


#ifndef __NEOIP_NLAY_EVENT_HPP__ 
#define __NEOIP_NLAY_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class nlay_full_t;
class pkt_t;

/** \ref class to store the packet type sent by \ref ns-peer
 */
class nlay_event_t {
public:
	enum type {
		NONE,
		CNX_CLOSING,		//!< the connection has been closed (may be triggered by multiple things)
		CNX_DESTROYED,		//!< the connection has been closed (may be triggered by multiple things)
		REMOTE_PEER_UNREACHABLE,//!< When the remote peer is declared unreachable
		IDLE_TIMEDOUT,		//!< When the connection has been idle too long
		CNX_REFUSED,		//!< the connection has been refused by the other peer
		CNX_ESTABLISHED,	//!< the connection is now established
		PKT_TO_LOWER,		//!< When a packet must be sent asynchronouly to the lower layer
		PKT_TO_UPPER,		//!< When a packet must be sent asynchronouly to the upper layer
		MAYSEND_ON,		//!< now data may be sent over the connection
		MAYSEND_OFF,		//!< now data can no more be sent over the connection
		NEW_MTU,		//!< when a new mtu is set 
					//!< TODO is this used? dont think so. to remove if confirmed not
		MAX
	};
private:
	nlay_event_t::type	type_val;

	// all event parameters
	std::string	reason;
	nlay_full_t *	nlay_full;
	pkt_t *		pkt_ptr;
	size_t		mtu_size;

public:
	nlay_event_t()	throw();
	~nlay_event_t()	throw();

	nlay_event_t::type	get_value()	const throw()	{ return type_val;	}

	bool			is_cnx_refused() 				const throw();
	static nlay_event_t	build_cnx_refused(const std::string &reason)	throw();
	const std::string &	get_cnx_refused_reason()			const throw();
		
	bool			is_cnx_established() 				const throw();
	static nlay_event_t	build_cnx_established(nlay_full_t *nlay_full)	throw();
	nlay_full_t *		get_cnx_established()				const throw();

	bool			is_cnx_closing() 				const throw();
	static nlay_event_t	build_cnx_closing(const std::string &reason)	throw();
	const std::string &	get_cnx_closing_reason()				const throw();

	bool			is_cnx_destroyed() 				const throw();
	static nlay_event_t	build_cnx_destroyed(const std::string &reason)	throw();
	const std::string &	get_cnx_destroyed_reason()			const throw();

	bool			is_idle_timedout() 				const throw();
	static nlay_event_t	build_idle_timedout(const std::string &reason)	throw();
	const std::string &	get_idle_timedout_reason()			const throw();

	bool			is_remote_peer_unreachable() 				const throw();
	static nlay_event_t	build_remote_peer_unreachable(const std::string &reason)throw();
	const std::string &	get_remote_peer_unreachable_reason()			const throw();

	bool			is_pkt_to_lower() 				const throw();
	static nlay_event_t	build_pkt_to_lower(pkt_t *pkt)			throw();
	pkt_t *			get_pkt_to_lower()				const throw();

	bool			is_pkt_to_upper() 				const throw();
	static nlay_event_t	build_pkt_to_upper(pkt_t *pkt)			throw();
	pkt_t *			get_pkt_to_upper()				const throw();

	bool			is_maysend_on() 				const throw();
	static nlay_event_t	build_maysend_on()				throw();
	
	bool			is_maysend_off() 				const throw();
	static nlay_event_t	build_maysend_off()				throw();

	bool			is_new_mtu() 					const throw();
	static nlay_event_t	build_new_mtu(size_t mtu_size)			throw();
	size_t			get_new_mtu()					const throw();
				
	/** \brief return true if the event if fatal (aka if the neoip_nlay object is no more usable)
	 */
	bool is_fatal()	const throw() {
		return is_cnx_refused() || is_cnx_closing() || is_cnx_destroyed()
				|| is_idle_timedout() || is_remote_peer_unreachable();
	}
	//! return true is the event is allowed to be returned synchronously by a resp
	bool is_sync_resp_ok() const throw() {
		// NONE happen when no connection is established
		return is_cnx_established() || get_value() == NONE;
	}
	//! return true is the event is allowed to be returned by a synchrnously itor
	bool is_sync_itor_ok() const throw() {
		// NONE happen when no connection is established
		return is_cnx_established() || is_cnx_refused() || get_value() == NONE;
	}
	//! return true is the event is allowed to be returned asynchronously by a itor
	bool is_async_itor_ok() const throw() {
		return is_pkt_to_lower();
	}
	//! return true is the event is allowed to be returned by a full
	bool is_full_ok() const throw() {
		return is_pkt_to_lower() || is_pkt_to_upper() || is_new_mtu() 
				|| is_maysend_on() || is_maysend_off()
				|| is_cnx_closing() || is_cnx_destroyed()
				|| is_idle_timedout() || is_remote_peer_unreachable();
	}
	
	/*************** display function	*******************************/
	friend	std::ostream & operator << (std::ostream & os, const nlay_event_t &nlay_event) throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_EVENT_HPP__  */



