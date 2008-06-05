/*! \file
    \brief Class to handle the bt_swarm_sched_t
 
- TODO it doesnt update the bt_swarm_stats_t::dloaded_datalen
  - this should be kept into account in the scheduler instead ???

\par Note about next_pidx_hint and request sequencing
- if bt_swarm_sched_profile_t::seq_pselect_ifequ is set, the bt_pselect_vapi_t is hinted
  in order to select pieceidx in sequence if their importance are equal.
- in order to maximize the request sequencialisation, a 'next_pidx_hint' is maintained
  - it is reduces dramatically the overhead for bt_ecnx_vapi_t connection by allowing
    larger external request
  - so increase the speed of bt_ecnx_vapi_t.
- next_pidx_hint is a hint on the next pieceidx to select
  - it is honored by the bt_pselect_vapi_t on a adversory basis
    - aka, it will be the next selected pieceidx *IIF* possible without changing the 
      normal rules. aka it is the next_pidx_hint has the same remote_wish and
      bt_pieceprec_t than the normal piece.
    - e.g. if the next_pidx_hint is bt_pieceprec_t::NOTNEEDED or already available
      or already requested. or if another piece is considered more important (e.g. 
      with a higher precedence or with a lower remote_wish)
- thus if nothing disturbs the next_pidx_hint, first piece selection will be random
  and all following one will be made in sequence.
  - things that may disturbs the next_pidx_hint, piece with different bt_pieceprec_t,
    different rarity, or bt_swarm_full_t connection handling requests.
  - NOTE: multiple bt_ecnx_vapi_t interacts in sequences without disturbing each others
- this is honored by bt_swarm_sched_ecnx_t and bt_swarm_sched_full_t in ordered to 
  avoid fragmentation of the totfile space when transmitting request.
  - fragmentation occurs especially due to the randomness of the piece_idx for 
    the pieces of same bt_pieceprec_t and rarity
- As the next_pidx_hint starts as a random place, the inter peer randomness is 
  still garanteed.
  - this is important to ensure a good global spreading of the data when many
    leechs connects a small number of seed.

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm_sched_ecnx.hpp"
#include "neoip_bt_swarm_sched_full.hpp"
#include "neoip_bt_swarm_sched_piece.hpp"
#include "neoip_bt_swarm_sched_block.hpp"
#include "neoip_bt_swarm_sched_request.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_helper.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_bt_pselect_slide.hpp"
#include "neoip_bt_pselect_fixed.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_swarm_sched_t::bt_swarm_sched_t()		throw()
{
	// zero some fields
	m_bt_swarm	= NULL;
	m_sched_ecnx	= NULL;
	m_sched_full	= NULL;
	// set the next_pidx_hint to null pieceidx to get a random pieceidx at first
	m_next_pidx_hint= std::numeric_limits<size_t>::max();
}

/** \brief Destructor
 */
