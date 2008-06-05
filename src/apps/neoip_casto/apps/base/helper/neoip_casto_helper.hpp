/*! \file
    \brief Header for handling the convertion of bt_mfile_t from/to torrent file
    
*/


#ifndef __NEOIP_CASTO_HELPER_HPP__ 
#define __NEOIP_CASTO_HELPER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_httpo_full_t;
class	http_status_t;

/** \brief static helpers to manipulate bt_mfile_t
 */
class casto_helper_t {
public:
	static void	reply_err_httpo_full(bt_httpo_full_t * httpo_full
					, const http_status_t &status_code
					, const std::string &reason_phrase)	throw();	
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTO_HELPER_HPP__  */










