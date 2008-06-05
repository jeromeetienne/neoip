/*! \file
    \brief Header of the \ref dh_param.cpp

*/


#ifndef __NEOIP_X509_REQUEST_HPP__ 
#define __NEOIP_X509_REQUEST_HPP__ 

/* system include */
#include <string>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>

/* local include */
#include "neoip_crypto_err.hpp"
#include "neoip_x509_privkey.hpp"
#include "neoip_serial.hpp"
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to handle diffie hellman parameters
 */
class x509_request_t {
private:
	gnutls_x509_crq		gnutls_request;

	// canonical object management stuff
private:
	void		zeroing()				throw();
	void		copy(const x509_request_t &other)	throw();
public:	
	/*************** ctor/dtor	***************************************/
	x509_request_t()				throw()	{ zeroing();			}
	~x509_request_t()			 	throw()	{ nullify();			}
	x509_request_t(const x509_request_t &other)	throw()	{ zeroing(); copy(other);	}
	x509_request_t &operator = (const x509_request_t & other)	throw(){
		if( this == &other )	return *this;
		nullify();
		copy(other);
		return *this;
	}

	/*************** Query function	***************************************/
	bool		is_null()				const throw();
	std::string	common_name()				const throw();

	/*************** Compatibility layer	*******************************/
	std::string	get_name()				const throw()	{ return common_name();	}
	
	/*************** Update function	*******************************/
	void		nullify()				throw();
	crypto_err_t	generate(const std::string name,const x509_privkey_t &privkey) throw();

	/*************** Convertion function	*******************************/
	static x509_request_t	from_der_datum(const datum_t &der_datum)	throw();
	datum_t			to_der_datum()					const throw();
	
	/*************** Comparison Operator	*******************************/
	int	compare(const x509_request_t & other)		const throw();
	bool	operator ==(const x509_request_t & other)	const throw()	{ return compare(other) == 0; }
	bool	operator !=(const x509_request_t & other)	const throw()	{ return compare(other) != 0; }
	bool	operator < (const x509_request_t & other)	const throw()	{ return compare(other) <  0; }
	bool	operator <=(const x509_request_t & other)	const throw()	{ return compare(other) <= 0; }
	bool	operator > (const x509_request_t & other)	const throw()	{ return compare(other) >  0; }
	bool	operator >=(const x509_request_t & other)	const throw()	{ return compare(other) >= 0; }
	
	/*************** Display function	*******************************/
	std::string	to_string()				const throw();
	friend	std::ostream & operator << (std::ostream & os, const x509_request_t &skey_auth )	throw()
		{ return os << skey_auth.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const x509_request_t &request)	throw();
	friend	serial_t& operator >> (serial_t& serial, x509_request_t &request) 	throw(serial_except_t);

	/*************** List of friend class	*******************************/
	friend	class x509_cert_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_X509_REQUEST_HPP__  */



