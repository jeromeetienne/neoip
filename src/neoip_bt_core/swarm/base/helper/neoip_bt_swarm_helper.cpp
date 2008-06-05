/*! \file
    \brief Definition of the \ref bt_swarm_helper_t class
    
*/

/* system include */
/* local include */
#include "neoip_bt_swarm_helper.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_sched.hpp"
#include "neoip_bt_swarm_peersrc.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm_sched_piece.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Set the bt_pieceprec_arr_t AND delete all piece already available but now notneeded
 * 
 * - NOTE: this is a slow function because it is testing each piece 
 *   - so its performance is directly related to bt_mfile_t::nb_piece()
 * - TODO find a better name for this function
 */
void	bt_swarm_helper_t::pieceprec_arr_del_notneeded(bt_swarm_t *bt_swarm
				, const bt_pieceprec_arr_t &pieceprec_arr)	throw()
{
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	// set the bt_pieceprec_arr_t 
	set_pselect_fixed_pieceprec_arr(bt_swarm, pieceprec_arr);

	// go thru each pieceidx - to remove piece nomore in pieceq and locally avail
	for(size_t pieceidx = 0; pieceidx < bt_mfile.nb_piece(); pieceidx++ ){
		// if this pieceidx is needed, goto the next
		if( pieceprec_arr[pieceidx].is_needed() )		continue;
		// if this pieceidx IS NOT locally available, goto the next
		if( bt_swarm->local_pavail().is_unavail(pieceidx) )	continue;
		// declare this piece as nomore avail
		bt_swarm->declare_piece_nomore_avail(pieceidx);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   query nb leecher/seeder
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the total number of leech according to bt_swarm_peersrc_t
 * 
 * - aka the total number in the swarm, they may or may NOT be connected to 
 *   the local peer
 */
size_t	bt_swarm_helper_t::peersrc_nb_leech(const bt_swarm_t *bt_swarm)		throw()
{
	bt_swarm_peersrc_t *	swarm_peersrc	= bt_swarm->swarm_peersrc();
	// return the number from the bt_swarm_peersrc_t
	return swarm_peersrc->nb_leecher();
}

/** \brief Return the total number of seed according to bt_swarm_peersrc_t
 * 
 * - aka the total number in the swarm, they may or may NOT be connected to 
 *   the local peer
 */
size_t	bt_swarm_helper_t::peersrc_nb_seed(const bt_swarm_t *bt_swarm)		throw()
{
	bt_swarm_peersrc_t *	swarm_peersrc	= bt_swarm->swarm_peersrc();
	// return the number from the bt_swarm_peersrc_t
	return swarm_peersrc->nb_seeder();
}

/** \brief Return the number of leech connected by the bt_swarm_t
 */
size_t	bt_swarm_helper_t::swarm_nb_leech(const bt_swarm_t *bt_swarm)		throw()
{
	size_t	nb_leech	= 0;
	// go thru the whole bt_swarm_t::full_db
	const std::list<bt_swarm_full_t *> &		full_db = bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>::const_iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		const bt_swarm_full_t * swarm_full	= *iter;
		// if this bt_swarm_full_t is a leech, increase the counter
		if( swarm_full->is_leech() )	nb_leech++;
	}
	// return the just computed value
	return nb_leech;
}

/** \brief Return the number of seed connected by the bt_swarm_t
 */
size_t	bt_swarm_helper_t::swarm_nb_seed(const bt_swarm_t *bt_swarm)		throw()
{
	size_t	nb_seed	= 0;
	// go thru the whole bt_swarm_t::full_db
	const std::list<bt_swarm_full_t *> &		full_db = bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>::const_iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		const bt_swarm_full_t * swarm_full	= *iter;
		// if this bt_swarm_full_t is a seed, increase the counter
		if( swarm_full->is_seed() )	nb_seed++;
	}
	// return the just computed value
	return nb_seed;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			stat on bt_swarm_full_t state
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the number of bt_swarm_full_t with local_doauth_req
 */
size_t	bt_swarm_helper_t::nb_full_local_doauth_req(const bt_swarm_t *bt_swarm)	throw()
{
	size_t	nb_full	= 0;
	// go thru the whole bt_swarm_t::full_db
	const std::list<bt_swarm_full_t *> &		full_db = bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>::const_iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		const bt_swarm_full_t * swarm_full	= *iter;
		// if this bt_swarm_full_t has local_doauth_req, increase the counter
		if( swarm_full->local_doauth_req() )	nb_full++;
	}
	// return the just computed value
	return nb_full;
}


/** \brief Return the number of bt_swarm_full_t with local_dowant_req
 */
