/*! \file
    \brief Declaration of the neoip_slay_tls_full
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_slay_full_btjamrc4.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_bt_jamrc4_full.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(slay_full_vapi_t, slay_full_btjamrc4_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
slay_full_btjamrc4_t::slay_full_btjamrc4_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero from field
	m_btjamrc4_full	= NULL;
}

/** \brief Destructor
 */
slay_full_btjamrc4_t::~slay_full_btjamrc4_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete m_btjamrc4_full if needed
	nipmem_zdelete	m_btjamrc4_full;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         Setup Fcuntion
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/** \brief Start the operation
 */
slay_err_t	slay_full_btjamrc4_t::start(bt_jamrc4_full_t *p_btjamrc4_full)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->m_btjamrc4_full	= p_btjamrc4_full;

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
slay_err_t	slay_full_btjamrc4_t::notify_recved_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");

	// notify pkt_out to m_btjamrc4_full 
	bt_err	= m_btjamrc4_full->notify_recved_data(pkt_inp, pkt_out);
	if( bt_err.failed() )	return slay_err_from_bt(bt_err);

	// return no error
	return slay_err_t::OK;
}

/** \brief notify the data to xmit
 */
slay_err_t	slay_full_btjamrc4_t::notify_toxmit_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");
	
	// notify pkt_out to m_btjamrc4_full 
	bt_err	= m_btjamrc4_full->notify_toxmit_data(pkt_inp, pkt_out);
	if( bt_err.failed() )	return slay_err_from_bt(bt_err);

	// return no error
	return slay_err_t::OK;
}

/** \brief xmit a gracefull shutdown
 */
slay_err_t	slay_full_btjamrc4_t::notify_gracefull_shutdown(pkt_t &pkt_out)		throw()
{
	// no gracefull_shutdown for bt_jamrc4_full_t
	pkt_out	= pkt_t();
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
std::string	slay_full_btjamrc4_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "slay_full_btjamrc4_t";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END




