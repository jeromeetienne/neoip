/*! \file
    \brief Header of the \ref btcli_ezsession_info_t

*/


#ifndef __NEOIP_BTCLI_EZSESSION_INFO_HPP__ 
#define __NEOIP_BTCLI_EZSESSION_INFO_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_ezsession_t;

/** \brief to store the parameters replied by a tracker
 */
class btcli_ezsession_info_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	btcli_ezsession_info_t()				throw()	{}
	btcli_ezsession_info_t(bt_ezsession_t *bt_ezsession)	throw();
	
	/*************** query function	***************************************/
	bool		is_null()		const throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	btcli_ezsession_info_t &var_name(const var_type &var_name)	throw()			\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( double		, recv_maxrate);
	RES_VAR_DIRECT( double		, xmit_maxrate);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const btcli_ezsession_info_t &ezsession_info) throw()
					{ return oss << ezsession_info.to_string();	}

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const btcli_ezsession_info_t &ezsession_info)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, btcli_ezsession_info_t &ezsession_info)		throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BTCLI_EZSESSION_INFO_HPP__  */



