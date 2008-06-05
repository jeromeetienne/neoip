/*! \file
    \brief Header of the ntudp_pserver_rec_t
    
*/


#ifndef __NEOIP_NTUDP_PSERVER_REC_HPP__ 
#define __NEOIP_NTUDP_PSERVER_REC_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_peerid.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for ntudp_pserver_rec_t
 */
class ntudp_pserver_rec_t : NEOIP_COPY_CTOR_ALLOW {
private:
	ipport_addr_t	m_listen_addr_pview;
	ntudp_peerid_t	m_peerid;
public:
	/*************** ctor/dtor	***************************************/
	ntudp_pserver_rec_t()							throw()	{}
	ntudp_pserver_rec_t(const ipport_addr_t &m_listen_addr_pview
					, const ntudp_peerid_t &m_peerid)	throw();

	/*************** Query function	***************************************/
	bool	is_null()	const throw()	{ return peerid().is_null() && listen_addr_pview().is_null();	}
	const ipport_addr_t &	listen_addr_pview()	const throw() { return m_listen_addr_pview;		}
	const ntudp_peerid_t &	peerid()		const throw() { return m_peerid;			}

	/*************** Comparison Operator	*******************************/
	int	compare(const ntudp_pserver_rec_t &other)	const throw();
	bool	operator == (const ntudp_pserver_rec_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const ntudp_pserver_rec_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const ntudp_pserver_rec_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const ntudp_pserver_rec_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const ntudp_pserver_rec_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const ntudp_pserver_rec_t &other) const throw() { return compare(other) >= 0; }
	
	/*************** display function	*******************************/
	std::string		to_string()			const throw();
	friend	std::ostream & operator << (std::ostream & os, const ntudp_pserver_rec_t &ntudp_pserver_rec ) throw()
						{ return os << ntudp_pserver_rec.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const ntudp_pserver_rec_t &ntudp_pserver_rec)	throw();
	friend	serial_t& operator >> (serial_t & serial, ntudp_pserver_rec_t &ntudp_pserver_rec)	throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PSERVER_REC_HPP__  */



