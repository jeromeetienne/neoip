/*! \file
    \brief Header of the neoip_string class
    
*/


#ifndef __NEOIP_ROUTER_APPS_HELPER_HPP__ 
#define __NEOIP_ROUTER_APPS_HELPER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	router_err_t;
class	router_lident_t;
class	router_rident_t;
class	router_rident_arr_t;
class	router_rootca_t;
class	router_rootca_arr_t;
class	router_acache_t;
class	file_path_t;
class	x509_cert_t;
class	x509_privkey_t;
class	router_name_t;

/** \brief helper to manipulate string
 */
class router_apps_helper_t {
public:
	/*************** lident load/save	*******************************/
	static router_err_t	lident_create(const file_path_t &config_path, const router_name_t &dnsname
						, size_t key_len, const x509_cert_t &ca_cert
						, const x509_privkey_t &ca_privkey)			throw();
	static router_err_t	lident_load(const file_path_t &config_path, router_lident_t &lident)	throw();

	/*************** rident load/save	*******************************/
	static router_err_t	rident_save(const file_path_t &filename, const router_rident_t &rident)	throw();
	static router_err_t	rident_load(const file_path_t &filename, router_rident_t &rident)	throw();
	static router_err_t	rident_arr_load(const file_path_t &config_path, router_rident_arr_t &arr)throw();

	/*************** rootca load/save	*******************************/
	static router_err_t	rootca_create(const file_path_t &config_path, const router_name_t &dnsname
								, size_t key_len)			throw();
	static router_err_t	rootca_load(const file_path_t &filename, router_rootca_t &rootca)	throw();
	static router_err_t	rootca_arr_load(const file_path_t &config_path, router_rootca_arr_t &arr)throw();
	static router_err_t	rootca_load_for_authsign(const router_name_t &dnsname, x509_cert_t &ca_cert
									, x509_privkey_t &ca_privkey)	throw();

	/*************** acache load/save	*******************************/
	static router_err_t	acache_load(const file_path_t &config_path, router_acache_t &acache)	throw();
	static router_err_t	acache_save(const file_path_t &config_path, const router_acache_t &acache)throw();

	/*************** disp_info function	*******************************/		
	static router_err_t	disp_info_lident()		throw();
	static router_err_t	disp_info_rident_arr()		throw();
	static router_err_t	disp_info_rootca_arr()		throw();
	static router_err_t	disp_info_acl()			throw();
	static router_err_t	disp_info_acache()		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_APPS_HELPER_HPP__  */



