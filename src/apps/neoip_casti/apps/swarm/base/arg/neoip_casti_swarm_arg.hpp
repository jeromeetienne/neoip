/*! \file
    \brief Header of the casti_swarm_arg_t

*/


#ifndef __NEOIP_CASTI_SWARM_ARG_HPP__ 
#define __NEOIP_CASTI_SWARM_ARG_HPP__ 
/* system include */
/* local include */
#include "neoip_argpack.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_http_uri.hpp"
#include "neoip_bt_scasti_mod_type.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casti_apps_t;
class	bt_err_t;

NEOIP_ARGPACK_DECL_CLASS_BEGIN(casti_swarm_arg_t)
NEOIP_ARGPACK_DECL_IS_VALID_STUB(casti_swarm_arg_t)
NEOIP_ARGPACK_DECL_NORMALIZE_STUB(casti_swarm_arg_t)
NEOIP_ARGPACK_DECL_ITEM_PTR(casti_swarm_arg_t	, casti_apps_t *	, casti_apps)
NEOIP_ARGPACK_DECL_ITEM(casti_swarm_arg_t	, http_uri_t		, mdata_srv_uri)
NEOIP_ARGPACK_DECL_ITEM(casti_swarm_arg_t	, std::string		, cast_name)
NEOIP_ARGPACK_DECL_ITEM(casti_swarm_arg_t	, std::string		, cast_privtext)
NEOIP_ARGPACK_DECL_ITEM(casti_swarm_arg_t	, http_uri_t		, httpi_uri)
NEOIP_ARGPACK_DECL_ITEM(casti_swarm_arg_t	, bt_scasti_mod_type_t	, scasti_mod)
NEOIP_ARGPACK_DECL_ITEM(casti_swarm_arg_t	, http_uri_t		, http_peersrc_uri)

public:	bt_err_t	check()	const throw();

NEOIP_ARGPACK_DECL_CLASS_END(casti_swarm_arg_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_SWARM_ARG_HPP__  */






