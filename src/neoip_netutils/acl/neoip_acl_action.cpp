/*! \file
    \brief Definition of the \ref acl_action_t
    
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_acl_action.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   OSTREAM redirection (mainly for debug)
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection (mainly for debug)
 */
std::ostream & operator << (std::ostream & os, const acl_action_t & type)	throw()
{
	switch( type.type_val ){
	case acl_action_t::NONE:	os << "NONE";		break;
	case acl_action_t::ALLOW:	os << "ALLOW";		break;
	case acl_action_t::DENY:	os << "DENY";		break;
	case acl_action_t::ASK:		os << "ASK";		break;
	default: 	DBG_ASSERT(0);
	}
	return os;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   SERIALIZATION
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief serialization of acl_action_t
 */
serial_t& operator << (serial_t& serial, const acl_action_t &acl_action)	throw()
{
	serial	<< (uint8_t)acl_action.type_val;
	return serial;
}

/** \brief unserialization of acl_action_t
 */
serial_t& operator >> ( serial_t& serial, acl_action_t &acl_action )	throw(serial_except_t)
{
	uint8_t		value;
	serial		>> value;
	acl_action.type_val	= (acl_action_t::type)value;
	return serial;
}

NEOIP_NAMESPACE_END
