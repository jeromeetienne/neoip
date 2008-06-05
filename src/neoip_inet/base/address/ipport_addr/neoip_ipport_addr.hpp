/*! \file
    \brief Header of the ipport_addr_t
    
*/


#ifndef __NEOIP_IPPORT_ADDR_HPP__ 
#define __NEOIP_IPPORT_ADDR_HPP__ 
/* system include */
#include <iostream>
#ifndef _WIN32
#	include <netinet/in.h>
#endif
/* local include */
#include "neoip_ip_addr.hpp"
#include "neoip_serial.hpp"
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for ipport_addr_t
 * 
 * - this class is able to support a ip_addr with a 16-bit port, so suitable for udp/tcp
 */
class ipport_addr_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the ANY ipport_addr_t aka "0.0.0.0:0" for ipv4
	static const ipport_addr_t	ANY_IP4;	
private:
	ip_addr_t	m_ipaddr;	//!< the ip address 
	uint16_t	m_port;		//!< the port
	/*************** Internal function	*******************************/
	bool		from_string(const std::string &ipport_str)		throw();
public:
	/*************** ctor/dtor	***************************************/
	ipport_addr_t()		throw()	{ nullify();	}
	ipport_addr_t(const ip_addr_t &new_ipaddr, uint16_t newport)		throw();
	ipport_addr_t(const char * ipport_str)					throw();
	ipport_addr_t(const std::string &ipport_str)				throw();

	/*************** sockaddr_in functions	*******************************/
	explicit ipport_addr_t(const struct sockaddr_in &sa_in)			throw();
	struct 	sockaddr_in	to_sockaddr_in() 				const throw();

	/*************** query function	***************************************/
	bool			is_null()		const throw()	{ return m_ipaddr.is_null();	}
	void			nullify()		throw()		{ m_ipaddr = ip_addr_t();
									  m_port = 0;			}
	const ip_addr_t &	ipaddr()		const throw()	{ return m_ipaddr;		}
	const uint16_t &	port()			const throw()	{ return m_port;		}
	bool			is_fully_qualified()	const throw()	{ return ipaddr().is_fully_qualified() && port();}
	bool			is_aport_qualified()	const throw()	{ return ipaddr().is_any() && port();}
	ipport_addr_t &		ipaddr(const ip_addr_t &new_ipaddr)	throw()	{ this->m_ipaddr = new_ipaddr;	return *this;	}
	ipport_addr_t &		port(const uint16_t new_port)		throw()	{ this->m_port = new_port;	return *this;	}

	/*************** Compatibility layer	*******************************/
	const ip_addr_t &	get_ipaddr()		const throw()	{ return ipaddr();		}
	const uint16_t &	get_port()		const throw()	{ return port();		}
	ipport_addr_t &		set_ipaddr(const ip_addr_t &new_ipaddr)	throw()	{ return ipaddr(new_ipaddr);	}
	ipport_addr_t &		set_port(const uint16_t &new_port)	throw()	{ return port(new_port);	}

	/*************** Comparison Operator	*******************************/
	int	compare(const ipport_addr_t &other)			const throw();
	bool	operator == (const ipport_addr_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const ipport_addr_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const ipport_addr_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const ipport_addr_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const ipport_addr_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const ipport_addr_t &other) const throw() { return compare(other) >= 0; }
	
	/*************** display function	*******************************/
	std::string	to_string()						const throw();
	friend	std::ostream & operator << (std::ostream & os, const ipport_addr_t &ipport_addr ) throw()
			{ return os << ipport_addr.to_string();	}
	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const ipport_addr_t &ipport_addr)	throw();
	friend	serial_t& operator >> (serial_t & serial, ipport_addr_t &ipport_addr)		throw(serial_except_t);	
	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const ipport_addr_t &ipport_addr)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, ipport_addr_t &ipport_addr)		throw(xml_except_t);

};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IPPORT_ADDR_HPP__  */



