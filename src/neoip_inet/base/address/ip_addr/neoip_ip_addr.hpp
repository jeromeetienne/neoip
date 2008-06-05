/*! \file
    \brief Header of the ip_addr_t
    
*/


#ifndef __NEOIP_IP_ADDR_HPP__ 
#define __NEOIP_IP_ADDR_HPP__ 
/* system include */
#include <iostream>
#include <string>
#ifndef _WIN32
#	include <netinet/in.h>
#endif
/* local include */
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for ip_addr_t
 */
class ip_addr_t : NEOIP_COPY_CTOR_ALLOW {
private:
	uint8_t	version;	//!< the version of the ip_addr_t. == 4 for IPv4 or == 6 for IPv6
	union	{
		uint32_t	v4;	// ipv4 address stored in local order
	} address;

	bool		from_string(const std::string &ip_addr_str)   throw();
public:
	/*************** ctor/dtor	***************************************/
	ip_addr_t() 				throw()	{ nullify();	}
	ip_addr_t(const char *ip_addr_str)  	throw();
	ip_addr_t(const std::string &str)  	throw();
	
	/*************** null function	***************************************/
	bool		is_null()		const throw()	{ return version == 0;			}
	void		nullify()		throw()		{ version = 0;				}
	/*************** sockaddr_in functions	*******************************/
	explicit ip_addr_t(const struct sockaddr_in &sa_in)			throw();
	struct 	sockaddr_in	to_sockaddr_in() 				const throw();

	/*************** query for type of ip_addr_t	***********************/
	bool	is_fully_qualified()	const throw();
	bool	is_any()		const throw();
	bool	is_multicast()		const throw();
	bool	is_broadcast()		const throw();
	bool	is_localhost()		const throw();
	bool	is_linklocal()		const throw();
	bool	is_private()		const throw();
	bool	is_public()		const throw();

	/*************** query the version	*******************************/
	bool	is_v4()		const throw()	{ return version == 4;	}
	bool	is_v6()		const throw()	{ return version == 6;	}
	int	get_version() 	const throw()	{ return version;	}

	/************** function specific to ipv4	***********************/
	uint32_t	get_v4_addr() const throw() { DBG_ASSERT( is_v4() ); return address.v4;	}
	explicit ip_addr_t(uint32_t v4_addr) throw(){ version = 4; address.v4 = v4_addr;	}

	/************** Arithmetic operator	*******************************/
	ip_addr_t	operator+ (const uint32_t val)	const throw();
	ip_addr_t	operator- (const uint32_t val)	const throw();
	void		operator+=(const uint32_t val)	throw() { *this = *this + val; }
	void		operator-=(const uint32_t val)	throw() { *this = *this - val; }	
	ip_addr_t &	operator ++ ()			throw()	{ *this += 1; return *this;			}
	ip_addr_t 	operator ++ (int dummy)		throw()	{ ip_addr_t tmp(*this); tmp += 1; return tmp;	}
	ip_addr_t &	operator -- ()			throw()	{ *this -= 1; return *this;			}
	ip_addr_t 	operator -- (int dummy)		throw()	{ ip_addr_t tmp(*this); tmp -= 1; return tmp;	}


	/************** comparison operator	*******************************/
	int	compare( const ip_addr_t &other )	const throw();
	bool	operator == (const ip_addr_t &other)	const throw() { return compare(other) == 0; }
	bool	operator != (const ip_addr_t &other)	const throw() { return compare(other) != 0; }
	bool	operator <  (const ip_addr_t &other)	const throw() { return compare(other) <  0; }
	bool	operator <= (const ip_addr_t &other)	const throw() { return compare(other) <= 0; }
	bool	operator >  (const ip_addr_t &other)	const throw() { return compare(other) >  0; }
	bool	operator >= (const ip_addr_t &other)	const throw() { return compare(other) >= 0; }

	/************** display function	*******************************/
	std::string	to_string()			const throw();
	friend	std::ostream & operator << (std::ostream & os, const ip_addr_t &ip_addr ) throw()
						{ return os << ip_addr.to_string();	}
	
	/************** serialization	***************************************/
	friend	serial_t& operator << (serial_t& serial, const ip_addr_t &ip_addr)	throw();
	friend	serial_t& operator >> (serial_t & serial, ip_addr_t &ip_addr)	throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IP_ADDR_HPP__  */



