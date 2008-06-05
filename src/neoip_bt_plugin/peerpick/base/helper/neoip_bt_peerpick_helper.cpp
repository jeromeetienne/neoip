/*! \file
    \brief Definition of the \ref bt_peerpick_helper_t class

\par TODO unify common part
- peer_select_new_fast and peer_select_new_rand are in casto and plain
  - unify this
- moreover im sure the others implementation got other things in common
  - maximize the entropy to factorizing  

\par Implementation notes 
- All the connection picker pick a single connection according to various criteria
  - common criteria: (i) the remote peer MUST want to requet the local peer (no need
    to allow remote peer to upload from local peer if remote peer doesnt want to)
    (ii) the remote_peer MUST NOT have already a bt_reqauth_type_t (aka it must
    be in bt_reqauth_type_t::DENY)

*/

/* system include */
/* local include */
#include "neoip_bt_peerpick_helper.hpp"
#include "neoip_bt_peerpick_profile.hpp"
#include "neoip_bt_peerpick_mode.hpp"
#include "neoip_bt_peerpick_plain.hpp"
#include "neoip_bt_peerpick_relay.hpp"
#include "neoip_bt_peerpick_casti.hpp"
#include "neoip_bt_peerpick_casto.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_prec.hpp"
#include "neoip_bt_swarm_full_sched.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_utmsg_fstart_cnx.hpp"
#include "neoip_bt_utmsg_byteacct_cnx.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Construct the bt_peerpick_vapi_t
 */
