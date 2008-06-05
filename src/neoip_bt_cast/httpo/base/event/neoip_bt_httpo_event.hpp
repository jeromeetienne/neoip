/*! \file
    \brief Header of the \ref bt_httpo_event_t
*/


#ifndef __NEOIP_BT_HTTPO_EVENT_HPP__ 
#define __NEOIP_BT_HTTPO_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_httpo_full_t;

/** \ref class to store the event from bt_httpo_event_t
 */
class bt_httpo_event_t {
public:
	enum type {
		NONE,
		CNX_ESTABLISHED,
		CNX_CLOSED,
		MAX
	};
private:
	bt_httpo_event_t::type	type_val;
	
	bt_httpo_full_t *	httpo_full;
public:
	/*************** ctor/dtor	***************************************/
	bt_httpo_event_t()	throw();
	~bt_httpo_event_t()	throw();
	
	/*************** query function	***************************************/
	bt_httpo_event_t::type	get_value()	const throw()	{ return type_val;		}
	bool			is_fatal()	const throw()	{ return is_cnx_closed();	}
	/*************** specific event ctor/dtor/query	***********************/
	bool			is_cnx_established() 				const throw();
	static bt_httpo_event_t	build_cnx_established(bt_httpo_full_t *	httpo_full)   throw();
	bt_httpo_full_t *	get_cnx_established()				const throw();

	bool			is_cnx_closed() 				const throw();
	static bt_httpo_event_t	build_cnx_closed()				throw();

	//! return true is the event is allowed to be returned by a resp
	bool is_resp_ok() const throw() {
		return is_cnx_established();
	}
	//! return true is the event is allowed to be returned by a full
	bool is_full_ok() const throw() {
		return is_cnx_closed();
	}

	/*************** display function	*******************************/
	std::string			to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_httpo_event_t &event) throw()
					{ return oss << event.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTPO_EVENT_HPP__  */



