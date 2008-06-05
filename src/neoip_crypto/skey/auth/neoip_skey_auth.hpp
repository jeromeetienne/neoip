/*! \file
    \brief Header of the \ref skey_auth_t class
*/


#ifndef __NEOIP_SKEY_AUTH_HPP__ 
#define __NEOIP_SKEY_AUTH_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_skey_auth_type.hpp"
#include "neoip_crypto_gcrypt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_datum.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to authenticate with symmetric key
 */
class skey_auth_t : NEOIP_COPY_CTOR_ALLOW {
private:
	skey_auth_type_t	auth_type;	// the type of this skey_auth_t
	gcry_md_hd_t		gcry_md_hd;	// gcrypt context

	bool		init_gcry_hd(const skey_auth_type_t &auth_type)	throw();
	void		deinit_gcry_hd()					throw();
	// canonical object management stuff
private:
	void		zeroing()				throw();
	void		copy(const skey_auth_t &other)		throw();
public:	
	skey_auth_t()				throw()	{ zeroing();			}
	skey_auth_t(const skey_auth_t &other)	throw()	{ zeroing(); copy(other);	}
	~skey_auth_t()			 	throw()	{ nullify();			}
	//! assignement operator
	skey_auth_t &	operator = (const skey_auth_t & other)	throw(){
		if( this == &other )	return *this;
		nullify();
		copy(other);
		return *this;
	}
	
	// canonical object management stuff
	int		compare(const skey_auth_t & other)	const throw();
	std::string	to_string()				const throw();
	void		nullify()				throw();
	bool		is_null()				const throw();
friend	std::ostream & operator << (std::ostream & os, const skey_auth_t &skey_auth )	throw()
	{ return os << skey_auth.to_string();	}
	// comparison operator
	bool	operator ==(const skey_auth_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const skey_auth_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const skey_auth_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const skey_auth_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const skey_auth_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const skey_auth_t & other)  const throw(){ return compare(other) >= 0; }

	// computation function
	bool		init_key(const void *key_ptr, size_t key_len)	throw();
	void		update(const void *buf_ptr, size_t buf_len)	throw();
	const void *	final()						throw();
	datum_t		get_output()					throw()
			{ return datum_t( final(), get_output_len(), datum_t::SECMEM );	}

	// computation helper function with datum_t
	bool		init_key(const datum_t &datum_key) 		throw()
			{ return init_key(datum_key.get_data(), datum_key.get_len());		}
	void		update(const datum_t &datum_buf)		throw()
			{ update(datum_buf.get_data(), datum_buf.get_len());		}

	// query function
	size_t		get_output_len()				const throw();
	size_t		get_key_len()					const throw();
	
	// constructor with init
	skey_auth_t(const skey_auth_type_t &auth_type)		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SKEY_AUTH_HPP__  */



