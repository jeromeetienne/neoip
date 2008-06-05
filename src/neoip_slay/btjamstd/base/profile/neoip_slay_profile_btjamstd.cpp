/*! \file
    \brief definition of the \ref slay_profile_btjamstd_t
*/

/* system include */
/* local include */
#include "neoip_slay_profile_btjamstd.hpp"
#include "neoip_slay_profile.hpp"

NEOIP_NAMESPACE_BEGIN

// define and insert this product into the factory plant
FACTORY_PRODUCT_DEFINITION(slay_profile_vapi_t, slay_profile_btjamstd_t);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
slay_profile_btjamstd_t::slay_profile_btjamstd_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return no error if the slay_profile_btjamstd_t is valid, .failed() otherwise
 */
slay_err_t	slay_profile_btjamstd_t::check()		const throw()
{
	bt_err_t	bt_err;
	// check bt_jamstd_profile_t
	bt_err	= bt_jamstd().check();
	if( bt_err.failed() )	return slay_err_from_bt(bt_err);
	// check if shared_secret is set
	if( shared_secret().is_null() )	return slay_err_t(slay_err_t::ERROR, "shared secret MUST be set");
	// return no error
	return slay_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   access to domain specific api
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief to access slay_profile_btjamstd_t of this slay_profile_btjamstd_t
 */
const slay_profile_btjamstd_t &slay_profile_btjamstd_t::from_slay(const slay_profile_t &slay_profile)	throw()
{
	// sanity check - the slay_domain_t MUST be tls
	DBG_ASSERT( slay_profile.domain() == slay_domain_t::BTJAMSTD );
	// return the slay_profile_btjamstd_api_t
	return dynamic_cast <const slay_profile_btjamstd_t &>(*slay_profile.profile_vapi());
}

/** \brief to access slay_profile_btjamstd_t of this slay_profile_btjamstd_t
 */
slay_profile_btjamstd_t &	slay_profile_btjamstd_t::from_slay(slay_profile_t &slay_profile)	throw()
{
	// sanity check - the slay_domain_t MUST be tls
	DBG_ASSERT( slay_profile.domain() == slay_domain_t::BTJAMSTD );
	// return the slay_profile_btjamstd_api_t
	return dynamic_cast <slay_profile_btjamstd_t &>(*slay_profile.profile_vapi());
}


NEOIP_NAMESPACE_END




