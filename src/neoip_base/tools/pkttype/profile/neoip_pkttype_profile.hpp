/*! \file
    \brief Header of the pkttype_profile_t
    
*/


#ifndef __NEOIP_PKTTYPE_PROFILE_HPP__ 
#define __NEOIP_PKTTYPE_PROFILE_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_peerid.hpp"
#include "neoip_ntudp_portid.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for pkttype_profile_t
 */
class pkttype_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:
	enum serial_type_t {
		NONE,
		UINT8,		//!< to serialize on unsigned 8-bit
		UINT16,		//!< to serialize on unsigned 16-bit
		UINT32,		//!< to serialize on unsigned 32-bit
		MAX
	};
private:
	serial_type_t	serial_type_val;	//!< the type of serialization
	size_t		offset_val;		//!< the offset to apply to the pkttype index
	size_t		nb_reserved_val;	//!< the number of reserved pkttype
public:
	/*************** ctor/dtor	***************************************/
	pkttype_profile_t()								throw();
	pkttype_profile_t(size_t offset, size_t nb_reserved, serial_type_t serial_type)	throw();

	/*************** Setup function	***************************************/
	pkttype_profile_t &	offset(size_t offset) throw()		{ offset_val = offset; return *this;}
	const size_t		offset()	const throw()		{ return offset_val;		}
	pkttype_profile_t &	nb_reserved(size_t nb_reserved) 	throw()
							{ nb_reserved_val = nb_reserved; return *this;}
	const size_t		nb_reserved()				const throw()
							{ return nb_reserved_val;	}
	pkttype_profile_t &	serial_type(serial_type_t serial_type)	throw()
							{ serial_type_val = serial_type; return *this;}
	const pkttype_profile_t::serial_type_t	serial_type()		const throw()
							{ return serial_type_val;			}

	/*************** Query function	***************************************/
	//! return true if the pkttype_profile_t is null
	bool	is_null()	const throw()	{ return serial_type_val == NONE;	}

	/*************** Comparison function	*******************************/
	int	compare(const pkttype_profile_t &other)	  const throw();
	bool 	operator == (const pkttype_profile_t & other) const throw() { return compare(other) == 0;	}
	bool 	operator != (const pkttype_profile_t & other) const throw() { return compare(other) != 0;	}
	bool 	operator <  (const pkttype_profile_t & other) const throw() { return compare(other) <  0;	}
	bool 	operator <= (const pkttype_profile_t & other) const throw() { return compare(other) <= 0;	}
	bool 	operator >  (const pkttype_profile_t & other) const throw() { return compare(other) >  0;	}
	bool 	operator >= (const pkttype_profile_t & other) const throw() { return compare(other) >= 0;	}

	
	/*************** display function	*******************************/
	std::string	to_string()						const throw();
	friend	std::ostream & operator << (std::ostream & os, const pkttype_profile_t &pkttype_profile ) throw()
					{ return os << pkttype_profile.to_string();	}

	/*************** Serialization function	*******************************/
	serial_t &	serialize(serial_t &serial, size_t val)			const throw();
	size_t		unserialize(serial_t &serial)				throw(serial_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PKTTYPE_PROFILE_HPP__  */



