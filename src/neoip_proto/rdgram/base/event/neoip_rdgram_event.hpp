/*! \file
    \brief Header of the \ref rdgram_event_t
*/


#ifndef __NEOIP_RDGRAM_EVENT_HPP__ 
#define __NEOIP_RDGRAM_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class pkt_t;

/** \ref class to store the packet type sent by \ref ns-peer
 */
class rdgram_event_t : NEOIP_COPY_CTOR_ALLOW {
public:
	enum type {
		NONE,
		PKT_TO_LOWER,
		MAYSEND_ON,		//!< now data may be sent over the connection
		MAYSEND_OFF,		//!< now data can no more be sent over the connection
		MAX
	};
	pkt_t *		pkt_ptr;
private:
	rdgram_event_t::type	type_val;
public:
	rdgram_event_t()	throw();
	~rdgram_event_t()	throw();
	
	rdgram_event_t::type	get_value()	const throw()	{ return type_val;	}

	bool			is_pkt_to_lower() 		const throw();
	static rdgram_event_t	build_pkt_to_lower(pkt_t *pkt)throw();
	pkt_t *			get_pkt_to_lower()		const throw();

	bool			is_maysend_on() 		const throw();
	static rdgram_event_t	build_maysend_on()		throw();
	
	bool			is_maysend_off() 		const throw();
	static rdgram_event_t	build_maysend_off()		throw();
	
friend	std::ostream & operator << (std::ostream & os, const rdgram_event_t &rdgram_event) throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RDGRAM_EVENT_HPP__  */



