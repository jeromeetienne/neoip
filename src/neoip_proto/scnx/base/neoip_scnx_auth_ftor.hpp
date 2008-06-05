/*! \file
    \brief Header of the scnx_auth_ftor_t callback

*/


#ifndef __NEOIP_SCNX_AUTH_FTOR_HPP__ 
#define __NEOIP_SCNX_AUTH_FTOR_HPP__ 
/* system include */
/* local include */
#include "neoip_scnx_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	x509_cert_t;

/** \brief the callback class for authenticating a x509_cert_t received by the 
 *         scnx layer
 */
class scnx_auth_ftor_cb_t {
public:
	/** \brief called to authenticate a x509_cert_t received by the scnx layer
	 * 
	 * @return scnx_err_t::OK if the certificate is allowed, any other value mean denied
	 */
	virtual scnx_err_t scnx_auth_ftor_cb(void *userptr, const x509_cert_t &x509_cert) throw() = 0;
	virtual ~scnx_auth_ftor_cb_t() {};
};


/** \brief Functor for authentication of certificate by the cnx layer
 */
class scnx_auth_ftor_t : NEOIP_COPY_CTOR_ALLOW {
private:
	scnx_auth_ftor_cb_t *	callback;	//!< the callback to notify when the functor is called
	void *			userptr;	//!< the userptr associated with the above callback
public:
	/** \brief Constructor of the functor
	 */
	scnx_auth_ftor_t(scnx_auth_ftor_cb_t *callback = NULL, void *userptr = NULL) throw()
	{
		this->callback	= callback;
		this->userptr	= userptr;
	}
	
	/** \brief Return true if the object is considered null, false otherwise
	 */
	bool	is_null()	const throw()	{ return callback == NULL;	}

	/** \brief overload the () operator (as it is a functor)
	 * 
	 * @return scnx_err_t::OK if the certificate is allowed, any other value mean denied
	 */
	scnx_err_t operator()(const x509_cert_t &x509_cert)	const throw()
	{
		// sanity check - the object MUST NOT be null
		DBG_ASSERT( !is_null() );
		// call the callback in the scnx_auth_ftor_t template
		return callback->scnx_auth_ftor_cb(userptr, x509_cert);
	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_AUTH_FTOR_HPP__  */



