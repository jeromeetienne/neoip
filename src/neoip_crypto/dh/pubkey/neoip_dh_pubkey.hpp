/*! \file
    \brief Header of the \ref dh_pubkey.cpp

*/


#ifndef __NEOIP_DH_PUBKEY_HPP__ 
#define __NEOIP_DH_PUBKEY_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_crypto_gcrypt.hpp"
#include "neoip_namespace.hpp"
#include "neoip_serial.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to handle diffie hellman public key
 */
class dh_pubkey_t {
private:
	gcry_mpi_t		gcry_pubkey;	//!< the public key in gcrypt format

	//! return the public key in gcrypt format
	const gcry_mpi_t &	get_gcry_pubkey() const throw()	{ return gcry_pubkey; }
	

	// canonical object management stuff
private:
	void		zeroing()				throw();
	void		copy(const dh_pubkey_t &other)		throw();
public:	
	dh_pubkey_t()				throw()	{ zeroing();			}
	dh_pubkey_t(const dh_pubkey_t &other)	throw()	{ zeroing(); copy(other);	}
	~dh_pubkey_t()			 	throw()	{ nullify();			}
	dh_pubkey_t &	operator = (const dh_pubkey_t & other)	throw(){
		if( this == &other )	return *this;
		nullify();
		copy(other);
		return *this;
	}
	friend	std::ostream & operator << (std::ostream & os, const dh_pubkey_t &skey_auth )	throw()
		{ return os << skey_auth.to_string();	}
	// comparison operator
	bool	operator ==(const dh_pubkey_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const dh_pubkey_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const dh_pubkey_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const dh_pubkey_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const dh_pubkey_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const dh_pubkey_t & other)  const throw(){ return compare(other) >= 0; }
	
	// canonical object management stuff
	int		compare(const dh_pubkey_t & other)	const throw();
	std::string	to_string()				const throw();
	void		nullify()				throw();
	bool		is_null()				const throw();
	datum_t		to_datum()				const throw();
	static dh_pubkey_t	from_datum(const datum_t &datum)throw();

friend	serial_t& operator << ( serial_t& serial, const dh_pubkey_t &dh_pubkey )	throw();
friend	serial_t& operator >> ( serial_t& serial, dh_pubkey_t &dh_pubkey ) 	throw(serial_except_t);
	
friend	class dh_privkey_t;
friend	class dh_shsecret_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DH_PUBKEY_HPP__  */



