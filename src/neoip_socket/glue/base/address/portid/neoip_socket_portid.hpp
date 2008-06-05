/*! \file
    \brief Header of the socket_portid_t
*/


#ifndef __NEOIP_SOCKET_PORTID_HPP__ 
#define __NEOIP_SOCKET_PORTID_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_socket_portid_vapi.hpp"
#include "neoip_socket_domain.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \ref class to store the socket portid
 */
class socket_portid_t : NEOIP_COPY_CTOR_ALLOW {
private:
	socket_portid_vapi_t *	portid_vapi;	//!< pointer on the domain specific api

	/*************** internal function	*******************************/
	socket_portid_vapi_t *	get_internal_api()	const throw()	{ return portid_vapi;	}
	void			ctor_from_str(const std::string &portid_str)	throw();
public:
	/*************** ctor/dtor	***************************************/
	socket_portid_t(const std::string &portid_str)	throw()	{ ctor_from_str(portid_str);	}
	socket_portid_t(const char *portid_str)		throw()	{ ctor_from_str(std::string(portid_str));}
	socket_portid_t(const socket_portid_vapi_t *portid_vapi)	throw();
	socket_portid_t()				throw();
	~socket_portid_t()			 	throw();

	/*************** object copy stuff	*******************************/
	socket_portid_t(const socket_portid_t &other)			throw();
	socket_portid_t &operator = (const socket_portid_t & other)	throw();

	/*************** Query function	***************************************/
	bool		is_null()			const throw()	{ return !portid_vapi;	}
	socket_domain_t	get_domain()			const throw();

	/*************** comparison operator	*******************************/
	int	compare(const socket_portid_t & other)	  const throw();
	bool	operator ==(const socket_portid_t & other)const throw()	{ return compare(other) == 0; }
	bool	operator !=(const socket_portid_t & other)const throw()	{ return compare(other) != 0; }
	bool	operator < (const socket_portid_t & other)const throw()	{ return compare(other) <  0; }
	bool	operator <=(const socket_portid_t & other)const throw()	{ return compare(other) <= 0; }
	bool	operator > (const socket_portid_t & other)const throw()	{ return compare(other) >  0; }
	bool	operator >=(const socket_portid_t & other)const throw()	{ return compare(other) >= 0; }

	/*************** display function	*******************************/
	std::string	to_string()			const throw();
	friend std::ostream & operator << (std::ostream & os, const socket_portid_t & portid)
							throw()		{ return os << portid.to_string();	}

	/*************** serialization function	*******************************/
	friend serial_t& operator << (serial_t& serial, const socket_portid_t & portid)	throw();
	friend serial_t& operator >> (serial_t& serial, socket_portid_t &portid)	throw(serial_except_t);

	/*************** List of friend function	***********************/
	friend class	socket_addr_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PORTID_HPP__  */



