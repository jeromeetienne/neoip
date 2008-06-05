/*! \file
    \brief Header of the bt_peerpick_helper_t class
    
*/


#ifndef __NEOIP_BT_PEERPICK_HELPER_HPP__ 
#define __NEOIP_BT_PEERPICK_HELPER_HPP__ 
/* system include */
#include <stddef.h>
/* local include */
#include "neoip_bt_peerpick_vapi.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_peerpick_vapi_t;
class	bt_peerpick_mode_t;
class	bt_peerpick_profile_t;
class	bt_swarm_t;
class	bt_swarm_full_t;
class	bt_err_t;
class	bt_reqauth_type_t;
class	delay_t;

/** \brief static helpers to manipulate bt_peerpick_vapi_t
 */
class bt_peerpick_helper_t {
public:
	static	bt_peerpick_vapi_t *	vapi_ctor(const bt_peerpick_mode_t &mode
						, bt_swarm_t *bt_swarm
						, const bt_peerpick_profile_t &profile
						, bt_err_t &bt_err_out)		throw();

	// TODO find better name for those functions? find_* ?
	static bt_swarm_full_t *	pick_random_any(bt_swarm_t *bt_swarm);
	static bt_swarm_full_t *	pick_random_fstart(bt_swarm_t *bt_swarm);
	static bt_swarm_full_t *	pick_fastest_recv(bt_swarm_t *bt_swarm);
	static bt_swarm_full_t *	pick_highest_giver(bt_swarm_t *bt_swarm);

// TODO find better name for those functions
	typedef bt_swarm_full_t * (*swarm_full_picker_fct_t)(bt_swarm_t *bt_swarm);
	static void pick_many_cnx(const bt_reqauth_type_t &reqauth_type
				, const delay_t &reqauth_delay, double reqauth_prec
				, size_t nb_cnx, bt_swarm_t *bt_swarm
				, swarm_full_picker_fct_t picker_fct)		throw();
	static void pick_one_cnx(const bt_reqauth_type_t &reqauth_type
				, const delay_t &reqauth_delay, double reqauth_prec
				, bt_swarm_full_t *swarm_full)			throw();
				
				
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERPICK_HELPER_HPP__  */



