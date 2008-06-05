/*! \file
    \brief Header of the \ref bt_swarm_stats_t

*/


#ifndef __NEOIP_BT_SWARM_STATS_HPP__ 
#define __NEOIP_BT_SWARM_STATS_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_bt_swarm_stats_wikidbg.hpp"
#include "neoip_serial.hpp"
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_file_size.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the statistic about a bt_swarm_t
 */
class bt_swarm_stats_t : NEOIP_COPY_CTOR_ALLOW	
			, private wikidbg_obj_t<bt_swarm_stats_t, bt_swarm_stats_wikidbg_init>
			{
private:
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_stats_t()	throw();

	/*************** Query function	***************************************/
	double		share_ratio()		const throw();
	file_size_t	uloaded_overhd()	const throw();
	file_size_t	dloaded_overhd()	const throw();
	double		uloaded_overhd_ratio()	const throw();
	double		dloaded_overhd_ratio()	const throw();

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_swarm_stats_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( file_size_t	, uloaded_datalen);
	RES_VAR_DIRECT( file_size_t	, dloaded_datalen);
	RES_VAR_DIRECT( file_size_t	, uloaded_fulllen);
	RES_VAR_DIRECT( file_size_t	, dloaded_fulllen);
	RES_VAR_DIRECT( file_size_t	, deleted_dload_datalen);
	RES_VAR_DIRECT( size_t		, nb_hash_failed);
	RES_VAR_DIRECT( size_t		, timedout_req_nb);
	RES_VAR_DIRECT( size_t		, total_req_nb);
	RES_VAR_DIRECT( file_size_t	, total_req_len);
	RES_VAR_DIRECT( size_t		, dup_req_nb);
	RES_VAR_DIRECT( file_size_t	, dup_req_len);
	RES_VAR_DIRECT( size_t		, dup_rep_nb);
	RES_VAR_DIRECT( file_size_t	, dup_rep_len);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const bt_swarm_stats_t &swarm_stats) throw()
						{ return os << swarm_stats.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bt_swarm_stats_t &swarm_stats)	throw();
	friend	serial_t& operator >> (serial_t & serial, bt_swarm_stats_t &swarm_stats) 	throw(serial_except_t);	

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const bt_swarm_stats_t &swarm_stats)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, bt_swarm_stats_t &swarm_stats)		throw(xml_except_t);

	/*************** List of friend class	*******************************/
	friend class	bt_swarm_stats_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_STATS_HPP__  */



