/*! \file
    \brief Definition of the \ref scnx_profile_t

*/


/* system include */
/* local include */
#include "neoip_scnx_profile.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
scnx_profile_t::scnx_profile_t()	throw()
{
	// setup the default diffie-hellman parameter
	dh_param		( dh_param_t::build_ike_mopd_grp1() );
	// setup the skey_auth_t option list
	skey_auth_optlist	( skey_auth_type_arr_t().append(skey_auth_type_t("md5/hmac/12"))
							.append(skey_auth_type_t("sha1/hmac/16")) );
	// setup the skey_ciph_t option list
	skey_ciph_optlist	( skey_ciph_type_arr_t().append(skey_ciph_type_t("aes/ctr"))
							.append(skey_ciph_type_t("des/cbc")));
}

/** \brief Destructor
 */
scnx_profile_t::~scnx_profile_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
scnx_err_t	scnx_profile_t::check()	const throw()
{
	// check the ident_cert is set
	if( ident_cert().is_null() )
		return scnx_err_t(scnx_err_t::ERROR, "scnx_profile_t MUST have an ident_cert");
	// check the ident_privkey is set
	if( ident_privkey().is_null() )
		return scnx_err_t(scnx_err_t::ERROR, "scnx_profile_t MUST have an ident_privkey");
	// check the scnx_auth_fto is set
	if( scnx_auth_ftor().is_null() )
		return scnx_err_t(scnx_err_t::ERROR, "scnx_profile_t MUST have an scnx_auth_ftor");
	// return no error
	return scnx_err_t::OK;
}

NEOIP_NAMESPACE_END

