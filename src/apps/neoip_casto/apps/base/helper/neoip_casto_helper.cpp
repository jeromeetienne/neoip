/*! \file
    \brief Definition for casto_helper_t

*/

/* system include */
/* local include */
#include "neoip_casto_helper.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_status.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Reply an error http_rephd_t to this httpo_full_t
 * 
 * - this is used to reply an error just before deleting the bt_httpo_full_t
 */
void	casto_helper_t::reply_err_httpo_full(bt_httpo_full_t * httpo_full
					, const http_status_t &status_code
					, const std::string &reason_phrase)	throw()
{
	http_rephd_t	http_rephd; 
	// build the http_rephd_t
	http_rephd.version(http_version_t::V1_1).status_code(status_code.get_value())
				.reason_phrase(reason_phrase);
	// Start the bt_httpo_full_t but in reply error
	httpo_full->start_reply_error( http_rephd );
}

NEOIP_NAMESPACE_END;