bt_swarm_sched_t::~bt_swarm_sched_t()		throw()
{
	// delete the bt_swarm_sched_ecnx_t
	nipmem_zdelete	m_sched_ecnx;
	// delete the bt_swarm_sched_full_t
	nipmem_zdelete	m_sched_full;
	// delete all the do_needreq bt_swarm_sched_piece_t if needed
	while( !piece_do_needreq_db.empty() )	nipmem_delete	piece_do_needreq_db.begin()->second;
	// delete all the no_needreq bt_swarm_sched_piece_t if needed
	while( !piece_no_needreq_db.empty() )	nipmem_delete	piece_no_needreq_db.begin()->second;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_swarm_sched_t &	bt_swarm_sched_t::set_profile(const bt_swarm_sched_profile_t &m_profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( m_profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= m_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_swarm_sched_t::start(bt_swarm_t *m_bt_swarm
			, const file_size_inval_t &partavail_piece_inval)	throw()
{
	bt_err_t		bt_err;
	// copy the parameter
	this->m_bt_swarm	= m_bt_swarm;

	// create the bt_swarm_sched_ecnx_t - the scheduler for the bt_ecnx_vapi_t
	m_sched_ecnx		= nipmem_new bt_swarm_sched_ecnx_t(this);
	// create the bt_swarm_sched_full_t - the scheduler for the bt_swarm_full_t
	m_sched_full		= nipmem_new bt_swarm_sched_full_t(this);
	
	// if some piece are already partially available, create their bt_swarm_sched_piece_t
	if( !partavail_piece_inval.empty() )
		partavail_piece_ctor(partavail_piece_inval);
		
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Create the bt_swarm_sched_piece_t for the partially available pieces
 * 
 * - NOTE: used for the bt_swarm_resumedata_t
 */
void	bt_swarm_sched_t::partavail_piece_ctor(const file_size_inval_t &partavail_piece_inval)	throw()
{
	const bt_mfile_t &	bt_mfile	= bt_swarm()->get_mfile();
	file_size_inval_t	current_inval	= partavail_piece_inval;
	bt_swarm_sched_piece_t *sched_piece;
	file_size_inval_t	piece_unavail_inval;
	file_size_inval_t	piece_isavail_inval;
	// log to debug
	KLOG_DBG("enter current_inval=" << current_inval);
	// loop until the current_inval is empty
	while( !current_inval.empty() ){
		// compute the piece_idx for the first item available
		file_size_t	first_offset	= current_inval[0].min_value();
		size_t	    	piece_idx	= bt_unit_t::totfile_to_pieceidx(first_offset, bt_mfile);
		// compute the totfile_range for this piece_idx
		file_range_t	piece_range	= bt_unit_t::pieceidx_to_totfile_range(piece_idx, bt_mfile);
		// compute the intersection between the piece_range and the current_inval
		piece_unavail_inval = file_size_inval_t(piece_range.to_inval_item()) - current_inval;
		piece_isavail_inval = file_size_inval_t(piece_range.to_inval_item()) - piece_unavail_inval;
		// sanity check - piece_isavail_inval MUST NOT be empty
		DBG_ASSERT( !piece_isavail_inval.empty() );
		// create the sched_piece - it links itself to the bt_swarm_sched_t
		sched_piece	= nipmem_new bt_swarm_sched_piece_t(this, piece_idx, piece_isavail_inval);
		// remove piece_isavail_inval from the current_inval
		current_inval	-= piece_isavail_inval;
		// sanity check - now the piece_range MUST be distinct from the current_inval
		DBG_ASSERT( current_inval.is_distinct(piece_range.to_inval_item()) );
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a file_size_inval_t of the data locally available of the incomplete pieces
 * 
 * - it is used in bt_swarm_resumedata_t to avoid redownloading already available data when resuming
 */
file_size_inval_t	bt_swarm_sched_t::get_partavail_piece_inval() const throw()
{
	file_size_inval_t				partavail_piece_inval;
	bt_swarm_sched_t::piece_db_t::const_iterator	iter;
	// go thru the whole piece_do_needreq_db
	for(iter = piece_do_needreq_db.begin(); iter != piece_do_needreq_db.end(); iter++ ){
		bt_swarm_sched_piece_t * sched_piece 	= iter->second;
		// update the partavail_piece_inval
		partavail_piece_inval += sched_piece->get_isavail_inval();
	}
	// go thru the whole piece_no_needreq_db
	for(iter = piece_no_needreq_db.begin(); iter != piece_no_needreq_db.end(); iter++ ){
		bt_swarm_sched_piece_t * sched_piece 	= iter->second;
		// update the partavail_piece_inval
		partavail_piece_inval += sched_piece->get_isavail_inval();
	}
	// return the just built partavail_piece_inval
	return partavail_piece_inval;
}

/** \brief Return true if the bt_swarm_sched_t is to be considered in 'endgame'
 * 
 * - it is used to change the timeout of the bt_swarm_sched_request_t at the 
 *   end of a download.
 *   - this avoid to wait for a request to timeout while other remote peer 
 *     may satisfy it faster.
 *   - it is always frustrating to tranfert the whole stuff at 1mbyte/s and 
 *     see it ending at 100byte/s
 */
bool	bt_swarm_sched_t::in_endgame()			const throw()
{
	bt_pselect_vapi_t *	pselect_vapi	= m_bt_swarm->pselect_vapi();
	// if the bt_pselect_vapi_t has no selectable piece, it is not in endgame
	if( pselect_vapi->has_selectable() )	return false;
	// if all previous tests passed, bt_swarm_sched_t is in endgame
	return true;	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief this function notify the bt_swarm_sched_t that the piece precedence have been changed
 */
void	bt_swarm_sched_t::notify_pieceprec_change()	throw()
{
	// delete all pending bt_swarm_sched_piece_t which are now in bt_pieceprec_t::NOTNEEDED
	remove_notneeded_pieceprec_sched_piece();

	// try to send requests on all idle connection (ecnx or full)
	// - this is in case those connections are idle because none of the
	//   data remotly available were selectable.
	// - as the pieceprec change may have made new piece selectable, some idle connection
	//   may be able to handle request for those new piece.
	try_send_request_on_idle_cnx();
}

/** \brief try to send requests on all idle connection (ecnx or full)
 */
void	bt_swarm_sched_t::try_send_request_on_idle_cnx()	throw()
{
	// forward it to the bt_swarm_sched_ecnx();
	sched_ecnx()->try_send_request_on_idle_cnx();
	// forward it to the bt_swarm_sched_full();
	sched_full()->try_send_request_on_idle_cnx();
}

/** \brief Delete all the bt_swarm_sched_piece_t which have a bt_pieceprec_t::NOTNEEDED
 * 
 * -# go thru piece_do_needreq_db and piece_no_needreq_db to get all sched_piece
 *    in bt_pieceprec_t::NOTNEEDED
 * -# copy them in a list sched_piece_todel_db
 *    - the copy is required as the sched_piece cant be deleted during the walking
 * -# delete all the sched_piece contained in sched_piece_todel_db
 */
void	bt_swarm_sched_t::remove_notneeded_pieceprec_sched_piece()	throw()
{
	bt_pselect_vapi_t *			pselect_vapi	= bt_swarm()->pselect_vapi();
	std::list<bt_swarm_sched_piece_t *>	sched_piece_todel_db;
	bt_swarm_stats_t &			swarm_stats	= bt_swarm()->swarm_stats();
	bt_swarm_sched_t::piece_db_t::iterator	iter;
	// log to debug	
	KLOG_DBG("enter");
	KLOG_DBG("piece_do_needreq_db.size()=" << piece_do_needreq_db.size() );
	KLOG_DBG("piece_no_needreq_db.size()=" << piece_no_needreq_db.size() );

	// go thru the whole piece_do_needreq_db
	for(iter = piece_do_needreq_db.begin(); iter != piece_do_needreq_db.end(); iter++ ){
		bt_swarm_sched_piece_t *sched_piece 	= iter->second;
		size_t			piece_idx	= sched_piece->pieceidx();
		// log to debug
		KLOG_DBG("do_needreq piece_idx=" << piece_idx);
		// if the bt_pieceprec_t of this piece_idx is still needed, keep it
		if( pselect_vapi->pieceprec(piece_idx).is_needed() )	continue;
		// put this sched_piece in the sched_piece_todel_db 
		sched_piece_todel_db.push_back(sched_piece);
	}

	// go thru the whole piece_no_needreq_db
	for(iter = piece_no_needreq_db.begin(); iter != piece_no_needreq_db.end(); iter++ ){
		bt_swarm_sched_piece_t *sched_piece 	= iter->second;
		size_t			piece_idx	= sched_piece->pieceidx();
		// log to debug
		KLOG_DBG("no_needreq piece_idx=" << piece_idx);
		// if bt_pieceprec_t of piece_idx is still needed, keep it
		if( pselect_vapi->pieceprec(piece_idx).is_needed() )	continue;
		// put this sched_piece in the sched_piece_todel_db 
		sched_piece_todel_db.push_back(sched_piece);
	}
	
	// log to debug
	KLOG_DBG(sched_piece_todel_db.size() << " sched_piece are NOTNEEDED. and  will be deleted");

	// delete all the bt_swarm_sched_piece_t present in the sched_piece_todel_db
	while( !sched_piece_todel_db.empty() ){
		bt_swarm_sched_piece_t *sched_piece 	= sched_piece_todel_db.front();
		// remove it from the sched_piece_todel_db
		sched_piece_todel_db.pop_front();
		// update the bt_swarm_stats_t
		swarm_stats.deleted_dload_datalen( swarm_stats.deleted_dload_datalen()
							+ sched_piece->partavail_len() );
		// log to debug
		KLOG_ERR("delete sched_piece " << sched_piece->pieceidx() );
		// delete the object itself
		nipmem_zdelete	sched_piece;
	}
}

/** \brief Return the expire_timeout for all bt_swarm_sched_request_t
 * 
 * - used when just entering in_endgame() to restart with a shorter delay
 */
void	bt_swarm_sched_t::restart_timeout_for_allreq(const delay_t &expire_delay)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// go thru the whole piece_no_needreq_db
	// - unless to get in the piece_do_needreq_db as all their requests are already timedout
	piece_db_t::iterator	iter;
	for(iter = piece_no_needreq_db.begin(); iter != piece_no_needreq_db.end(); iter++ ){
		bt_swarm_sched_piece_t *sched_piece 	= iter->second;
		// forward the restart_timeout_for_allreq to this sched_piece
		sched_piece->restart_timeout_for_allreq(expire_delay); 
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      piece selection
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on a bt_swarm_sched_piece_t available in this remote_pieceavail
 * 
 * - return a currently active piece (bt_swarm_sched_piece_t) if one is remotely available
 * - else create a new bt_swarm_sched_piece_t according to the bt_pselect_vapi_t
 * - if no remote piece needs to be downloaded, return a NULL
 * @param pidx_hint if possible, select this pieceidx. it is valid IIF not size_t max
 *                        in relation with next_pidx_hint
 */
bt_swarm_sched_piece_t * bt_swarm_sched_t::select_piece_to_request(bt_swarm_sched_cnx_vapi_t *sched_cnx
						,const bt_pieceavail_t &remote_pieceavail
						,size_t pieceidx_hint)		throw()
{
	bt_pselect_vapi_t *			pselect_vapi	= bt_swarm()->pselect_vapi();
	bt_swarm_sched_t::piece_db_t::iterator	iter;
	bt_swarm_sched_piece_t *		sched_piece;

	// honor the pieceidx_hint with the sched_piece in piece_do_needreq_db
	// - it is a while, even if it doesnt loop, because its contains many break;
	while( pieceidx_hint != std::numeric_limits<size_t>::max() ){
		// if the pieceidx_hint IS NOT remotely availble, leave the loop
		if( !remote_pieceavail.is_avail(pieceidx_hint))		break;
		// try to find pieceidx_hint in the piece_do_needreq_db
		iter	= piece_do_needreq_db.find(pieceidx_hint);
		// if pieceidx_hint IS NOT in piece_do_needreq_db, leave the loop
		if( iter == piece_do_needreq_db.end() )			break;
		// build an alias on the found sched_piece
		sched_piece 	= iter->second;
		// if sched_piece HAS NO block which may be requested thru this sched_cnx, leave the loop
		if( !sched_piece->may_request_block_thru(sched_cnx) )	break;
		// return this sched_piece
		return sched_piece;
	}

	// try to find a sched_piece in piece_do_needreq_db which is remotly available
	sched_piece = select_piece_in_piece_db(piece_do_needreq_db, remote_pieceavail, sched_cnx);
	if( sched_piece )	return sched_piece;

	// NOTE: if this point is reached, no pending sched_piece may be requested by this swarm_full
	
	// if profile.seq_pselect_ifequ is NOT set, ignore pieceidx_hint whatever its value
	if( !profile().seq_pselect_ifequ() )	pieceidx_hint = std::numeric_limits<size_t>::max();
	// find the piece_idx of the rarest piece which is available remotely and not locally
	size_t piece_idx = pselect_vapi->next_piece_to_select(remote_pieceavail, pieceidx_hint);

	// if none is found, return NULL
	if( piece_idx == std::numeric_limits<size_t>::max() )	return NULL;
	
	// create a bt_swarm_sched_piece_t for this piece_idx which has been selected
	sched_piece	= nipmem_new bt_swarm_sched_piece_t(this, piece_idx, file_size_inval_t());
	
	// if it is now in_endgame(), restart_timeout_for_allreq to the endgame timeout
	// - being now in_endgame() implies it just passed in_endgame() as a brand
	//   new bt_swarm_sched_piece_t just been created (as opposed to reusing
	//   a previously created)
	// - NOTE: it MUST be done after the bt_swarm_sched_piece_t creation as the
	//   bt_swarm_sched_piece_t ctor is the one actually 'selecting' the piece.
	//   - thus in_endgame() may be aware if some pieces remain to select
	if( in_endgame() )	restart_timeout_for_allreq(profile().request_timeout_endgame());

	// return the just created bt_swarm_sched_piece_t
	return sched_piece;
}

/** \brief Try to select a bt_swarm_sched_piece_t from a piece_db_t respecting bt_pieceprec_t
 */
bt_swarm_sched_piece_t * bt_swarm_sched_t::select_piece_in_piece_db(const piece_db_t &piece_db
				, const bt_pieceavail_t &remote_pieceavail
				, bt_swarm_sched_cnx_vapi_t *sched_cnx)		throw()
{
	bt_pselect_vapi_t *		pselect_vapi	= bt_swarm()->pselect_vapi();
	piece_db_t::const_iterator	iter;
	bt_pieceprec_t			bt_pieceprec;
	// try to find a sched_piece in piece_db which is remotly available
	std::multimap<bt_pieceprec_t, bt_swarm_sched_piece_t *>	cand_db;
	for(iter = piece_db.begin(); iter != piece_db.end(); iter++ ){
		bt_swarm_sched_piece_t * sched_piece 	= iter->second;
		// if this sched_piece IS NOT remotly available, skip it
		if( !remote_pieceavail.is_avail(sched_piece->pieceidx()) )	continue;
		// if this sched_piece has no block that needs request on this sched_cnx, skip it
		if( !sched_piece->may_request_block_thru(sched_cnx) )		continue;
		// get the bt_pieceprec_t for the pieceidx of this sched_piece
		bt_pieceprec	= pselect_vapi->pieceprec(sched_piece->pieceidx());
		// put this sched_piece in the cand_db
		cand_db.insert( std::make_pair(bt_pieceprec, sched_piece) );
	}
	// if there are no candidate, return NULL
	if( cand_db.empty() )	return NULL;
	// return the bt_swarm_sched_piece_t with the highest bt_pieceprec_t
	return cand_db.rbegin()->second;
} 

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    piece_db management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief DoLink a do_needreq bt_swarm_sched_piece_t
 */
void	bt_swarm_sched_t::piece_do_needreq_dolink(bt_swarm_sched_piece_t *sched_piece) 	throw()
{
	// link the bt_swarm_sched_piece_t
	bool	succeed	= piece_do_needreq_db.insert(std::make_pair(sched_piece->pieceidx(),sched_piece)).second;
	DBG_ASSERT( succeed );
}

/** \brief UnLink a do_needreq bt_swarm_sched_piece_t
 */
void 	bt_swarm_sched_t::piece_do_needreq_unlink(bt_swarm_sched_piece_t *sched_piece)	throw()
{
	// sanity check - the sched_piece MUST be in the piece_do_needreq_db
	DBG_ASSERT( piece_do_needreq_db.find(sched_piece->pieceidx()) != piece_do_needreq_db.end() );	
	// unlink the bt_swarm_sched_piece_t	
	piece_do_needreq_db.erase(sched_piece->pieceidx());
}

/** \brief DoLink a no_needreq bt_swarm_sched_piece_t
 */
void	bt_swarm_sched_t::piece_no_needreq_dolink(bt_swarm_sched_piece_t *sched_piece) 	throw()
{
	// link the bt_swarm_sched_piece_t
	bool	succeed	= piece_no_needreq_db.insert(std::make_pair(sched_piece->pieceidx(),sched_piece)).second;
	DBG_ASSERT( succeed );
}

/** \brief UnLink a no_needreq bt_swarm_sched_piece_t
 */
void 	bt_swarm_sched_t::piece_no_needreq_unlink(bt_swarm_sched_piece_t *sched_piece)	throw()
{
	// sanity check - the sched_piece MUST be in the piece_no_needreq_db
	DBG_ASSERT( piece_no_needreq_db.find(sched_piece->pieceidx()) != piece_no_needreq_db.end() );	
	// unlink the bt_swarm_sched_piece_t	
	piece_no_needreq_db.erase(sched_piece->pieceidx());
}


/** \brief Return a pointer on a bt_swarm_shced_piece_t matching the piece_idx, or NULL if none is found
 */
bt_swarm_sched_piece_t *bt_swarm_sched_t::piece_by_idx(size_t piece_idx)	const throw()
{
	piece_db_t::const_iterator	iter;

	// try to find this piece_idx in the piece_do_needreq_db
	iter	= piece_do_needreq_db.find(piece_idx);
	if( iter != piece_do_needreq_db.end() )	return iter->second;

	// try to find this piece_idx in the piece_no_needreq_db
	iter	= piece_no_needreq_db.find(piece_idx);
	if( iter != piece_no_needreq_db.end() )	return iter->second;
	
	// if this point is reached, no match has been found and return NULL
	return NULL;
}

NEOIP_NAMESPACE_END





