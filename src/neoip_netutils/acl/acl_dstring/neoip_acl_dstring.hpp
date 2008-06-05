/*! \file
    \brief Header of the \ref acl_dstring_t
    
*/


#ifndef __NEOIP_ACL_DSTRING_HPP__ 
#define __NEOIP_ACL_DSTRING_HPP__ 
/* system include */
#include <iostream>
#include <list>
/* local include */
#include "neoip_acl.hpp"
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
class acl_dstring_name_t;
class acl_dstring_pattern_t;

class acl_dstring_t {
private:
	acl_t<acl_dstring_name_t, acl_dstring_pattern_t>	acl_data;
public:
	acl_dstring_t()		throw();
	~acl_dstring_t()	throw();
	acl_dstring_t &		add(const acl_action_t &action, const std::string &str1_pattern
					, const std::string &str2_pattern)	throw();
	acl_action_t		find(const std::string str1_name, const std::string str2_name)
										const throw();

friend std::ostream& operator << (std::ostream & os, const acl_dstring_t &acl_dstring)	throw();
friend serial_t& operator << (serial_t& serial, const acl_dstring_t &acl_dstring)	throw();
friend serial_t& operator >> (serial_t& serial, acl_dstring_t &acl_dstring) 	throw(serial_except_t);
};


/** \brief dummy class to store operation name for acl_utest
 */
class acl_dstring_name_t {
private:
	std::string	str1_name;
	std::string	str2_name;
public:
	acl_dstring_name_t(const std::string &str1_name, const std::string str2_name)	throw() 
		: str1_name(str1_name), str2_name(str2_name) {}
	
	const std::string &	get_str1() const throw()	{ return str1_name;	}
	const std::string &	get_str2() const throw()	{ return str2_name;	}
};

/** \brief dummy class to store operation pattern for acl_utest
 */
class acl_dstring_pattern_t {
private:
	std::string	str1_pattern;
	std::string	str2_pattern;
public:
	acl_dstring_pattern_t()	throw()	{}
	acl_dstring_pattern_t(const std::string &str1_pattern, const std::string &str2_pattern)	throw();

	bool pattern_match(const acl_dstring_name_t &name) const throw();

friend std::ostream& operator << (std::ostream & os, const acl_dstring_pattern_t &pattern)	throw();
friend serial_t& operator << (serial_t& serial, const acl_dstring_pattern_t &pattern)	throw();
friend serial_t& operator >> (serial_t& serial, acl_dstring_pattern_t &pattern) 	throw(serial_except_t);
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_ACL_DSTRING_HPP__ 
