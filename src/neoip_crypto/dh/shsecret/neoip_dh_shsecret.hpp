/*! \file
    \brief Header of the \ref dh_shsecret.cpp

*/


#ifndef __NEOIP_DH_SHSECRET_HPP__ 
#define __NEOIP_DH_SHSECRET_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_crypto_gcrypt.hpp"
#include "neoip_dh_param.hpp"
#include "neoip_dh_privkey.hpp"
#include "neoip_dh_pubkey.hpp"
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to handle diffie hellman public key
 */
class dh_shsecret_t {
private:
	gcry_mpi_t		gcry_shsecret;	//!< the public key in gcrypt format	

	// canonical object management stuff
private:
	void		zeroing()				throw();
	void		copy(const dh_shsecret_t &other)		throw();
public:	
	dh_shsecret_t()				throw()	{ zeroing();			}
	dh_shsecret_t(const dh_shsecret_t &other)	throw()	{ zeroing(); copy(other);	}
	~dh_shsecret_t()			 	throw()	{ nullify();			}
	dh_shsecret_t &	operator = (const dh_shsecret_t & other)	throw(){
		if( this == &other )	return *this;
		nullify();
		copy(other);
		return *this;
	}
	friend	std::ostream & operator << (std::ostream & os, const dh_shsecret_t &skey_auth )	throw()
		{ return os << skey_auth.to_string();	}
	// comparison operator
	bool	operator ==(const dh_shsecret_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const dh_shsecret_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const dh_shsecret_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const dh_shsecret_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const dh_shsecret_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const dh_shsecret_t & other)  const throw(){ return compare(other) >= 0; }
	
	// canonical object management stuff
	int		compare(const dh_shsecret_t & other)	const throw();
	std::string	to_string()				const throw();
	void		nullify()				throw();
	bool		is_null()				const throw();
	datum_t		to_datum()				const throw();

	dh_shsecret_t(const dh_privkey_t &privkey, const dh_pubkey_t &remote_pubkey
							, const dh_param_t &param)	throw();
	datum_t		get_shsecret_datum()	const throw()	{ return to_datum();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DH_SHSECRET_HPP__  */



