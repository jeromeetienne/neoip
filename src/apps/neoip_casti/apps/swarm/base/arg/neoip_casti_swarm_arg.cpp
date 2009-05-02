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
NEOIP_ARGPACK_DEF_ITEM(casti_swarm_arg_t	, http_uri_t		, scasti_uri)
NEOIP_ARGPACK_DEF_ITEM(casti_swarm_arg_t	, bt_scasti_mod_type_t	, scasti_mod)
NEOIP_ARGPACK_DEF_ITEM(casti_swarm_arg_t	, http_uri_t		, http_peersrc_uri)
NEOIP_ARGPACK_DEF_ITEM(casti_swarm_arg_t	, std::string		, web2srv_str)
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
	if( !scasti_uri_present() || scasti_uri().is_null() )
		return bt_err_t(bt_err_t::ERROR, "Invalid scasti_uri");
	if( !scasti_mod_present() || scasti_mod().is_null() )
		return bt_err_t(bt_err_t::ERROR, "Invalid scasti_mod");

	// test the optional fields
	if( http_peersrc_uri_present() && http_peersrc_uri().is_null() )
		return bt_err_t(bt_err_t::ERROR, "Invalid http_peersrc_uri");

	// IIF scasti_uri().scheme() == http_scheme_t::RTMP, check the uri format as 'security' mechanism
	// - as bt_scasti_rtmp_t is a server, the uri must have some authentication
	// - this authentication is handled by the presence of the cast_privtext in path
	// - TODO this is rather bad stuff as the password is send in clear over the network
	if( scasti_uri().scheme() == http_scheme_t::RTMP ){
		// rebuilt a canonical scasti_uri
		// - the format is scasti_uri = rtmp://anyhost/$mdata_src_uri/$cast_privtext/$cast_name
		std::string	rebuilt_str;
		rebuilt_str	 = "rtmp://0.0.0.0/";
		rebuilt_str	+= string_t::escape_not_in(mdata_srv_uri().to_string(), http_uri_t::UNRESERVED_CHARSET);
		rebuilt_str	+= "/";
		rebuilt_str	+= string_t::escape_not_in(cast_privtext()	, http_uri_t::UNRESERVED_CHARSET);
		rebuilt_str	+= "/";
		rebuilt_str	+= string_t::escape_not_in(cast_name()		, http_uri_t::UNRESERVED_CHARSET);
		http_uri_t	rebuilt_uri(rebuilt_str);
		// scasti_uri is valid IIF their the pathquery part of the uri is equal
		// - the hostport part is ignored as it depends how the client reaches neoip-casti
		if( rebuilt_uri.pathquery_str() != scasti_uri().pathquery_str() )
			return bt_err_t(bt_err_t::ERROR, "Non canonical scasti_uri");
	}
	// return no error;
	return bt_err_t::OK;
}


NEOIP_NAMESPACE_END

