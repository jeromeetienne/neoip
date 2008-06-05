/*! \file
    \brief Header of the \ref acl_dstring_t

\par Brief description
\ref acl_dstring_t is a class helper for \ref acl_t which allows to handle
two strings for identifiers.
- a possible usage is the acl to use for connections
  - aka allowed or not to accept or reject a remote peer during a connection
    establishement.
    
*/


/* system include */
/* local include */
#include "neoip_acl_dstring.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                                 CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
acl_dstring_t::acl_dstring_t()	throw()
{
}

/** \brief Destructor
 */
acl_dstring_t::~acl_dstring_t()	throw()
{
}

/** \brief add an rule in the acl_dstring_t
 */
acl_dstring_t &	acl_dstring_t::add(const acl_action_t &action, const std::string &str1_pattern
					, const std::string &str2_pattern)	throw()
{
	// TODO change this .add in .append() to express the order
	acl_data.add(action, acl_dstring_pattern_t(str1_pattern, str2_pattern) );
	return *this;
}

/** \brief test if a item match a rule in the acl_dstring_t
 */
acl_action_t	acl_dstring_t::find(const std::string str1_name, const std::string str2_name)
										const throw()
{
	KLOG_DBG("try to find str1=" << str1_name << " and " << str2_name);
	return acl_data.find( acl_dstring_name_t(str1_name, str2_name) );
}

/** \brief ostream redirection
 */
std::ostream& operator << (std::ostream & os, const acl_dstring_t &acl_dstring)	throw()
{
	return os << acl_dstring.acl_data;
}

/** \brief Serialization
 */
serial_t& operator << (serial_t& serial, const acl_dstring_t &acl_dstring)	throw()
{
	return serial << acl_dstring.acl_data;
}

/** \brief UnSerialization
 */
serial_t& operator >> (serial_t& serial, acl_dstring_t &acl_dstring) 	throw(serial_except_t)
{
	serial >> acl_dstring.acl_data;
	return serial;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           pattern 
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief constructor with value
 */
acl_dstring_pattern_t::acl_dstring_pattern_t(const std::string &str1_pattern
					, const std::string &str2_pattern)	throw() 
{
	this->str1_pattern = str1_pattern;
	this->str2_pattern = str2_pattern;
}

/** \brief the pattern matching function used by \ref acl_t
 */
bool acl_dstring_pattern_t::pattern_match(const acl_dstring_name_t &name) const throw()
{
	// perform pattern match based on the glob function
	if( !string_t::glob_match(str1_pattern, name.get_str1()) )	return false;
	if( !string_t::glob_match(str2_pattern, name.get_str2()) )	return false;
	return true;
}

/** \brief ostream redirection
 */
std::ostream& operator << (std::ostream & os, const acl_dstring_pattern_t &pattern)	throw()
{
	os << "[" << pattern.str1_pattern;
	os << ", "<< pattern.str2_pattern;
	os << "]";
	return os;
}

/** \brief serialisation
 */
serial_t& operator << (serial_t& serial, const acl_dstring_pattern_t &pattern)	throw()
{
	serial << datum_t(pattern.str1_pattern.c_str(),pattern.str1_pattern.size()+1);
	serial << datum_t(pattern.str2_pattern.c_str(),pattern.str2_pattern.size()+1);
	return serial;
}

/** \brief unserialisation
 */
serial_t& operator >> (serial_t& serial, acl_dstring_pattern_t &pattern) 	throw(serial_except_t)
{
	datum_t	datum;
	serial >> datum;
	pattern.str1_pattern = (char *)datum.get_data();
	serial >> datum;
	pattern.str2_pattern = (char *)datum.get_data();
	return serial;
}

NEOIP_NAMESPACE_END

