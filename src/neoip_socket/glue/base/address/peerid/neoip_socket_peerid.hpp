/*! \file
    \brief Header of the socket_peerid_t
*/


#ifndef __NEOIP_SOCKET_PEERID_HPP__ 
#define __NEOIP_SOCKET_PEERID_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_socket_peerid_vapi.hpp"
#include "neoip_socket_domain.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \ref class to store the socket peerid
 */
class socket_peerid_t : NEOIP_COPY_CTOR_ALLOW {
private:
	socket_peerid_vapi_t *	peerid_vapi;	//!< pointer on the domain specific api

	/*************** internal function	*******************************/
	socket_peerid_vapi_t *	get_internal_api()	const throw()	{ return peerid_vapi;	}
	void			ctor_from_str(const std::string &peerid_str)	throw();
public:
	/*************** ctor/dtor	***************************************/
	socket_peerid_t(const std::string &peerid_str)	throw()	{ ctor_from_str(peerid_str);	}
	socket_peerid_t(const char *peerid_str)		throw()	{ ctor_from_str(std::string(peerid_str));}
	socket_peerid_t(const socket_peerid_vapi_t *peerid_vapi)	throw();
	socket_peerid_t()				throw();
	~socket_peerid_t()			 	throw();

	/*************** object copy stuff	*******************************/
	socket_peerid_t(const socket_peerid_t &other)			throw();
	socket_peerid_t &operator = (const socket_peerid_t & other)	throw();

	/*************** Query function	***************************************/
	socket_domain_t	get_domain()	const throw();
	bool		is_null()	const throw()	{ return !peerid_vapi;	}
	bool		is_localhost()	const throw()	{ DBG_ASSERT(peerid_vapi); return peerid_vapi->is_localhost();	}
	bool		is_linklocal()	const throw()	{ DBG_ASSERT(peerid_vapi); return peerid_vapi->is_linklocal();	}
	bool		is_private()	const throw()	{ DBG_ASSERT(peerid_vapi); return peerid_vapi->is_private();	}
	bool		is_public()	const throw()	{ DBG_ASSERT(peerid_vapi); return peerid_vapi->is_public();	}
	bool		is_any()	const throw()	{ DBG_ASSERT(peerid_vapi); return peerid_vapi->is_any();	}

	/*************** comparison operator	*******************************/
	int	compare(const socket_peerid_t & other)	  const throw();
	bool	operator ==(const socket_peerid_t & other)const throw()	{ return compare(other) == 0; }
	bool	operator !=(const socket_peerid_t & other)const throw()	{ return compare(other) != 0; }
	bool	operator < (const socket_peerid_t & other)const throw()	{ return compare(other) <  0; }
	bool	operator <=(const socket_peerid_t & other)const throw()	{ return compare(other) <= 0; }
	bool	operator > (const socket_peerid_t & other)const throw()	{ return compare(other) >  0; }
	bool	operator >=(const socket_peerid_t & other)const throw()	{ return compare(other) >= 0; }

	/*************** display function	*******************************/
	std::string	to_string()			const throw();
	friend std::ostream & operator << (std::ostream & os, const socket_peerid_t & peerid)
							throw()		{ return os << peerid.to_string();	}

	/*************** serialization function	*******************************/
	friend serial_t& operator << (serial_t& serial, const socket_peerid_t & peerid)	throw();
	friend serial_t& operator >> (serial_t& serial, socket_peerid_t &peerid)	throw(serial_except_t);

	/*************** List of friend function	***********************/
	friend class	socket_addr_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PEERID_HPP__  */



