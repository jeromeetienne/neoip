/*! \file
    \brief Declaration of the neoip_slay_tls_dfull
*/


#ifndef __NEOIP_SLAY_FULL_TLS_HPP__
#define __NEOIP_SLAY_FULL_TLS_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_slay_helper_tls.hpp"
#include "neoip_slay_full_vapi.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	tls_full_t;

/** \brief class for the \ref slay_full_t of the slay_domain_t::TLS
 */
class slay_full_tls_t : NEOIP_COPY_CTOR_DENY, public slay_full_vapi_t {
private:
	tls_full_t *	m_tls_full;		//!< tls context for full
public:
	/*************** ctor/dtor	***************************************/
	slay_full_tls_t()				throw();
	~slay_full_tls_t()				throw();

	/*************** Setup Function	***************************************/
	slay_err_t	start(tls_full_t *p_tls_full)	throw();

	/*************** Query function	***************************************/
	const slay_domain_t &	domain()		const throw()	{ return slay_helper_tls_t::DOMAIN_VAR;}

	/*************** Action function	*******************************/
	slay_err_t	notify_recved_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw(); 
	slay_err_t	notify_toxmit_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw();
	slay_err_t	notify_gracefull_shutdown(pkt_t &pkt_out)		throw();
		
	/*************** Display function	*******************************/
	std::string		to_string()		const throw();	

	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(slay_full_vapi_t, slay_full_tls_t);
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_FULL_TLS_HPP__ */



