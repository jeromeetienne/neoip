/*! \file
    \brief Header of the \ref dh_param.cpp

*/


#ifndef __NEOIP_X509_CERT_HPP__ 
#define __NEOIP_X509_CERT_HPP__ 

/* system include */
#include <string>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

/* local include */
#include "neoip_crypto_err.hpp"
#include "neoip_x509_privkey.hpp"
#include "neoip_x509_request.hpp"
#include "neoip_serial.hpp"
#include "neoip_datum.hpp"
#include "neoip_copy_ctor_checker.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to handle diffie hellman parameters
 */
class x509_cert_t : NEOIP_COPY_CTOR_ALLOW {
private:
	gnutls_x509_crt		gnutls_cert;

	/*************** Internal function	*******************************/
	crypto_err_t	generate_param( const x509_request_t &cert_req )	throw();
	void		zeroing()				throw();
	void		copy(const x509_cert_t &other)		throw();
public:	
	/*************** ctor/dtor	***************************************/
	x509_cert_t()				throw()	{ zeroing();			}
	~x509_cert_t()			 	throw()	{ nullify();			}
	x509_cert_t(const x509_cert_t &other)	throw()	{ zeroing(); copy(other);	}
	x509_cert_t &	operator = (const x509_cert_t & other)	throw(){
		if( this == &other )	return *this;
		nullify();
		copy(other);
		return *this;
	}

	/*************** Query function	***************************************/
	void		nullify()		throw();
	bool		is_null()		const throw();
	size_t		key_length()		const throw();
	std::string	algo_name()		const throw();
	std::string	subject_name()		const throw();
	std::string	issuer_name()		const throw();

	/*************** Convertion function	*******************************/
	static x509_cert_t	from_der_datum(const datum_t &der_datum)	throw();
	datum_t			to_der_datum()					const throw();

	/*************** Compatibility layer	*******************************/
	int		get_key_len()		const throw()	{ return key_length();	}
	std::string	get_algo_name()		const throw()	{ return algo_name();	}
	std::string	get_subject_name()	const throw()	{ return subject_name();}
	std::string	get_issuer_name()	const throw()	{ return issuer_name();	}

	/*************** Update function	*******************************/
	crypto_err_t	generate_from_request(const x509_request_t &request, const x509_cert_t &ca_cert
						, const x509_privkey_t &ca_privkey)	throw();
	crypto_err_t	generate_selfsigned(const x509_request_t &request
						, const x509_privkey_t &privkey)	throw();	
	crypto_err_t	verify_cert(const x509_cert_t &cert)				const throw();
	crypto_err_t	verify_data(const void *data_ptr, size_t data_len
				, const void *signature_ptr, size_t signature_len)	const throw();
	crypto_err_t	verify_data(const datum_t &data, const datum_t &sign)		const throw()
		{ return verify_data(data.void_ptr(), data.length(), sign.void_ptr(), sign.length());	}


	/*************** Comparison Operator	*******************************/
	int	compare(const x509_cert_t & other)	const throw();
	bool	operator ==(const x509_cert_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const x509_cert_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const x509_cert_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const x509_cert_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const x509_cert_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const x509_cert_t & other)  const throw(){ return compare(other) >= 0; }
	
	/*************** display function	*******************************/
	std::string	to_string()			const throw();
	friend	std::ostream & operator << (std::ostream & os, const x509_cert_t &skey_auth )	throw()
							{ return os << skey_auth.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const x509_cert_t &cert)	throw();
	friend	serial_t& operator >> (serial_t& serial, x509_cert_t &cert) 		throw(serial_except_t);

	/*************** List of friend class	*******************************/	
	friend	class tls_privctx_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_X509_CERT_HPP__  */



