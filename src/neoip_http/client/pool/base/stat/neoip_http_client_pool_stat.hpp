/*! \file
    \brief Header of the \ref http_client_pool_stat_t

*/


#ifndef __NEOIP_HTTP_CLIENT_POOL_STAT_HPP__ 
#define __NEOIP_HTTP_CLIENT_POOL_STAT_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the statistic about a bt_swarm_t
 */
class http_client_pool_stat_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	http_client_pool_stat_t()	throw();

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	http_client_pool_stat_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( size_t	, nb_cnx_peak);
	RES_VAR_DIRECT( size_t	, nb_cnx_put);
	RES_VAR_DIRECT( size_t	, nb_cnx_get);
	RES_VAR_DIRECT( size_t	, nb_cnx_died_internal);
	RES_VAR_DIRECT( size_t	, nb_cnx_died_external);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const http_client_pool_stat_t &pool_stat) throw()
						{ return os << pool_stat.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_CLIENT_POOL_STAT_HPP__  */



