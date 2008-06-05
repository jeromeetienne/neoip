/*! \file
    \brief Header of the \ref nslan_event_t
*/


#ifndef __NEOIP_NSLAN_EVENT_HPP__ 
#define __NEOIP_NSLAN_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_nslan_rec.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class pkt_t;

/** \ref class to store the packet type sent by \ref ns-peer
 */
class nslan_event_t : NEOIP_COPY_CTOR_ALLOW {
public:
	enum type {
		NONE,
		TIMEDOUT,		//!< When the query timed out
		GOT_RECORD,		//!< When a record is reported
		MAX
	};
	nslan_rec_t	nslan_rec;
	ipport_addr_t	nslan_rec_source_addr;
	std::string	reason;
private:
	nslan_event_t::type	type_val;
public:
	/*************** ctor/dtor	***************************************/
	nslan_event_t()		throw();
	~nslan_event_t()	throw();
	
	nslan_event_t::type	get_value()	const throw()	{ return type_val;	}

	bool			is_timedout() 					const throw();
	static nslan_event_t	build_timedout(const std::string &reason)	throw();
	const std::string &	get_timedout_reason()				const throw();

	bool			is_got_record() 				const throw();
	static nslan_event_t	build_got_record(const nslan_rec_t &nslan_rec
					, const ipport_addr_t &src_addr)	throw();
	const nslan_rec_t &	get_got_record(ipport_addr_t *src_addr)		const throw();
	
	//!< return true if the event MAY be returned by a query
	bool is_query_ok() const throw()
		{ return is_timedout() || is_got_record();	}
	
	/*************** Display function	*******************************/
	friend	std::ostream & operator << (std::ostream & os, const nslan_event_t &nslan_event) throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NSLAN_EVENT_HPP__  */



