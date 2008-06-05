/*! \file
    \brief Header of the tcp_event_t
*/


#ifndef __NEOIP_TCP_EVENT_HPP__ 
#define __NEOIP_TCP_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	tcp_full_t;
class	pkt_t;

/** \ref class to store the packet type sent by \ref ns-peer
 */
class tcp_event_t {
public:
	enum type {
		NONE,
		CNX_CLOSED,
		CNX_REFUSED,
		ITOR_TIMEDOUT,
		CNX_ESTABLISHED,
		RECVED_DATA,
		MAYSEND_ON,
		MAX
	};
	// all event parameters	
	std::string		reason_str;
	tcp_full_t *		tcp_full;
	pkt_t *			pkt_ptr;
private:
	tcp_event_t::type	type_val;
public:
	/*************** ctor/dtor	***************************************/
	tcp_event_t()	throw();
	~tcp_event_t()	throw();
	
	/*************** query function	***************************************/	
	tcp_event_t::type	get_value()	const throw()	{ return type_val;	}
	
	/*************** specific event ctor/dtor/query	***********************/
	bool			is_cnx_closed() 				const throw();
	static tcp_event_t	build_cnx_closed(const std::string &reason)	throw();
	const std::string &	get_cnx_closed_reason()				const throw();

	bool			is_cnx_refused() 				const throw();
	static tcp_event_t	build_cnx_refused(const std::string &reason) throw();
	const std::string &	get_cnx_refused_reason()			const throw();

	bool			is_itor_timedout() 				const throw();
	static tcp_event_t	build_itor_timedout(const std::string &reason) throw();
	const std::string &	get_itor_timedout_reason()			const throw();

	bool			is_cnx_established() 				const throw();
	static tcp_event_t	build_cnx_established(tcp_full_t *tcp_full)	throw();
	tcp_full_t *		get_cnx_established()				const throw();

	bool			is_recved_data() 				const throw();
	static tcp_event_t	build_recved_data(pkt_t *pkt)			throw();
	pkt_t *			get_recved_data()				const throw();

	bool			is_maysend_on() 				const throw();
	static tcp_event_t	build_maysend_on()				throw();

	/** \brief return true if the event if fatal (aka if the tcp object is no more usable)
	 */
	bool	is_fatal()	const throw(){
		return is_cnx_closed() || is_cnx_refused() || is_itor_timedout();
	}

	//! return true is the event is allowed to be returned by a resp
	bool is_resp_ok() const throw() {
		return is_cnx_established();
	}
	//! return true is the event is allowed to be returned by a itor
	bool is_itor_ok() const throw() {
		return is_cnx_established() || is_cnx_refused() || is_itor_timedout();
	}
	//! return true is the event is allowed to be returned by a full
	bool is_full_ok() const throw() {
		return is_recved_data() || is_cnx_closed() || is_maysend_on();
	}
	
	//! return true if the event is allowed to be returned by a tcp_client_t
	bool	is_client_ok()	const throw()	{
		return is_itor_ok() || is_full_ok();
	}
	
	/*************** display function	*******************************/
	std::string		to_string()					const throw();	
	friend	std::ostream & operator << (std::ostream & os, const tcp_event_t &tcp_event)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_EVENT_HPP__  */



