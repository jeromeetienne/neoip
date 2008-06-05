/*! \file
    \brief Definition of the casti_swarm_arg_t

*/

/* system include */
/* local include */
#include "neoip_casti_swarm_arg.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ARGPACK_DEF_CLASS_BEGIN(casti_swarm_arg_t)
NEOIP_ARGPACK_DEF_ITEM(casti_swarm_arg_t	, casti_apps_t *	, casti_apps)
NEOIP_ARGPACK_DEF_ITEM(casti_swarm_arg_t	, http_uri_t		, mdata_srv_uri)
NEOIP_ARGPACK_DEF_ITEM(casti_swarm_arg_t	, std::string		, cast_name)
NEOIP_ARGPACK_DEF_ITEM(casti_swarm_arg_t	, std::string		, cast_privtext)
NEOIP_ARGPACK_DEF_ITEM(casti_swarm_arg_t	, http_uri_t		, httpi_uri)
NEOIP_ARGPACK_DEF_ITEM(casti_swarm_arg_t	, bt_httpi_mod_type_t	, httpi_mod)
NEOIP_ARGPACK_DEF_ITEM(casti_swarm_arg_t	, httpi_uri_t		, http_peersrc_uri)
NEOIP_ARGPACK_DEF_CLASS_END(casti_swarm_arg_t)

/** \brief Check if the object is valid
 */ 
bt_err_t	casti_swarm_arg_t::check()	const throw()
{
	// test the mandatory fields
	if( !casti_apps_present() || !casti_apps() )
		return bt_err_t(bt_err_t::ERROR, "Invalid casti_apps pointer");
	if( !mdata_srv_uri_present() || mdata_srv_uri().is_null() )
		return bt_err_t(bt_err_t::ERROR, "Invalid mdata_srv_uri");
	if( !cast_name_present() || cast_name().empty() )
		return bt_err_t(bt_err_t::ERROR, "Invalid cast_name");
	if( !cast_privtext_present() || cast_privtext().empty() )
		return bt_err_t(bt_err_t::ERROR, "Invalid cast_privtext");
	if( !httpi_uri_present() || httpi_uri().is_null() )
		return bt_err_t(bt_err_t::ERROR, "Invalid httpi_uri");
	if( !httpi_mod_present() || httpi_mod().is_null() )
		return bt_err_t(bt_err_t::ERROR, "Invalid httpi_mod");
	// test the optional fields	
	if( http_peersrc_uri_present() && http_peersrc_uri().is_null() )
		return bt_err_t(bt_err_t::ERROR, "Invalid http_peersrc_uri");
	// return no error;
	return bt_err_t::OK;
}


NEOIP_NAMESPACE_END

