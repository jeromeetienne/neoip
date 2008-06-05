/*! \file
    \brief Declaration of static helper functions for the btjamrc4 layer
    
*/


#ifndef __NEOIP_BT_JAMRC4_HELPER_HPP__ 
#define __NEOIP_BT_JAMRC4_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_id_t;
class	skey_ciph_t;

/** \brief static helpers for the bt_jamrc4_full_t
 */
class bt_jamrc4_helper_t {
public:
	static skey_ciph_t *	build_xmit_ciph(const bt_id_t &itor_nonce
					, const bt_id_t &resp_nonce, bool is_itor)	throw();
	static skey_ciph_t *	build_recv_ciph(const bt_id_t &itor_nonce
					, const bt_id_t &resp_nonce, bool is_itor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMRC4_HELPER_HPP__  */










