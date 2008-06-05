/*! \file
    \brief Declaration of the neoip_slay_tls_dresp
*/


#ifndef __NEOIP_SLAY_RESP_TLS_HPP__
#define __NEOIP_SLAY_RESP_TLS_HPP__
/* system include */
/* local include */
#include "neoip_slay_helper_tls.hpp"
#include "neoip_slay_resp_vapi.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_slay_profile.hpp"
#include "neoip_tls_resp.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration

/** \brief class for the \ref slay_resp_t of the slay_domain_t::TLS
 */
class slay_resp_tls_t : public slay_resp_vapi_t {
private:
	slay_profile_t	m_slay_profile;	//!< the slay_profile_t for this responder
	tls_resp_t	m_tls_resp;
public:
	/*************** ctor/dtor	***************************************/
	slay_resp_tls_t()			throw();
	~slay_resp_tls_t()			throw();
	
	/*************** Setup function	***************************************/
	slay_err_t	start(const slay_resp_arg_t &resp_arg)		throw();
		
	/*************** query function	***************************************/
	const slay_domain_t &	domain()	const throw()	{ return slay_helper_tls_t::DOMAIN_VAR;}
	const slay_profile_t &	profile()	const throw()	{ return m_slay_profile;	}
	pkt_t &			xmit_buffer()	throw()		{ return m_tls_resp.xmit_buffer();	}

	/*************** Action function	*******************************/
	slay_err_t		notify_recved_data(const pkt_t &pkt_inp
						, slay_full_t **slay_full_out)	throw();
	
	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	
	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(slay_resp_vapi_t, slay_resp_tls_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_RESP_TLS_HPP__ */



