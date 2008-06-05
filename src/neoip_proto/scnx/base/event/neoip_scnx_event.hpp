/*! \file
    \brief Header of the \ref scnx_event_t
*/


#ifndef __NEOIP_SCNX_EVENT_HPP__ 
#define __NEOIP_SCNX_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class scnx_full_t;
class pkt_t;

/** \ref class to store the packet type sent by \ref ns-peer
 */
class scnx_event_t {
public:
	enum type {
		NONE,
		CNX_REFUSED,		//!< the connection has been refused by the other peer
		CNX_ESTABLISHED,	//!< the connection is now established
		PKT_TO_LOWER,		//!< When a packet must be sent asynchronouly to the lower layer
		MAX
	};
	std::string	reason;
	scnx_full_t *	scnx_full;
	void *		auxnego_ptr;
	pkt_t *		pkt_ptr;
private:
	scnx_event_t::type	type_val;
public:
	scnx_event_t()	throw();
	~scnx_event_t()	throw();

	scnx_event_t::type	get_value()	const throw()	{ return type_val;	}

	bool			is_cnx_refused() 				const throw();
	static scnx_event_t	build_cnx_refused(const std::string &reason)	throw();
	const std::string &	get_cnx_refused_reason()			const throw();
		
	bool			is_cnx_established() 				const throw();
	static scnx_event_t	build_cnx_established(scnx_full_t *scnx_full, void *auxnego_ptr) throw();
	scnx_full_t *		get_cnx_established(void **auxnego_ptr)		const throw();

	bool			is_pkt_to_lower() 				const throw();
	static scnx_event_t	build_pkt_to_lower(pkt_t *pkt)			throw();
	pkt_t *			get_pkt_to_lower()				const throw();
		
	/** \brief return true if the event if fatal (aka if the neoip_scnx object is no more usable)
	 */
	bool is_fatal()	const throw() {
		return is_cnx_refused();
	}
	
	//! return true is the event is allowed to be synchrnously returned by a resp
	bool is_sync_resp_ok() const throw() {
		// NONE happen when no connection is established	
		return is_cnx_established() || get_value() == NONE;
	}
	//! return true is the event is allowed to be synchrnously returned by a itor
	bool is_sync_itor_ok() const throw() {
		// NONE happen when no connection is established	
		return is_cnx_established() || is_cnx_refused() || get_value() == NONE;
	}
	//! return true is the event is allowed to be asynchrnously returned by a itor
	bool is_async_itor_ok() const throw() {
		return is_pkt_to_lower();
	}	
	//! return true is the event is allowed to be returned by a full
	bool is_full_ok() const throw() {
		return is_pkt_to_lower();
	}
friend	std::ostream & operator << (std::ostream & os, const scnx_event_t &scnx_event) throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_EVENT_HPP__  */



