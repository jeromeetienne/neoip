/*! \file
    \brief Header of the socket_addr_t
*/


#ifndef __NEOIP_SOCKET_ADDR_HPP__ 
#define __NEOIP_SOCKET_ADDR_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_socket_domain.hpp"
#include "neoip_socket_peerid.hpp"
#include "neoip_socket_portid.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_peerid_vapi_t;
class	socket_portid_vapi_t;

/** \ref class to store the socket peerid
 */
class socket_addr_t : NEOIP_COPY_CTOR_ALLOW {
private:
	socket_peerid_vapi_t *	m_peerid_vapi;	//!< pointer on the domain specific api for peerid
	socket_portid_vapi_t *	m_portid_vapi;	//!< pointer on the domain specific api for portid

	/*************** internal function	*******************************/
	void			ctor_from_str(const std::string &addr_str)	throw();
public:
	/*************** ctor/dtor	***************************************/
	socket_addr_t(const std::string &addr_str)	throw()	{ ctor_from_str(addr_str);	}
	socket_addr_t(const char *addr_str)		throw()	{ ctor_from_str(addr_str);	}
	socket_addr_t(const socket_peerid_t &m_peerid, const socket_portid_t &m_portid)	throw();
	socket_addr_t()					throw();
	~socket_addr_t()			 	throw();

	/*************** object copy stuff	*******************************/
	socket_addr_t(const socket_addr_t &other)			throw();
	socket_addr_t &operator = (const socket_addr_t & other)		throw();

	/*************** Query function	***************************************/
	socket_domain_t	get_domain()	const throw();
	bool		is_null()	const throw()	{ return !m_peerid_vapi;			}
	socket_peerid_t	get_peerid()	const throw()	{ return socket_peerid_t(m_peerid_vapi);	}
	socket_portid_t	get_portid()	const throw()	{ return socket_portid_t(m_portid_vapi);	}

	/*************** access to internal vapi	***********************/
	socket_peerid_vapi_t *	get_peerid_vapi()	const throw()	{ return m_peerid_vapi;	}
	socket_portid_vapi_t *	get_portid_vapi()	const throw()	{ return m_portid_vapi;	}

	/*************** comparison operator	*******************************/
	int	compare(const socket_addr_t & other)	const throw();
	bool	operator ==(const socket_addr_t & other)const throw()	{ return compare(other) == 0; }
	bool	operator !=(const socket_addr_t & other)const throw()	{ return compare(other) != 0; }
	bool	operator < (const socket_addr_t & other)const throw()	{ return compare(other) <  0; }
	bool	operator <=(const socket_addr_t & other)const throw()	{ return compare(other) <= 0; }
	bool	operator > (const socket_addr_t & other)const throw()	{ return compare(other) >  0; }
	bool	operator >=(const socket_addr_t & other)const throw()	{ return compare(other) >= 0; }

	/*************** display function	*******************************/
	std::string		to_string()		const throw();
	friend std::ostream & operator << (std::ostream & os, const socket_addr_t & addr)
							throw()		{ return os << addr.to_string();	}

	/*************** serialization function	*******************************/
	friend serial_t& operator << (serial_t& serial, const socket_addr_t & addr)	throw();
	friend serial_t& operator >> (serial_t& serial, socket_addr_t &addr)	throw(serial_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_ADDR_HPP__  */



