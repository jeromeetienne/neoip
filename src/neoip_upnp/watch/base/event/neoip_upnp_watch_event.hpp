/*! \file
    \brief Header of the \ref upnp_watch_event_t
*/


#ifndef __NEOIP_UPNP_WATCH_EVENT_HPP__ 
#define __NEOIP_UPNP_WATCH_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the event from upnp_watch_t
 */
class upnp_watch_event_t {
public:
	enum type {
		NONE,
		UPNPDISC_CHANGED,
		EXTIPADDR_CHANGED,
		MAX
	};
private:
	upnp_watch_event_t::type	type_val;
public:
	/*************** ctor/dtor	***************************************/
	upnp_watch_event_t()	throw();
	~upnp_watch_event_t()	throw();
	
	/*************** query function	***************************************/
	upnp_watch_event_t::type	get_value()	const throw()	{ return type_val;	}

	/*************** specific event ctor/dtor/query	***********************/
	bool				is_upnpdisc_changed()			const throw();
	static upnp_watch_event_t 	build_upnpdisc_changed()		throw();
		
	bool				is_extipaddr_changed()			const throw();
	static upnp_watch_event_t 	build_extipaddr_changed()		throw();

	/*************** display function	*******************************/
	std::string			to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const upnp_watch_event_t &event) throw()
					{ return oss << event.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_WATCH_EVENT_HPP__  */



