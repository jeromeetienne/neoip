/*! \file
    \brief Header of the \ref bt_utmsg_event_t
*/


#ifndef __NEOIP_BT_UTMSG_EVENT_HPP__ 
#define __NEOIP_BT_UTMSG_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the event from bt_utmsg_t
 */
class bt_utmsg_event_t {
public:
	enum type {
		NONE,
		DOREGISTER,
		UNREGISTER,
		MAX
	};
private:
	bt_utmsg_event_t::type	type_val;
public:
	/*************** ctor/dtor	***************************************/
	bt_utmsg_event_t()	throw();
	~bt_utmsg_event_t()	throw();
	
	/*************** query function	***************************************/
	bt_utmsg_event_t::type	get_value()	const throw()	{ return type_val;	}

	/*************** specific event ctor/dtor/query	***********************/
	bool				is_doregister()				const throw();
	static bt_utmsg_event_t 	build_doregister()			throw();
		
	bool				is_unregister()				const throw();
	static bt_utmsg_event_t 	build_unregister()			throw();

	/*************** display function	*******************************/
	std::string			to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_utmsg_event_t &event) throw()
					{ return oss << event.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_EVENT_HPP__  */



