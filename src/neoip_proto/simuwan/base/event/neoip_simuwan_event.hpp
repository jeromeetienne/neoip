/*! \file
    \brief Header of the \ref simuwan_event_t
*/


#ifndef __NEOIP_SIMUWAN_EVENT_HPP__ 
#define __NEOIP_SIMUWAN_EVENT_HPP__ 
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
class simuwan_event_t : NEOIP_COPY_CTOR_ALLOW {
public:
	enum type {
		NONE,
		PKT_TO_LOWER,
		MAX
	};
	pkt_t *		pkt_ptr;
private:
	simuwan_event_t::type	type_val;
public:
	simuwan_event_t()	throw();
	~simuwan_event_t()	throw();
	
	simuwan_event_t::type	get_value()	const throw()	{ return type_val;	}

	bool			is_pkt_to_lower() 		const throw();
	static simuwan_event_t	build_pkt_to_lower(pkt_t *pkt)throw();
	pkt_t *			get_pkt_to_lower()		const throw();

friend	std::ostream & operator << (std::ostream & os, const simuwan_event_t &simuwan_event) throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SIMUWAN_EVENT_HPP__  */



