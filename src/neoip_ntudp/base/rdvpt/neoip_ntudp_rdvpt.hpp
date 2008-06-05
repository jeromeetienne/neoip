/*! \file
    \brief Header of the ntudp_rdvpt_t
    
*/


#ifndef __NEOIP_NTUDP_RDVPT_HPP__ 
#define __NEOIP_NTUDP_RDVPT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_ipport_addr.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN



/** \brief class definition for ntudp_rdvpt_t
 */
class ntudp_rdvpt_t : NEOIP_COPY_CTOR_ALLOW {
public:	enum type_t {
		NONE,
		RELAY,
		DIRECT,
		MAX
	};
private:
	ipport_addr_t	ipport_addr;	//!< the ipport_addr_t of the ntudp_rdvpt_t
	type_t		rdvpt_type;	//!< the type of the ntudp_rdvpt_t
public:
	/*************** ctor/dtor	***************************************/
	ntudp_rdvpt_t()										throw()	{}
	ntudp_rdvpt_t(const ipport_addr_t &ipport_addr, const ntudp_rdvpt_t::type_t &rdvpt_type)throw();

	/*************** Query function	***************************************/
	bool				is_null()	const throw() { return ipport_addr.is_null();	}
	const ipport_addr_t &		get_addr()	const throw() { return ipport_addr;		}
	const ntudp_rdvpt_t::type_t &	get_type()	const throw() { return rdvpt_type;		}

	/*************** Comparison Operator	*******************************/
	int	compare(const ntudp_rdvpt_t &other)	const throw();
	bool	operator==(const ntudp_rdvpt_t &other)	const throw() { return compare(other) == 0; }
	bool	operator!=(const ntudp_rdvpt_t &other)	const throw() { return compare(other) != 0; }
	bool	operator< (const ntudp_rdvpt_t &other)	const throw() { return compare(other) <  0; }
	bool	operator<=(const ntudp_rdvpt_t &other)	const throw() { return compare(other) <= 0; }
	bool	operator> (const ntudp_rdvpt_t &other)	const throw() { return compare(other) >  0; }
	bool	operator>=(const ntudp_rdvpt_t &other)	const throw() { return compare(other) >= 0; }
	
	/*************** display function	*******************************/
	std::string	to_string()			const throw();
	friend	std::ostream & operator << (std::ostream & os, const ntudp_rdvpt_t &ntudp_rdvpt ) throw()
						{ return os << ntudp_rdvpt.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const ntudp_rdvpt_t &ntudp_rdvpt)	throw();
	friend	serial_t& operator >> (serial_t & serial, ntudp_rdvpt_t &ntudp_rdvpt)		throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_RDVPT_HPP__  */



