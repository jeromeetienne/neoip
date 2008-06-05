/*! \file
    \brief Header of the \ref acl_t

- TODO to use STRTYPE for this one
*/


#ifndef __NEOIP_ACL_ACTION_HPP__ 
#define __NEOIP_ACL_ACTION_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_namespace.hpp"
#include "neoip_serial.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the packet type sent by \ref ns-peer
 */
class acl_action_t {
public:
	enum type {
		NONE,
		ALLOW,
		DENY,
		ASK,
		MAX
	};
private:
	acl_action_t::type	type_val;
public:
	// ctor/dtor
	acl_action_t( void ) : type_val(NONE) {}
	acl_action_t( acl_action_t::type param_type ) : type_val(param_type) {}
	// asignement operator
	acl_action_t &operator =( acl_action_t::type param_type ){
		type_val = param_type;
		return *this;
	}
	// comparison operator
	bool operator == ( const acl_action_t & other ) const { return this->type_val == other.type_val; }
	bool operator != ( const acl_action_t & other ) const { return this->type_val != other.type_val; }
	
	// comparison short-cut
	bool	is_allow(void)	const { return type_val == ALLOW;	}
	bool	is_deny(void)	const { return type_val == DENY;	}
	bool	is_ask(void)	const { return type_val == ASK;		}

friend	std::ostream& operator << (std::ostream & os, const acl_action_t & type)		throw();
friend	serial_t& operator << ( serial_t& serial, const acl_action_t &acl_action_t)	throw();
friend	serial_t& operator >> ( serial_t& serial, acl_action_t &acl_action_t )   
										throw(serial_except_t);
};

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_ACL_ACTION_HPP__  */










