/*! \file
    \brief Header of the \ref dh_param.cpp

*/


#ifndef __NEOIP_X509_PRIVKEY_HPP__ 
#define __NEOIP_X509_PRIVKEY_HPP__ 

/* system include */
#include <string>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

#ifdef _WIN32
#	undef ERROR
#	undef INFINITE
#	undef OPTIONAL
#endif

/* local include */
#include "neoip_crypto_err.hpp"
#include "neoip_serial.hpp"
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to handle diffie hellman parameters
 */
class x509_privkey_t {
private:
	gnutls_x509_privkey	gnutls_privkey;

	// canonical object management stuff
private:
	void		zeroing()				throw();
	void		copy(const x509_privkey_t &other)	throw();
public:
	/*************** ctor/dtor	***************************************/
	x509_privkey_t()				throw()	{ zeroing();			}
	x509_privkey_t(const x509_privkey_t &other)	throw()	{ zeroing(); copy(other);	}
	~x509_privkey_t()			 	throw()	{ nullify();			}
	x509_privkey_t & operator = (const x509_privkey_t & other)	throw(){
		if( this == &other )	return *this;
		nullify();
		copy(other);
		return *this;
	}

	// canonical object management stuff
	void		nullify()		throw();
	bool		is_null()		const throw();
	std::string	algo_name()		const throw();

	// utility function
	crypto_err_t	generate(size_t len_bit)			throw();
	datum_t		sign_data(const void *data_ptr, size_t data_len)const throw();
	datum_t		sign_data(const datum_t &datum)			const throw()	{ return sign_data(datum.char_ptr(), datum.length());	}

	/*************** Convertion function	*******************************/
	static x509_privkey_t	from_der_datum(const datum_t &der_datum)	throw();
	datum_t			to_der_datum()					const throw();
	
	/*************** Comparison Operator	*******************************/
	int	compare(const x509_privkey_t & other)		const throw();
	bool	operator ==(const x509_privkey_t & other)	const throw()	{ return compare(other) == 0; }
	bool	operator !=(const x509_privkey_t & other)	const throw()	{ return compare(other) != 0; }
	bool	operator < (const x509_privkey_t & other)	const throw()	{ return compare(other) <  0; }
	bool	operator <=(const x509_privkey_t & other)	const throw()	{ return compare(other) <= 0; }
	bool	operator > (const x509_privkey_t & other)	const throw()	{ return compare(other) >  0; }
	bool	operator >=(const x509_privkey_t & other)	const throw()	{ return compare(other) >= 0; }
	
	/*************** Display function	*******************************/
	std::string	to_string()			const throw();
	friend	std::ostream & operator << (std::ostream & os, const x509_privkey_t &skey_auth )	throw()
							{ return os << skey_auth.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const x509_privkey_t &privkey)	throw();
	friend	serial_t& operator >> (serial_t& serial, x509_privkey_t &privkey)	throw(serial_except_t);

	/*************** List of friend class	*******************************/	
	friend	class x509_request_t;
	friend	class x509_cert_t;
	friend	class tls_privctx_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_X509_PRIVKEY_HPP__  */



