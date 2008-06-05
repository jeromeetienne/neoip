/*! \file
    \brief Declaration of the slay_itor_btjamstd_t
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_slay_itor_btjamstd.hpp"
#include "neoip_slay_profile_btjamstd.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN 

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(slay_itor_vapi_t, slay_itor_btjamstd_t);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
slay_itor_btjamstd_t::slay_itor_btjamstd_t()						throw()
{
	// init some fields
	m_slay_profile	= slay_profile_t(slay_domain_t::BTJAMSTD);
}

/** \brief Destructor
 */
slay_itor_btjamstd_t::~slay_itor_btjamstd_t()						throw()
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
slay_err_t	slay_itor_btjamstd_t::start(const slay_itor_arg_t &itor_arg)		throw()
{
	// sanity check - slay_itor_arg_t MUST be valid
	DBG_ASSERT( itor_arg.is_valid() );
	// sanity check - itor_arg.domain() MUST be the local one
	DBG_ASSERT( itor_arg.domain() == domain() );
	
	// if itor_arg.profile_present(), copy it
	if( itor_arg.profile_present() )	m_slay_profile	= itor_arg.profile();

	// set the tls_profile_t to m_btjamstd_itor
	const slay_profile_btjamstd_t & dom_profile	= slay_profile_btjamstd_t::from_slay(itor_arg.profile());
	m_btjamstd_itor.profile( dom_profile.bt_jamstd() );
	
	// start the bt_jamstd_itor_t
	bt_err_t	bt_err;
	bt_err		= m_btjamstd_itor.start(dom_profile.shared_secret());
	if( bt_err.failed() )	return slay_err_from_bt(bt_err);
	
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
 * @param slay_full_out	if returned as non-null, it contains a bt_jamstd_full_t
 *			and the connection is considered established
 * 
 * @return a slay_err_t. if slay_err.failed() then the connection should be dropped
 */
slay_err_t	slay_itor_btjamstd_t::notify_recved_data(const pkt_t &pkt_inp
						, slay_full_t **slay_full_out)	throw()
{
	bt_err_t	bt_err;
	bt_jamstd_full_t *	btjamstd_full;
	// log to debug
	KLOG_DBG("enter");
	// set btjamstd_full_out to NULL by default
	*slay_full_out	= NULL;

	// notify pkt_inp to m_btjamstd_itor
	bt_err	= m_btjamstd_itor.notify_recved_data(pkt_inp, &btjamstd_full);
	if( bt_err.failed() )	return slay_err_from_bt(bt_err);
	
	// if bt_jamstd_full_t is replied by m_btjamstd_itor, build the slay_full_t for it
	if( btjamstd_full ){
		*slay_full_out	= slay_helper_btjamstd_t::build_slay_full(btjamstd_full);
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
std::string	slay_itor_btjamstd_t::to_string()					const throw()
{
	std::ostringstream	oss;
	// build the string
	oss << "slay_itor_btjamstd_t";
	// return the just built string
	return oss.str();	
}

NEOIP_NAMESPACE_END




