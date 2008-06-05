/*! \file
    \brief Header of the ntudp_event_t
*/


#ifndef __NEOIP_NTUDP_EVENT_HPP__ 
#define __NEOIP_NTUDP_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class ntudp_full_t;
class pkt_t;

/** \ref class to store the event notified by the udp layer
 * 
 */
class ntudp_event_t {
public:
	enum type {
		NONE,
		CNX_CLOSED,
		CNX_ESTA_TIMEDOUT,
		CNX_ESTABLISHED,
		UNKNOWN_HOST,
		NO_ROUTE_TO_HOST,
		RECVED_DATA,
		MTU_CHANGE,
		MAX
	};
	std::string		reason_str;
	ntudp_full_t *		ntudp_full;
	pkt_t *			pkt_ptr;
	size_t			mtu_size;
private:
	ntudp_event_t::type	type_val;
public:
	ntudp_event_t()	throw();
	~ntudp_event_t()	throw();

	ntudp_event_t::type	get_value()	const throw()	{ return type_val;	}

	bool			is_cnx_closed() 				const throw();
	static ntudp_event_t	build_cnx_closed(const std::string &reason = "")	throw();
	const std::string &	get_cnx_closed_reason()				const throw();

	bool			is_cnx_esta_timedout() 				const throw();
	static ntudp_event_t	build_cnx_esta_timedout(const std::string &reason = "") throw();
	const std::string &	get_cnx_esta_timedout_reason()			const throw();

	bool			is_cnx_established() 				const throw();
	static ntudp_event_t	build_cnx_established(ntudp_full_t *ntudp_full)	throw();
	ntudp_full_t *		get_cnx_established()				const throw();

	bool			is_unknown_host() 				const throw();
	static ntudp_event_t	build_unknown_host(const std::string &reason = "")	throw();
	const std::string &	get_unknown_host_reason()			const throw();

	bool			is_no_route_to_host() 				const throw();
	static ntudp_event_t	build_no_route_to_host(const std::string &reason = "")	throw();
	const std::string &	get_no_route_to_host_reason()			const throw();

	bool			is_recved_data() 				const throw();
	static ntudp_event_t	build_recved_data(pkt_t *pkt)			throw();
	pkt_t *			get_recved_data()				const throw();

	bool			is_mtu_change() 				const throw();
	static ntudp_event_t	build_mtu_change(size_t mtu_size)		throw();
	size_t			get_mtu_change()				const throw();



	/** \brief return true if the event if fatal (aka if the udp object is no more usable)
	 * 
	 * - doesnt depends of the type of the notifier (ntudp_resp_t, ntudp_itor_t etc...)
	 */
	bool is_fatal()	const throw(){
		return is_cnx_closed() || is_cnx_esta_timedout() || is_unknown_host() || is_no_route_to_host();
	}

	//! return true is the event is allowed to be returned by a ntudp_resp
	bool is_resp_ok() const throw() {
		return is_cnx_established();
	}
	//! return true is the event is allowed to be returned by a ntudp_itor_t
	bool is_itor_ok() const throw() {
		return is_cnx_established() || is_unknown_host() || is_no_route_to_host()
				|| is_cnx_esta_timedout();
	}
	//! return true is the event is allowed to be returned by a ntudp_itor_retry_t	
	bool is_itor_retry_ok()	const throw() {	return is_itor_ok();	}
	//! return true is the event is allowed to be returned by a ntudp_full_t
	bool is_full_ok() const throw() {
		return is_recved_data() || is_cnx_closed() || is_mtu_change();
	}
	//! return true is the event is allowed to be returned by a ntudp_client_t
	bool is_client_ok() const throw() {
		return is_itor_ok() || is_full_ok();
	}

	/*************** Display function	*******************************/
	std::string		to_string()					const throw();
friend	std::ostream & operator << (std::ostream & os, const ntudp_event_t &ntudp_event)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_EVENT_HPP__  */



