/*! \file
    \brief Header of the \ref dh_privkey.cpp

*/


#ifndef __NEOIP_DH_PRIVKEY_HPP__ 
#define __NEOIP_DH_PRIVKEY_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_crypto_gcrypt.hpp"
#include "neoip_dh_pubkey.hpp"
#include "neoip_dh_param.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to handle diffie hellman private key
 */
class dh_privkey_t {
private:
	gcry_mpi_t		gcry_privkey;	//!< the private key in gcrypt format
	dh_pubkey_t		pubkey;		//!< the public key associated with the privkey

	//! return the public key in gcrypt format
	const gcry_mpi_t &	get_gcry_privkey() const throw()	{ return gcry_privkey; }
	
	// canonical object management stuff
private:
	void		zeroing()				throw();
	void		copy(const dh_privkey_t &other)		throw();
public:	
	dh_privkey_t()				throw()	{ zeroing();			}
	dh_privkey_t(const dh_privkey_t &other)	throw()	{ zeroing(); copy(other);	}
	~dh_privkey_t()			 	throw()	{ nullify();			}
	dh_privkey_t &	operator = (const dh_privkey_t & other)	throw(){
		if( this == &other )	return *this;
		nullify();
		copy(other);
		return *this;
	}
	friend	std::ostream & operator << (std::ostream & os, const dh_privkey_t &skey_auth )	throw()
		{ return os << skey_auth.to_string();	}
	// comparison operator
	bool	operator ==(const dh_privkey_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const dh_privkey_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const dh_privkey_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const dh_privkey_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const dh_privkey_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const dh_privkey_t & other)  const throw(){ return compare(other) >= 0; }
	
	// canonical object management stuff
	int		compare(const dh_privkey_t & other)	const throw();
	std::string	to_string()				const throw();
	void		nullify()				throw();
	bool		is_null()				const throw();
	
	dh_privkey_t(const dh_param_t &param)		throw();
	const dh_pubkey_t &get_pubkey() const throw()	{ return pubkey;	}	

friend	class dh_shsecret_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DH_PRIVKEY_HPP__  */



