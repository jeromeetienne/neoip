/*! \file
    \brief Header of the \ref btcli_ezswarm_info_t

*/


#ifndef __NEOIP_BTCLI_EZSWARM_INFO_HPP__ 
#define __NEOIP_BTCLI_EZSWARM_INFO_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_file_size.hpp"
#include "neoip_bt_swarm_stats.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_ezswarm_t;

/** \brief to store the parameters replied by a tracker
 */
class btcli_ezswarm_info_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	btcli_ezswarm_info_t()				throw()	{}
	btcli_ezswarm_info_t(bt_ezswarm_t *bt_ezswarm)	throw();
	
	/*************** query function	***************************************/
	bool		is_null()		const throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	btcli_ezswarm_info_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( bt_id_t		, infohash);
	RES_VAR_DIRECT( std::string	, mfile_name);
	RES_VAR_DIRECT( std::string	, cur_state);
	RES_VAR_DIRECT( file_size_t	, totfile_size);

	// TODO all those are for SHARE only, make it apparent somehow
	RES_VAR_DIRECT( file_size_t	, totfile_anyavail);
	RES_VAR_DIRECT( double		, recv_rate);
	RES_VAR_DIRECT( double		, xmit_rate);
	RES_VAR_DIRECT( size_t		, peersrc_nb_leech);
	RES_VAR_DIRECT( size_t		, peersrc_nb_seed);
	RES_VAR_DIRECT( size_t		, swarm_nb_leech);
	RES_VAR_DIRECT( size_t		, swarm_nb_seed);
	RES_VAR_DIRECT( bt_swarm_stats_t, swarm_stats);
	
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const btcli_ezswarm_info_t &ezswarm_info) throw()
					{ return oss << ezswarm_info.to_string();	}

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const btcli_ezswarm_info_t &ezswarm_info)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, btcli_ezswarm_info_t &ezswarm_info)		throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BTCLI_EZSWARM_INFO_HPP__  */



