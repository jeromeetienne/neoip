/*! \file
    \brief Header of the ntudp_addr_t
    
*/


#ifndef __NEOIP_NTUDP_ADDR_HPP__ 
#define __NEOIP_NTUDP_ADDR_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_peerid.hpp"
#include "neoip_ntudp_portid.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for ntudp_addr_t
 */
class ntudp_addr_t : NEOIP_COPY_CTOR_ALLOW {
private:
	ntudp_peerid_t	m_peerid;	//!< the peerid part of the ntudp_addr_t
	ntudp_portid_t	m_portid;	//!< the portid part of the ntudp_addr_t
	
	/*************** Internal function	*******************************/
	bool		from_string(const std::string &addr_str)		throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_addr_t()								throw()	{}
	ntudp_addr_t(const ntudp_peerid_t &m_peerid, const ntudp_portid_t &m_portid)	throw();
	ntudp_addr_t(const char *addr_str)					throw();

	/*************** Query function	***************************************/
	bool			is_null()		const throw()	{ return peerid().is_null() && portid().is_null();	}
	void			nullify()		throw()		{ *this = ntudp_addr_t();	}
	bool			is_fully_qualified()	const throw()	{ return peerid().is_fully_qualified() && portid().is_fully_qualified();}
	const ntudp_peerid_t &	peerid()		const throw()	{ return m_peerid;		}
	const ntudp_portid_t &	portid()		const throw()	{ return m_portid;		}
	ntudp_addr_t &		peerid(const ntudp_peerid_t &new_peerid) throw()	{ m_peerid = new_peerid; return *this;}
	ntudp_addr_t &		portid(const ntudp_portid_t &new_portid) throw()	{ m_portid = new_portid; return *this;}

	/*************** Comparison Operator	*******************************/
	int	compare(const ntudp_addr_t &other)	const throw();
	bool	operator == (const ntudp_addr_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const ntudp_addr_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const ntudp_addr_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const ntudp_addr_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const ntudp_addr_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const ntudp_addr_t &other) const throw() { return compare(other) >= 0; }
	
	/*************** display function	*******************************/
	std::string	to_string()			const throw();
	friend	std::ostream & operator << (std::ostream & os, const ntudp_addr_t &ntudp_addr ) throw()
							{ return os << ntudp_addr.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const ntudp_addr_t &ntudp_addr)throw();
	friend	serial_t& operator >> (serial_t & serial, ntudp_addr_t &ntudp_addr)	throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_ADDR_HPP__  */



