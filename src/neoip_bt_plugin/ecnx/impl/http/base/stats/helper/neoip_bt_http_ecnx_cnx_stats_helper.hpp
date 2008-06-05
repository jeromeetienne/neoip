/*! \file
    \brief Header of the bt_http_ecnx_cnx_stats_helper_t class
    
*/


#ifndef __NEOIP_BT_HTTP_ECNX_CNX_STATS_HELPER_HPP__ 
#define __NEOIP_BT_HTTP_ECNX_CNX_STATS_HELPER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_http_ecnx_cnx_stats_t;

/** \brief static helpers to manipulate bt_http_ecnx_cnx_stats_t
 */
class bt_http_ecnx_cnx_stats_helper_t {
public:
	static	std::string	to_html(const bt_http_ecnx_cnx_stats_t &cnx_stats)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTP_ECNX_CNX_STATS_HELPER_HPP__  */



