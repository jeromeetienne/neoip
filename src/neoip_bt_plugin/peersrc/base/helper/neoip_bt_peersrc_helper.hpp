/*! \file
    \brief Header of the bt_peersrc_helper_t class
    
*/


#ifndef __NEOIP_BT_PEERSRC_HELPER_HPP__ 
#define __NEOIP_BT_PEERSRC_HELPER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ipport_addr_arr_t;
class	bt_peersrc_peer_arr_t;

/** \brief static helpers to manipulate bt_peersrc_t stuff
 */
class bt_peersrc_helper_t {
public:
	/**************	compact format stuff	*******************************/
	static std::string	 	ipport_addr_arr_to_compactfmt(const ipport_addr_arr_t &ipport_addr_arr)throw();
	static ipport_addr_arr_t 	ipport_addr_arr_from_compactfmt(const std::string &str_compactfmt)	throw();
	static std::string		peer_arr_to_compactfmt(const bt_peersrc_peer_arr_t &peer_arr)	throw();
	static bt_peersrc_peer_arr_t	peer_arr_from_compactfmt(const std::string &str_compactfmt)	throw();
	static std::string		peer_arr_to_peerflag(const bt_peersrc_peer_arr_t &peer_arr)	throw();
	static void			peer_arr_from_peerflag(bt_peersrc_peer_arr_t &peer_arr
								, const std::string &str_peerflag)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERSRC_HELPER_HPP__  */



