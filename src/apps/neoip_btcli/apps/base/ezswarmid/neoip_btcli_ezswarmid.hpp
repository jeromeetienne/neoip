/*! \file
    \brief Header of the \ref btcli_ezswarmid_t

*/


#ifndef __NEOIP_BTCLI_EZSWARMID_HPP__ 
#define __NEOIP_BTCLI_EZSWARMID_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_bt_id.hpp"
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_ezsession_t;
class	bt_ezswarm_t;

/** \brief to store the parameters replied by a tracker
 */
class btcli_ezswarmid_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** query function	***************************************/
	bool		is_null()						const throw();

	/*************** from/to bt_ezswarm_t	*******************************/
	bt_ezswarm_t *	to_bt_ezswarm(const bt_ezsession_t *bt_ezsession)	const throw();
	static btcli_ezswarmid_t	from_bt_ezswarm(const bt_ezswarm_t *bt_ezswarm)	throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	btcli_ezswarmid_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( bt_id_t		, infohash);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const btcli_ezswarmid_t &btcli_ezswarmid) throw()
						{ return oss << btcli_ezswarmid.to_string();	}

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const btcli_ezswarmid_t &btcli_ezswarmid)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, btcli_ezswarmid_t &btcli_ezswarmid)		throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BTCLI_EZSWARMID_HPP__  */



