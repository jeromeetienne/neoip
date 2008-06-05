/*! \file
    \brief Declaration of the neoip_slay_tls_full
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_slay_full_btjamstd.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_bt_jamstd_full.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(slay_full_vapi_t, slay_full_btjamstd_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
slay_full_btjamstd_t::slay_full_btjamstd_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero from field
	m_btjamstd_full	= NULL;
}

/** \brief Destructor
 */
slay_full_btjamstd_t::~slay_full_btjamstd_t()					throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete m_btjamstd_full if needed
	nipmem_zdelete	m_btjamstd_full;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         Setup Fcuntion
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/** \brief Start the operation
 */
slay_err_t	slay_full_btjamstd_t::start(bt_jamstd_full_t *p_btjamstd_full)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->m_btjamstd_full	= p_btjamstd_full;

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
slay_err_t	slay_full_btjamstd_t::notify_recved_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");

	// notify pkt_out to m_btjamstd_full 
	bt_err	= m_btjamstd_full->notify_recved_data(pkt_inp, pkt_out);
	if( bt_err.failed() )	return slay_err_from_bt(bt_err);

	// return no error
	return slay_err_t::OK;
}

/** \brief notify the data to xmit
 */
slay_err_t	slay_full_btjamstd_t::notify_toxmit_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");
	
	// notify pkt_out to m_btjamstd_full 
	bt_err	= m_btjamstd_full->notify_toxmit_data(pkt_inp, pkt_out);
	if( bt_err.failed() )	return slay_err_from_bt(bt_err);

	// return no error
	return slay_err_t::OK;
}

/** \brief xmit a gracefull shutdown
 */
slay_err_t	slay_full_btjamstd_t::notify_gracefull_shutdown(pkt_t &pkt_out)		throw()
{
	// no gracefull_shutdown for bt_jamstd_full_t
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
std::string	slay_full_btjamstd_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "slay_full_btjamstd_t";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END




