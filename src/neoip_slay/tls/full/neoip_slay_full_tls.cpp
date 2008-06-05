/*! \file
    \brief Declaration of the neoip_slay_tls_full
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_slay_full_tls.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_tls.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(slay_full_vapi_t, slay_full_tls_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
slay_full_tls_t::slay_full_tls_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero from field
	m_tls_full	= NULL;
}

/** \brief Destructor
 */
slay_full_tls_t::~slay_full_tls_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete m_tls_full if needed
	nipmem_zdelete	m_tls_full;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         Setup Fcuntion
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/** \brief Start the operation
 */
slay_err_t	slay_full_tls_t::start(tls_full_t *p_tls_full)			throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->m_tls_full	= p_tls_full;

	// return no error
	return slay_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the received data
 */
slay_err_t	slay_full_tls_t::notify_recved_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw()
{
	crypto_err_t	crypto_err;
	// log to debug
	KLOG_DBG("enter");
	// notify pkt_inp to m_tls_full
	crypto_err	= m_tls_full->notify_recved_data(pkt_inp);
	if( crypto_err.failed() )	return slay_err_from_crypto(crypto_err);

	// copy m_tls_full->recv_buffer() to pkt_out 
	pkt_out		= m_tls_full->recv_buffer();
	// flush m_tls_full->recv_buffer();
	m_tls_full->recv_buffer().head_free( m_tls_full->recv_buffer().length() );

	// return no error
	return slay_err_t::OK;
}

/** \brief notify the data to xmit
 */
slay_err_t	slay_full_tls_t::notify_toxmit_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw()
{
	crypto_err_t	crypto_err;
	// log to debug
	KLOG_DBG("enter");
	// notify pkt_inp to m_tls_full
	crypto_err	= m_tls_full->notify_toxmit_data(pkt_inp);
	if( crypto_err.failed() )	return slay_err_from_crypto(crypto_err);

	// copy m_tls_full->xmit_buffer() to pkt_out 
	pkt_out		= m_tls_full->xmit_buffer();
	// flush m_tls_full->xmit_buffer();
	m_tls_full->xmit_buffer().head_free( m_tls_full->xmit_buffer().length() );
	// return no error
	return slay_err_t::OK;
}

/** \brief xmit a gracefull shutdown
 */
slay_err_t	slay_full_tls_t::notify_gracefull_shutdown(pkt_t &pkt_out)		throw()
{
	crypto_err_t	crypto_err;
	// pass the command to m_tls_full	
	crypto_err	= m_tls_full->notify_gracefull_shutdown();
	if( crypto_err.failed() )	return slay_err_from_crypto(crypto_err);
	// copy m_tls_full->recv_buffer() to pkt_out 
	pkt_out		= m_tls_full->recv_buffer();
	// flush m_tls_full->recv_buffer();
	m_tls_full->recv_buffer().head_free( m_tls_full->recv_buffer().length() );
	// return no error
	return slay_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                           display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	slay_full_tls_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "slay_full_tls_t";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END