bt_peerpick_vapi_t *	bt_peerpick_helper_t::vapi_ctor(const bt_peerpick_mode_t &peerpick_mode
					, bt_swarm_t *bt_swarm
					, const bt_peerpick_profile_t &profile
					, bt_err_t &bt_err_out)	throw()
{
	bt_peerpick_vapi_t *	peerpick_vapi	= NULL;
	// build the bt_peerpick_vapi_t depending on the bt_peerpick_mode_t
	if( peerpick_mode == bt_peerpick_mode_t::PLAIN ){
		bt_peerpick_plain_t *	peerpick_plain;
		peerpick_plain	= nipmem_new bt_peerpick_plain_t();
		bt_err_out	= peerpick_plain->profile(profile.plain()).start(bt_swarm);
		peerpick_vapi	= peerpick_plain;
	}else if( peerpick_mode == bt_peerpick_mode_t::RELAY ){
		bt_peerpick_relay_t *	peerpick_relay;
		peerpick_relay	= nipmem_new bt_peerpick_relay_t();
		bt_err_out	= peerpick_relay->profile(profile.relay()).start(bt_swarm);
		peerpick_vapi	= peerpick_relay;
	}else if( peerpick_mode == bt_peerpick_mode_t::CASTI ){
		bt_peerpick_casti_t *	peerpick_casti;
		peerpick_casti	= nipmem_new bt_peerpick_casti_t();
		bt_err_out	= peerpick_casti->profile(profile.casti()).start(bt_swarm);
		peerpick_vapi	= peerpick_casti;
	}else if( peerpick_mode == bt_peerpick_mode_t::CASTO ){
		bt_peerpick_casto_t *	peerpick_casto;
		peerpick_casto	= nipmem_new bt_peerpick_casto_t();
		bt_err_out	= peerpick_casto->profile(profile.casto()).start(bt_swarm);
		peerpick_vapi	= peerpick_casto;
	}else {
		DBG_ASSERT( 0 );
	}
	// if an error occured, delete the bt_peerpick_vapi_t
	if( bt_err_out.failed() )	nipmem_zdelete peerpick_vapi;
	// return the pointer
	return peerpick_vapi;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			peer picker according to various criteria
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Pick a bt_swarm_full_t at random among the ones which other_dowant_req
 *         and are currently in bt_reqauth_type_t::DENY 
 */
bt_swarm_full_t	* bt_peerpick_helper_t::pick_random_any(bt_swarm_t *bt_swarm)
{
	std::list<bt_swarm_full_t *> &	swarm_full_db	= bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>	cand_full_db;
	std::list<bt_swarm_full_t *>::iterator iter;
	// go thru the whole swarm_full_db to copy all the candidates
	for(iter = swarm_full_db.begin(); iter != swarm_full_db.end(); iter++){
		bt_swarm_full_t *	swarm_full	= *iter;
		bt_swarm_full_sched_t *	full_sched	= swarm_full->full_sched();
		// if the remote peer doesnt want to request, no need to authorize it
		if( swarm_full->other_dowant_req() == false )			continue;
		// if the full_sched is not in bt_reqauth_type_t::DENY, dont autorize it
		if( full_sched->reqauth_type() != bt_reqauth_type_t::DENY )	continue;
		// put this swarm_full in the cand_full_db
		cand_full_db.push_back(swarm_full);
	}
	// if the cand_full_db is empty, return NULL now
	if( cand_full_db.empty() )	return NULL;
	// pick one bt_swarm_full_t randoming among all the cand_full_db
	iter	= cand_full_db.begin();
	std::advance(iter, rand() % cand_full_db.size() );
	bt_swarm_full_t	* chosen_full	= *iter;
	// return the chosen bt_swarm_full_t
	return chosen_full;
}

/** \brief Pick a bt_swarm_full_t at random among the ones which other_dowant_req
 *         and are currently in bt_reqauth_type_t::DENY and have bt_utmsg_fstart_t set
 */
bt_swarm_full_t	* bt_peerpick_helper_t::pick_random_fstart(bt_swarm_t *bt_swarm)
{
	std::list<bt_swarm_full_t *> &	swarm_full_db	= bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>	cand_full_db;
	std::list<bt_swarm_full_t *>::iterator iter;
	// go thru the whole swarm_full_db to copy all the candidates
	for(iter = swarm_full_db.begin(); iter != swarm_full_db.end(); iter++){
		bt_swarm_full_t *	swarm_full	= *iter;
		bt_swarm_full_sched_t *	full_sched	= swarm_full->full_sched();
		bt_swarm_full_utmsg_t *	full_utmsg	= swarm_full->full_utmsg();
		bt_utmsg_cnx_vapi_t *	utmsg_cnx	= full_utmsg->cnx_vapi(bt_utmsgtype_t::FSTART);
		bt_utmsg_fstart_cnx_t *	fstart_cnx	= dynamic_cast<bt_utmsg_fstart_cnx_t*>(utmsg_cnx);
		// if the remote peer doesnt want to request, no need to authorize it
		if( swarm_full->other_dowant_req() == false )			continue;
		// if the full_sched is not in bt_reqauth_type_t::DENY, dont autorize it
		if( full_sched->reqauth_type() != bt_reqauth_type_t::DENY )	continue;
		// if this bt_swarm_full_t do not support bt_utmsgtype_t::FSTART, goto the next
		if( !fstart_cnx )						continue;
		// if this bt_utmsg_fstart_cnx_t has not remote_fstart() set, goto the next
		if( fstart_cnx->remote_fstart() == false )			continue;
		// put this swarm_full in the cand_full_db
		cand_full_db.push_back(swarm_full);
	}
	// if the cand_full_db is empty, return NULL now
	if( cand_full_db.empty() )	return NULL;
	// pick one bt_swarm_full_t randoming among all the cand_full_db
	iter	= cand_full_db.begin();
	std::advance(iter, rand() % cand_full_db.size() );
	bt_swarm_full_t	* chosen_full	= *iter;
	// return the chosen bt_swarm_full_t
	return chosen_full;
}

/** \brief Pick a bt_swarm_full_t with the fastest dload among the ones which other_dowant_req
 *         and are currently in bt_reqauth_type_t::DENY 
 */
bt_swarm_full_t	* bt_peerpick_helper_t::pick_fastest_recv(bt_swarm_t *bt_swarm)
{
	std::list<bt_swarm_full_t *> &	swarm_full_db	= bt_swarm->get_full_db();
	bt_swarm_full_t	*		chosen_full	= NULL;
	double				max_recv_rate	= -1;
	std::list<bt_swarm_full_t *>::iterator iter;
	// go thru the whole swarm_full_db to get the chosen_full with the highest recv_rate
	for(iter = swarm_full_db.begin(); iter != swarm_full_db.end(); iter++){
		bt_swarm_full_t *	swarm_full	= *iter;
		bt_swarm_full_sched_t *	full_sched	= swarm_full->full_sched();
		// if the remote peer doesnt want to request, no need to authorize it
		if( swarm_full->other_dowant_req() == false )			continue;
		// if the full_sched is not in bt_reqauth_type_t::DENY, dont autorize it
		if( full_sched->reqauth_type() != bt_reqauth_type_t::DENY )	continue;
		// if the receive rate of this one is greater than the max, update the max
		if( max_recv_rate < swarm_full->recv_rate_avg() ){
			max_recv_rate	= swarm_full->recv_rate_avg();
			chosen_full	= swarm_full;
		}
	}
	// return the chosen bt_swarm_full_t
	return chosen_full;
}

/** \brief Pick a bt_swarm_full_t with the highest giver (> 0) with bt_utmsg_byteacct_t
 *         among the ones which other_dowant_req
 *         and are currently in bt_reqauth_type_t::DENY 
 *         and are supporting bt_utmsg_byteacct_t
 */
bt_swarm_full_t	* bt_peerpick_helper_t::pick_highest_giver(bt_swarm_t *bt_swarm)
{
	std::list<bt_swarm_full_t *> &	swarm_full_db	= bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>::iterator iter;
	std::multimap<double, bt_swarm_full_t *>	cand_full_db;
	bt_swarm_full_t	*		chosen_full	= NULL;
	double				max_given_rate	= 0.0;
	// go thru the whole swarm_full_db to copy all the candidates
	for(iter = swarm_full_db.begin(); iter != swarm_full_db.end(); iter++){
		bt_swarm_full_t *	swarm_full	= *iter;
		bt_swarm_full_sched_t *	full_sched	= swarm_full->full_sched();
		bt_swarm_full_utmsg_t *	full_utmsg	= swarm_full->full_utmsg();
		bt_utmsg_cnx_vapi_t *	utmsg_cnx	= full_utmsg->cnx_vapi(bt_utmsgtype_t::BYTEACCT);
		bt_utmsg_byteacct_cnx_t*byteacct_cnx	= dynamic_cast<bt_utmsg_byteacct_cnx_t*>(utmsg_cnx);
		// if the remote peer doesnt want to request, no need to authorize it
		if( swarm_full->other_dowant_req() == false )			continue;
		// if the full_sched is not in bt_reqauth_type_t::DENY, dont autorize it
		if( full_sched->reqauth_type() != bt_reqauth_type_t::DENY )	continue;
		// if this bt_swarm_full_t do not support bt_utmsgtype_t::BYTEACCT, goto the next
		if( !byteacct_cnx )						continue;
		// set the balance to xmit_rate - recv_rate
		double	cur_given_rate	= byteacct_cnx->xmit_rate() - byteacct_cnx->recv_rate();
		// if cur_given_rate is less than max_given_rate, goto the next
		if( cur_given_rate <= max_given_rate )				continue;
		// if all previous tests passed, this bt_swarm_full_t is the 'best'
		max_given_rate	= cur_given_rate;
		chosen_full	= swarm_full;
	}
	// return the chosen bt_swarm_full_t
	return chosen_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch nb_cnx with the reqauth_type/reqauth_delay/reqauth_prec
 *         all picked by the picker_fct
 * 
 * - NOTE: this function has a LOT of parameters which is dirty
 *   - but it is only a internal helper to avoid duplicating this code in
 *     most bt_peerpick_vapi_t implementation. so it is ok
 */
void	bt_peerpick_helper_t::pick_many_cnx(const bt_reqauth_type_t &reqauth_type
			, const delay_t &reqauth_delay, double reqauth_prec, size_t nb_cnx
			, bt_swarm_t *bt_swarm, swarm_full_picker_fct_t picker_fct) throw()
{
	bt_swarm_full_t *	swarm_full;
	// loop at most nb_cnx time (it may be shorter of no more connection may be picked)
	for(size_t i = 0; i < nb_cnx; i++ ){
		// try to find a bt_swarm_full_t to pick
		swarm_full	= picker_fct(bt_swarm);
		// if none has been found, leave the loop
		if( !swarm_full )	break;
		// pick this bt_swarm_full
		pick_one_cnx(reqauth_type, reqauth_delay, reqauth_prec, swarm_full);
	}	
}

/** \brief pick one bt_swarm_full_t
 */
void	bt_peerpick_helper_t::pick_one_cnx(const bt_reqauth_type_t &reqauth_type
				, const delay_t &reqauth_delay, double reqauth_prec
				, bt_swarm_full_t *swarm_full)			throw()
{
	bt_swarm_full_sched_t *	full_sched	= swarm_full->full_sched();
	bt_swarm_full_prec_t *	full_prec	= swarm_full->full_prec();
	// set the reqauth_type to the bt_swarm_full_t
	full_sched->reqauth_type(reqauth_type, reqauth_delay);	
	// if needed, send the reqauth command in sync with the current bt_reqauth_type_t
	full_sched->sync_doauth_req_cmd();
	// set the reqauth_prec_xmit for the chosen_full 
	full_prec->reqauth_prec_xmit( reqauth_prec );
}


NEOIP_NAMESPACE_END


