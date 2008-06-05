/*! \file
    \brief Header of the \ref router_acl_item_t

*/


#ifndef __NEOIP_ROUTER_ACL_ITEM_HPP__ 
#define __NEOIP_ROUTER_ACL_ITEM_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_router_acl_type.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store a single item in the router_acl_t
 */
class router_acl_item_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return type().is_null();	}
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	router_acl_item_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_STRUCT( router_acl_type_t	, type);
	RES_VAR_STRUCT( std::string		, pattern);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const router_acl_item_t &acl_item ) throw()
						{ return os << acl_item.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ACL_ITEM_HPP__  */



