/*! \file
    \brief Header for handling the convertion of bt_mfile_t from/to torrent file
    
*/


#ifndef __NEOIP_OLOAD_HELPER_HPP__ 
#define __NEOIP_OLOAD_HELPER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_http_uri.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_mfile_t;
class	bt_httpo_full_t;
class	oload_mod_vapi_t;
class	http_status_t;

/** \brief static helpers to manipulate bt_mfile_t
 */
class oload_helper_t {
private:
	static size_t	find_matching_subfile_idx(const bt_mfile_t &bt_mfile
						, const http_uri_t &nested_uri)	throw();
	
public:
	/*************** inner/outter parsing	*******************************/
	static http_uri_t	parse_outter_uri(const http_uri_t &nested_uri)	throw();
	static http_uri_t	parse_inner_uri(const http_uri_t &nested_uri)	throw();


	/*************** subfile stuff	***************************************/
	static size_t	subfile_idx_by_nested_uri(const bt_mfile_t &bt_mfile
						, const http_uri_t &nested_uri)	throw();
	static bool	has_subfile_idx_by_nested_uri(const bt_mfile_t &bt_mfile
						, const http_uri_t &nested_uri)	throw();
	static void	process_subfile(http_uri_t &outter_uri
						, http_uri_t &inner_uri)	throw();


	/*************** dupuri stuff	***************************************/
	static void	update_bt_mfile_with_dupuri(bt_mfile_t &bt_mfile
						, const http_uri_t &nested_uri)	throw();
	
	static std::list<http_uri_t> extract_dupuri_db(const http_uri_t &nested_uri)	throw();

	/*************** bt_httpo_full_t stuff	*******************************/
	static oload_mod_vapi_t *mod_vapi_from(const bt_httpo_full_t * httpo_full)throw();
	
	static void	reply_err_httpo_full(bt_httpo_full_t * httpo_full
					, const http_status_t &http_status
					, const std::string &reason_phrase
					, const http_uri_t &redir_uri = http_uri_t())	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_HELPER_HPP__  */