size_t	bt_swarm_helper_t::nb_full_local_dowant_req(const bt_swarm_t *bt_swarm)	throw()
{
	size_t	nb_full	= 0;
	// go thru the whole bt_swarm_t::full_db
	const std::list<bt_swarm_full_t *> &		full_db = bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>::const_iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		const bt_swarm_full_t * swarm_full	= *iter;
		// if this bt_swarm_full_t has local_dowant_req, increase the counter
		if( swarm_full->local_dowant_req() )	nb_full++;
	}
	// return the just computed value
	return nb_full;
}


/** \brief Return the number of bt_swarm_full_t with other_doauth_req
 */
size_t	bt_swarm_helper_t::nb_full_other_doauth_req(const bt_swarm_t *bt_swarm)	throw()
{
	size_t	nb_full	= 0;
	// go thru the whole bt_swarm_t::full_db
	const std::list<bt_swarm_full_t *> &		full_db = bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>::const_iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		const bt_swarm_full_t * swarm_full	= *iter;
		// if this bt_swarm_full_t has other_doauth_req, increase the counter
		if( swarm_full->other_doauth_req() )	nb_full++;
	}
	// return the just computed value
	return nb_full;
}


/** \brief Return the number of bt_swarm_full_t with other_dowant_req
 */
size_t	bt_swarm_helper_t::nb_full_other_dowant_req(const bt_swarm_t *bt_swarm)	throw()
{
	size_t	nb_full	= 0;
	// go thru the whole bt_swarm_t::full_db
	const std::list<bt_swarm_full_t *> &		full_db = bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>::const_iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		const bt_swarm_full_t * swarm_full	= *iter;
		// if this bt_swarm_full_t has other_dowant_req, increase the counter
		if( swarm_full->other_dowant_req() )	nb_full++;
	}
	// return the just computed value
	return nb_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the anyavail_len for this pieceidx
 * 
 * - it tests for fullavail AND for partavail
 */
size_t	bt_swarm_helper_t::pieceidx_anyavail_len(const bt_swarm_t *bt_swarm
							, size_t pieceidx)	throw()
{
	// if the pieceidx is fullavail, return its length
	if( bt_swarm->local_pavail().is_avail(pieceidx) )
		return bt_unit_t::pieceidx_to_piecelen(pieceidx, bt_swarm->get_mfile());

	// if the pieceidx is partavail, return the partavail length
	bt_swarm_sched_t *	swarm_sched	= bt_swarm->swarm_sched();
	bt_swarm_sched_piece_t*	sched_piece	= swarm_sched->piece_by_idx(pieceidx);
	if( sched_piece )	return sched_piece->partavail_len().to_size_t();

	// if the pieceidx is not fullavail or partavail, return 0
	return 0;
}

/** \brief return the sum of all the bt_swarm_full_t::recv_rate of reqauth_type 
 */
double	bt_swarm_helper_t::recv_rate_for_reqauth_type(const bt_swarm_t *bt_swarm
					, const bt_reqauth_type_t &reqauth_type)	throw()
{
	double	total_rate	= 0;
	// go thru the whole bt_swarm_t::full_db
	const std::list<bt_swarm_full_t *> &		full_db = bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>::const_iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		const bt_swarm_full_t * 	swarm_full	= *iter;
		const bt_swarm_full_sched_t *	full_sched	= swarm_full->full_sched();
		// if this bt_swarm_full_t is not of reqauth_type, goto the next
		if( full_sched->reqauth_type() != reqauth_type )	continue; 
		// update the total rate
		total_rate	+= swarm_full->recv_rate_avg();
	}
	// return the just computed value
	return total_rate;
}

/** \brief return the sum of all the bt_swarm_full_t::xmit_rate of reqauth_type 
 */
double	bt_swarm_helper_t::xmit_rate_for_reqauth_type(const bt_swarm_t *bt_swarm
					, const bt_reqauth_type_t &reqauth_type)	throw()
{
	double	total_rate	= 0;
	// go thru the whole bt_swarm_t::full_db
	const std::list<bt_swarm_full_t *> &		full_db = bt_swarm->get_full_db();
	std::list<bt_swarm_full_t *>::const_iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		const bt_swarm_full_t * 	swarm_full	= *iter;
		const bt_swarm_full_sched_t *	full_sched	= swarm_full->full_sched();
		// if this bt_swarm_full_t is not of reqauth_type, goto the next
		if( full_sched->reqauth_type() != reqauth_type )	continue; 
		// update the total rate
		total_rate	+= swarm_full->xmit_rate_avg();
	}
	// return the just computed value
	return total_rate;
}
NEOIP_NAMESPACE_END


