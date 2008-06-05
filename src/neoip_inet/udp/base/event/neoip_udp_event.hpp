/*! \file
    \brief Header of the udp_event_t
*/


#ifndef __NEOIP_UDP_EVENT_HPP__ 
#define __NEOIP_UDP_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class udp_full_t;
class pkt_t;

/** \ref class to store the event notified by the udp layer
 * 
 * - TODO the CNX_REFUSED is unused as udp_connection can not be refused
 *   - it is always accepted as there is no connection establishement over the wire
 *   - it is there due to cut/paste with other event_t
 */
class udp_event_t {
public:
	enum type {
		NONE,
		CNX_CLOSED,
		CNX_REFUSED,
		CNX_ESTABLISHED,
		RECVED_DATA,
		MTU_CHANGE,
		MAX
	};
	std::string		reason_str;
	udp_full_t *		udp_full;
	pkt_t *			pkt_ptr;
	size_t			mtu_size;
private:
	udp_event_t::type	type_val;
public:
	udp_event_t()	throw();
	~udp_event_t()	throw();

	udp_event_t::type	get_value()	const throw()	{ return type_val;	}

	bool			is_cnx_closed() 				const throw();
	static udp_event_t	build_cnx_closed(const std::string &reason)	throw();
	const std::string &	get_cnx_closed_reason()				const throw();

	bool			is_cnx_refused() 				const throw();
	static udp_event_t	build_cnx_refused(const std::string &reason) throw();
	const std::string &	get_cnx_refused_reason()			const throw();

	bool			is_cnx_established() 				const throw();
	static udp_event_t	build_cnx_established(udp_full_t *udp_full)	throw();
	udp_full_t *		get_cnx_established()				const throw();

	bool			is_recved_data() 				const throw();
	static udp_event_t	build_recved_data(pkt_t *pkt)			throw();
	pkt_t *			get_recved_data()				const throw();

	bool			is_mtu_change() 				const throw();
	static udp_event_t	build_mtu_change(size_t mtu_size)		throw();
	size_t			get_mtu_change()				const throw();


	/** \brief return true if the event if fatal (aka if the udp object is no more usable)
	 */
	bool is_fatal()	const throw(){
		return is_cnx_closed() || is_cnx_refused();
	}

	//! return true is the event is allowed to be returned by a resp
	bool is_resp_ok() const throw() {
		return is_cnx_established();
	}
	//! return true is the event is allowed to be returned by a itor
	bool is_itor_ok() const throw() {
		return is_cnx_established();
	}
	//! return true is the event is allowed to be returned by a full
	bool is_full_ok() const throw() {
		return is_recved_data() || is_cnx_closed() || is_mtu_change();
	}
	//! return true is the event is allowed to be returned by a client
	bool is_client_ok() const throw() {
		return is_itor_ok() || is_full_ok();
	}

	/*************** Display function	*******************************/
	std::string		to_string()					const throw();
	friend	std::ostream & operator << (std::ostream & os, const udp_event_t &udp_event)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UDP_EVENT_HPP__  */



