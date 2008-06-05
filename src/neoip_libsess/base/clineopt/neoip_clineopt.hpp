/*! \file
    \brief Header of the \ref clineopt_t

*/


#ifndef __NEOIP_CLINEOPT_HPP__ 
#define __NEOIP_CLINEOPT_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_item_arr.hpp"
#include "neoip_clineopt_mode.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the file in bt_mfile_t
 */
class clineopt_t : NEOIP_COPY_CTOR_ALLOW {
public:
	typedef item_arr_t<std::string>	alias_db_t;
private:
public:
	/*************** ctor/dtor	***************************************/
	clineopt_t()								throw()	{}
	clineopt_t(const std::string &canonical_name, const clineopt_mode_t &param_mode)	throw()
				{ this->canonical_name(canonical_name).param_mode(param_mode);	}
	/*************** query function	***************************************/
	bool		is_null()		const throw()	{ return canonical_name().empty();	}
	bool		may_handle(const std::string &str)const throw();

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	clineopt_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( std::string		, canonical_name);	//!< the string
	RES_VAR_STRUCT( alias_db_t		, alias_name_db);
	RES_VAR_DIRECT( std::string		, default_value);
	RES_VAR_DIRECT( std::string		, help_string);
	RES_VAR_DIRECT( clineopt_mode_t		, param_mode);
	RES_VAR_DIRECT( clineopt_mode_t		, option_mode);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const clineopt_t &clineopt ) throw()
						{ return os << clineopt.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CLINEOPT_HPP__  */



