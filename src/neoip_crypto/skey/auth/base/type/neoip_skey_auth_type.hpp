/*! \file
    \brief Header of the \ref neoip_crypto_skey_auth_type_t.cpp

*/


#ifndef __NEOIP_SKEY_AUTH_TYPE_HPP__ 
#define __NEOIP_SKEY_AUTH_TYPE_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_skey_auth_algo.hpp"
#include "neoip_skey_auth_mode.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_serial.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief type of the skey_auth_type_t
 */
class skey_auth_type_t : NEOIP_COPY_CTOR_ALLOW {
private:
	skey_auth_algo_t	algo;
	skey_auth_mode_t	mode;
	size_t			output_len;
public:
	// ctor/dtor
	skey_auth_type_t()				throw() : output_len(0)	{}
	skey_auth_type_t(skey_auth_algo_t algo_type, skey_auth_mode_t mode_type, int output_len)
										throw();
	skey_auth_type_t(const char *type_str)		throw();
	~skey_auth_type_t()				throw();

	// some canonical objects functions
	int	compare( const skey_auth_type_t & other )  			const throw();
	//! return true if the object is null
	bool	is_null()	const throw()	{ return algo.is_null();	}
	//! make the object null
	void	nullify()	throw()		{ *this = skey_auth_type_t();	}
	// convert the object to a string
	std::string	to_string()		const throw();
	
	// comparison operator
	bool	operator ==(const skey_auth_type_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const skey_auth_type_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const skey_auth_type_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const skey_auth_type_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const skey_auth_type_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const skey_auth_type_t & other)  const throw(){ return compare(other) >= 0; }

	// query functions
	const skey_auth_algo_t &	get_algo()	const throw() { return algo;		}
	const skey_auth_mode_t &	get_mode()	const throw() { return mode;		}
	size_t				get_output_len()const throw() { return output_len;	}

	// display and serialization function
friend	std::ostream& operator << (std::ostream & os, const skey_auth_type_t &auth_type )	throw()
	{ return os << auth_type.to_string();	}
friend	serial_t& operator << (serial_t& serial, const skey_auth_type_t &auth_type)		throw();
friend	serial_t& operator >> (serial_t& serial, skey_auth_type_t &auth_type) 	throw(serial_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SKEY_AUTH_TYPE_HPP__  */



