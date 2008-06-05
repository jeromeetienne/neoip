/*! \file
    \brief Header of the \ref bt_http_ecnx_cnx_stats_t

*/


#ifndef __NEOIP_BT_HTTP_ECNX_CNX_STATS_HPP__ 
#define __NEOIP_BT_HTTP_ECNX_CNX_STATS_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_file_size.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_http_ecnx_iov_t;
class	http_sclient_res_t;

/** \brief to store the statistic about a bt_http_ecnx_cnx_t
 * 
 * - NOTE: this is only for diagnostic purpose. this MUST NOT be used in 'core' parts
 *   - thus it may be avoided during compilation to save memory and cpu
 */
class bt_http_ecnx_cnx_stats_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	bt_http_ecnx_cnx_stats_t()	throw();
	
	/*************** action function	*******************************/
	void	update_from_iov_res(bt_http_ecnx_iov_t *ecnx_iov, const http_sclient_res_t &sclient_res)throw();

	/*************** arithmetic operator	*******************************/
	bt_http_ecnx_cnx_stats_t &	operator +=(const bt_http_ecnx_cnx_stats_t &other)	throw();

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_http_ecnx_cnx_stats_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( file_size_t	, uloaded_ovhdlen);
	RES_VAR_DIRECT( file_size_t	, dloaded_datalen);
	RES_VAR_DIRECT( file_size_t	, dloaded_ovhdlen);
	RES_VAR_DIRECT( size_t		, iov_nb);
	RES_VAR_DIRECT( file_size_t	, iov_total_len);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const bt_http_ecnx_cnx_stats_t &cnx_stats) throw()
						{ return os << cnx_stats.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTP_ECNX_CNX_STATS_HPP__  */



