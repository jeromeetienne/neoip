/*! \file
    \brief Declaration of the slay_itor_tls_t
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_slay_itor_tls.hpp"
#include "neoip_slay_profile_tls.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(slay_itor_vapi_t, slay_itor_tls_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
slay_itor_tls_t::slay_itor_tls_t()						throw()
{
	// init some fields
	m_slay_profile	= slay_profile_t(slay_domain_t::TLS);
}

/** \brief Destructor
 */
slay_itor_tls_t::~slay_itor_tls_t()						throw()
{
	// log to debug
	KLOG_DBG("enter");
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
slay_err_t	slay_itor_tls_t::start(const slay_itor_arg_t &itor_arg)		throw()
{
	// sanity check - slay_itor_arg_t MUST be valid
	DBG_ASSERT( itor_arg.is_valid() );
	// sanity check - itor_arg.domain() MUST be the local one
	DBG_ASSERT( itor_arg.domain() == domain() );
	
	// if itor_arg.profile_present(), copy it
	if( itor_arg.profile_present() )	m_slay_profile	= itor_arg.profile();

	// set the tls_profile_t to m_tls_itor
	const slay_profile_tls_t & dom_profile	= slay_profile_tls_t::from_slay(itor_arg.profile());
	m_tls_itor.profile( dom_profile.tls() );
	
	// start the tls_itor_t
	crypto_err_t	crypto_err;
	crypto_err	= m_tls_itor.start();
	if( crypto_err.failed() )	return slay_err_from_crypto(crypto_err);
	
	// return no error	
	return slay_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief used by the caller to notify pkt_inpt
 *
 * @param slay_full_out	if returned as non-null, it contains a tls_full_t
 *			and the connection is considered established
 * 
 * @return a slay_err_t. if slay_err.failed() then the connection should be dropped
 */
slay_err_t	slay_itor_tls_t::notify_recved_data(const pkt_t &pkt_inp
						, slay_full_t **slay_full_out)	throw()
{
	crypto_err_t	crypto_err;
	tls_full_t *	tls_full;
	// log to debug
	KLOG_DBG("enter");
	// set tls_full_out to NULL by default
	*slay_full_out	= NULL;

	// notify pkt_inp to m_tls_itor
	crypto_err	= m_tls_itor.notify_recved_data(pkt_inp, &tls_full);
	if( crypto_err.failed() )	return slay_err_from_crypto(crypto_err);
	
	// if tls_full_t is replied by m_tls_itor, build the slay_full_t for it
	if( tls_full ){
		*slay_full_out	= slay_helper_tls_t::build_slay_full(tls_full);
		if( *slay_full_out == NULL )	return slay_err_t(slay_err_t::ERROR, "unabled to build slay_full");
	}
	
	// return no error	
	return slay_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       Display Function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	slay_itor_tls_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "slay_itor_tls_t";
	// return the just built string
	return oss.str();	
}

NEOIP_NAMESPACE_END




