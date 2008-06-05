/*! \file
    \brief Header of the \ref http_sclient_res_t

*/


#ifndef __NEOIP_HTTP_SCLIENT_RES_HPP__ 
#define __NEOIP_HTTP_SCLIENT_RES_HPP__ 
/* system include */
/* local include */
#include "neoip_http_reqhd.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_http_err.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the param in the nlay stack
 */
class http_sclient_res_t : NEOIP_COPY_CTOR_ALLOW {
public:
	/*************** query function	***************************************/
	bool	is_null()	const throw()	{ return http_reqhd().is_null() || cnx_err().is_null();}
	bool	is_get_ok()	const throw()	{ return full_get_ok() || part_get_ok();	}
	bool	is_post_ok()	const throw();
	bool	full_get_ok()	const throw();
	bool	part_get_ok()	const throw();
	bool	full_head_ok()	const throw();

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	http_sclient_res_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( http_err_t	, cnx_err);
	RES_VAR_DIRECT( http_reqhd_t	, http_reqhd);
	RES_VAR_DIRECT( http_rephd_t	, http_rephd);
	RES_VAR_STRUCT( bytearray_t	, reply_body);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const http_sclient_res_t &http_sclient_res ) throw()
						{ return os << http_sclient_res.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SCLIENT_RES_HPP__  */



