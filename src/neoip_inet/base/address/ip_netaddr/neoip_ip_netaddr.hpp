/*! \file
    \brief Header of the ip_netaddr_t
    
*/


#ifndef __NEOIP_IP_NETADDR_HPP__ 
#define __NEOIP_IP_NETADDR_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_ip_addr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for ip_netaddr_t
 */
class ip_netaddr_t {
private:
	ip_addr_t	base_addr;
	uint8_t		prefix_len;
	
	bool		from_string( const std::string &ip_netaddr_str )	throw();
public:
	/*************** ctor/dtor	***************************************/
	ip_netaddr_t() 	throw()	{ nullify(); }
	ip_netaddr_t(const ip_addr_t &ip_addr, int prefix_len)			throw();
	ip_netaddr_t(const char *netaddr_str)					throw();
	ip_netaddr_t(const std::string &netaddr_str)				throw();

	/*************** null function	***************************************/
	bool		is_null()	const throw()	{ return base_addr.is_null();	}
	void		nullify()	throw()		{ base_addr = ip_addr_t();	}
	
	/*************** query function	***************************************/
	ip_addr_t		get_netmask()		const throw();
	ip_addr_t		get_any_addr()		const throw();
	ip_addr_t		get_bcast_addr()	const throw();
	const ip_addr_t &	get_base_addr() 	const throw()	{ return base_addr; }
	const uint8_t &		get_prefix_len()	const throw()	{ return prefix_len; }
	bool			contain(const ip_addr_t &ip_addr)	const throw();	

	/*************** 'cursor' function	*******************************/
	ip_addr_t		get_first_addr()			const throw();
	ip_addr_t		get_last_addr()				const throw();
	ip_addr_t		get_next_addr(const ip_addr_t &ipaddr)	const throw();

	/*************** Comparison operator	*******************************/
	int	compare(const ip_netaddr_t &other)			const throw();
	bool	operator == (const ip_netaddr_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const ip_netaddr_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const ip_netaddr_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const ip_netaddr_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const ip_netaddr_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const ip_netaddr_t &other) const throw() { return compare(other) >= 0; }
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const ip_netaddr_t &ip_netaddr ) throw()
			{ return os << ip_netaddr.to_string();	}
	/*************** serialization function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const ip_netaddr_t &ip_netaddr)throw();
	friend	serial_t& operator >> (serial_t & serial, ip_netaddr_t &ip_netaddr)	throw(serial_except_t);	
		
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IP_NETADDR_HPP__  */



