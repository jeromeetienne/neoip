/*! \file
    \brief Header of the \ref skey_ciph_t class
*/


#ifndef __NEOIP_SKEY_CIPH_HPP__ 
#define __NEOIP_SKEY_CIPH_HPP__ 

/* system include */
#include <iostream>
/* local include */
#include "neoip_crypto_gcrypt.hpp"
#include "neoip_skey_ciph_type.hpp"
#include "neoip_skey_ciph_iv.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_datum.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to ciphenticate with symmetric key
 */
class skey_ciph_t : NEOIP_COPY_CTOR_ALLOW {
private:
	skey_ciph_type_t	ciph_type;	//!< the type of this skey_ciph_t
	gcry_cipher_hd_t	gcry_cipher_hd;	//!< gcrypt context
	datum_t			key_datum;	//!< store the current key	

	// init/deinit the gcrypt context
	bool	init_gcry_hd()				throw();
	void	deinit_gcry_hd()			throw();


	// mode auxilary functions
	bool	require_padding()			const throw();
	bool	require_iv()				const throw();
	
	void	set_iv(const skey_ciph_iv_t & iv)	const throw();
	
	// convertion between local constant and gcrypt one
	int	get_gcry_cipher_mode()			const throw();
	int	get_gcry_cipher_algo()			const throw();

	// padding function
	void	padding_build(size_t src_len, void *dst_ptr )		const throw();
	ssize_t	padding_parse(const void *src_ptr, size_t src_len)	const throw();
	size_t	padding_cpu_len(size_t plaintxt_len)			const throw();

	// canonical object management stuff
private:
	void		zeroing()				throw();
	void		copy(const skey_ciph_t &other)		throw();
public:	
	skey_ciph_t()				throw()	{ zeroing();			}
	skey_ciph_t(const skey_ciph_t &other)	throw()	{ zeroing(); copy(other);	}
	~skey_ciph_t()			 	throw()	{ nullify();			}
	//! assignement operator
	skey_ciph_t &	operator = (const skey_ciph_t & other)	throw(){
		if( this == &other )	return *this;
		nullify();
		copy(other);
		return *this;
	}
	
	// canonical object management stuff
	int		compare(const skey_ciph_t & other)	const throw();
	std::string	to_string()				const throw();
	void		nullify()				throw();
	bool		is_null()				const throw();	
friend	std::ostream & operator << (std::ostream & os, const skey_ciph_t &skey_ciph )	throw()
	{ return os << skey_ciph.to_string();	}
	// comparison operator
	bool	operator ==(const skey_ciph_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const skey_ciph_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const skey_ciph_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const skey_ciph_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const skey_ciph_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const skey_ciph_t & other)  const throw(){ return compare(other) >= 0; }
	
	
	// constructor with init
	skey_ciph_t(const skey_ciph_type_t &ciph_type)		throw();
	
	// some query functions
	size_t	get_ciphertxt_len(size_t plaintxt_len)	  const throw();
	size_t	get_block_len()				  const throw();
	size_t	get_key_len()				  const	throw();
	
	// encryption/decryption functions
	bool	init_key(const void *key_ptr, size_t key_len)	throw();
	void	encipher(const skey_ciph_iv_t & iv, const void *src_ptr, size_t src_len
							, void *dst_ptr, size_t dst_len)	throw();
	ssize_t	decipher(const skey_ciph_iv_t & iv, const void *src_ptr, size_t src_len
							, void *dst_ptr, size_t dst_len)	throw();
	
	// encryption/descryption helper function for datum_t
	bool	init_key(const datum_t &key_datum)		throw()
		{ return init_key(key_datum.get_data(), key_datum.get_len());			}
	void	encipher(const skey_ciph_iv_t & iv, const datum_t &src, datum_t &dst) throw()
		{ encipher(iv, src.get_data(), src.get_len(), dst.get_data(), dst.get_len());	}
	void	decipher(const skey_ciph_iv_t & iv, const datum_t &src, datum_t &dst) throw()
		{ encipher(iv, src.get_data(), src.get_len(), dst.get_data(), dst.get_len());	}	

friend	class skey_ciph_iv_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SKEY_CIPH_HPP__  */



