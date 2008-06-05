/*! \file
    \brief Class to handle the bt_swarm_sched_piece_t

\par State description
- STATE_DO_NEEDREQ: a bt_swarm_sched_piece_t is do_needreq
  - it means it contains one or more bt_swarm_sched_block_t in do_needreq mode.
  - aka a piece is partially requested if it has block which needs to be more requested
- STATE_NO_NEEDREQ: a bt_swarm_sched_piece_t is no_needreq means:
  - it contains no bt_swarm_sched_block_t which are in do_needreq mode
  - note: a piece in checking via bt_piece_cpuhash_t is no_needreq as all its blocks are local

\par About range state
For any given byte, it is either:
- in isavail_db if it is already available
- in nevereq_db if no bt_swarm_sched_block_t have been created for it
- in a bt_swarm_sched_block_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_sched_piece.hpp"
#include "neoip_bt_swarm_sched_block.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_piece_cpuhash.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_bt_prange.hpp"
#include "neoip_math_shortcut.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_bt_swarm_helper.hpp"	// TODO TAG PIGNORE_RACE

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_swarm_sched_piece_t::bt_swarm_sched_piece_t(bt_swarm_sched_t *m_swarm_sched
			, size_t m_pieceidx, const file_size_inval_t &isavail_inval)	throw()
{
	bt_swarm_t *		bt_swarm	= m_swarm_sched->bt_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	bt_pselect_vapi_t *	pselect_vapi	= bt_swarm->pselect_vapi();
	// sanity check - this piece_idx MUST be selectable by the bt_pselect_vapi_t
	DBG_ASSERT( pselect_vapi->may_select(m_pieceidx) );
	// sanity check - this piece_idx precedence MUST be needed
	DBG_ASSERT( pselect_vapi->pieceprec(m_pieceidx).is_needed() );
	// sanity check - this piece_idx MUST NOT be locally available
	DBG_ASSERT( bt_swarm->local_pavail().is_avail(m_pieceidx) == false );

	// copy the parameter
	this->m_swarm_sched	= m_swarm_sched;
	this->m_pieceidx	= m_pieceidx;
	this->isavail_inval	= isavail_inval;
	// zero some field
	piece_cpuhash		= NULL;
	// mark this piece_idx as currently downloading in bt_pselect_vapi
	pselect_vapi->enter_dling(pieceidx()); 

	// build the nevereq_inval
	totfile_range		= bt_unit_t::pieceidx_to_totfile_range(pieceidx(), bt_mfile);
	nevereq_inval		+= totfile_range.to_inval_item();
	nevereq_inval		-= isavail_inval;
	// sanity check - the isavail_inval and the nevereq_inval MUST ALWAYS be distinct
	DBG_ASSERT( isavail_inval.is_distinct(nevereq_inval));

	// if the piece is already fully downloaded, trigger a post processing now	
	// - it fixes a race condition when bt_swarm_resumedata_t is saved during a 
	//   piece checking.
	// - if isavail_inval has been generated during a piece checking, it is fully available
	//   but not yet checked. So here launch the missing fully_dloaded_post_process
	if( is_fully_dloaded() )	fully_dloaded_post_process();

	// set the state_cur to STATE_NONE to trigger the registration in the new state_t
	state_cur	= STATE_NONE;
	state_goto( STATE_DO_NEEDREQ );
}

/** \brief Destructor
 */
bt_swarm_sched_piece_t::~bt_swarm_sched_piece_t()		throw()
{
	bt_pselect_vapi_t *pselect_vapi	= swarm_sched()->bt_swarm()->pselect_vapi();
	// mark this piece_idx as nomore downloading in bt_pselect_vapi
	pselect_vapi->leave_dling(pieceidx());

	// delete the block_do_needreq_db
	while( !block_do_needreq_db.empty() )	nipmem_delete	block_do_needreq_db.begin()->second;
	// delete the block_no_needreq_db 
	while( !block_no_needreq_db.empty() )	nipmem_delete	block_no_needreq_db.begin()->second;

	// go back to STATE_NONE
	state_goto( STATE_NONE );

	// delete bt_piece_cpuhash_t if needed
	nipmem_zdelete	piece_cpuhash;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the bt_swarm_sched_block_t for this totfile_beg, NULL if none matches
 */
bt_swarm_sched_block_t *bt_swarm_sched_piece_t::block_by_totfile_beg(const file_size_t &totfile_beg)	const throw()
{
	block_db_t::const_iterator	iter;

	// try to find the offset in the block_no_needreq_db
	// - NOTE: test the block_no_needreq_db first as it is commonly found in it
	iter	= block_no_needreq_db.find(totfile_beg);
	// if it has been found, return the bt_swarm_sched_block_t pointer
	if( iter != block_no_needreq_db.end() )	return iter->second;

	// try to find the offset in the block_do_needreq_db
	iter	= block_do_needreq_db.find(totfile_beg);
	// if it has been found, return the bt_swarm_sched_block_t pointer
	if( iter != block_do_needreq_db.end() )	return iter->second;

	// if this point is reached, no bt_swarm_sched_block_t matches this offset, return NULL
	return NULL;
}

/** \brief Return a pointer on a bt_swarm_sched_block_t to request on this sched_cnx 
 *         among block_do_needreq.
 * 
 * - it returns the sched_block which is not requested by sched_cnx and have 
 *   the least amount of request.
 *   - it aims to spread the number of request accross the blocks
 */
bt_swarm_sched_block_t *bt_swarm_sched_piece_t::get_block_do_needreq(bt_swarm_sched_cnx_vapi_t *sched_cnx)
										const throw()
{
	block_db_t::const_iterator	iter;
	// go thru block_do_needreq_db to find the first block which is not requested by sched_cnx
	std::multimap<size_t, bt_swarm_sched_block_t *>	cand_db;
	for(iter = block_do_needreq_db.begin(); iter != block_do_needreq_db.end(); iter++){
		bt_swarm_sched_block_t * sched_block	= iter->second;
		// if sched_block is already requested by this sched_cnx, goto the next one
		if( sched_block->is_reqed_by(sched_cnx) )	continue;
		// put this sched_block in the cand_db
		cand_db.insert( std::make_pair(sched_block->nb_request(), sched_block) );
	}
	// if there are no candidate, return NULL
	if( cand_db.empty() )	return NULL;
	// return the bt_swarm_sched_block_t with the least amount of bt_swarm_sched_request_t
	return cand_db.begin()->second;
}

/** \brief Return true if this sched_cnx may requests some block on this sched_piece
 */
bool	bt_swarm_sched_piece_t::may_request_block_thru(bt_swarm_sched_cnx_vapi_t *sched_cnx)	const throw()
{
	// if there is already a sched_block in block_do_needreq_db which may be requested, return true
	if( get_block_do_needreq(sched_cnx) )	return true;
	// if thee nevereq_inval is not empty, new block will be created, so return true
	if( !nevereq_inval.empty() )		return true;
	// else return false;
	return false;
}

/** \brief Return a pointer on a bt_swarm_sched_block_t to request on this sched_cnx, or NULL if none
 * 
 * Brief Description of the algo
 * -# if an existing sched_block in block_do_needreq_db matches, return it
 * -# if there are no more room to create a new one, return NULL
 * -# else create a new one
 */
bt_swarm_sched_block_t *bt_swarm_sched_piece_t::get_block_to_request(bt_swarm_sched_cnx_vapi_t *sched_cnx)
										throw()
{
	bt_swarm_t *			bt_swarm	= swarm_sched()->bt_swarm();
	const bt_swarm_profile_t &	swarm_profile	= bt_swarm->profile();	
	bt_swarm_sched_block_t * 	sched_block;
	
	// if a sched_block among block_db_needreq which may be requested by sched_cnx, return it
	sched_block	= get_block_do_needreq(sched_cnx);
	if( sched_block )	return sched_block;

	// if all the bt_swarm_sched_block_t have been requested, return NULL
	if( nevereq_inval.empty() )		return NULL;
	
	// create a bt_swarm_sched_block_t from the first item of nevereq_inval
	file_range_t	block_range	= file_range_t::from_inval_item(nevereq_inval[0]);
	// if the block_range is larger then the swarm_profile.xmit_req_maxlen(), clamp its end
	if( block_range.length() > swarm_profile.xmit_req_maxlen() )
		block_range.end	( block_range.beg() + swarm_profile.xmit_req_maxlen() - 1);
	// remove the block_range from the nevereq_inval
	nevereq_inval	-= block_range.to_inval_item();
	// sanity check - the isavail_inval and the nevereq_inval MUST ALWAYS be distinct
	DBG_ASSERT( isavail_inval.is_distinct(nevereq_inval) );
	// return a bt_swarm_sched_block_t for this range
	// - it links itself to the bt_swarm_sched_piece_t
	return nipmem_new bt_swarm_sched_block_t(this, block_range);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        state management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Compute the current state based on the current block_do_needreq_db and block_no_needreq_db
 */
bt_swarm_sched_piece_t::state_t	bt_swarm_sched_piece_t::state_cpu()		const throw()
{
	// a piece is considered STATE_DO_NEEDREQ if it contains block which need request themself
	if( !block_do_needreq_db.empty() )	return STATE_DO_NEEDREQ;
	// a piece is considered STATE_DO_NEEDREQ if the unreqeq_inval is NOT empty
	if( !nevereq_inval.empty() 	)	return STATE_DO_NEEDREQ;
	// otherwise it is considered in STATE_NO_NEEDREQ
	return STATE_NO_NEEDREQ;
}

/** \brief Compute the current state based on the current block_do_needreq_db and block_no_needreq_db
 */
void	bt_swarm_sched_piece_t::state_change_if_needed()			throw()
{
	// compute what the current state should be
	state_t	state_should	= state_cpu();
	// if state_cur is different that state_should, goto the state_should
	if( state_cur != state_should )	state_goto( state_should );
}

/** \brief Change the current state
 */
void	bt_swarm_sched_piece_t::state_goto(state_t state_new)			throw()
{
	// undo the state_cur
	switch( state_cur ){
	case STATE_DO_NEEDREQ:	swarm_sched()->piece_do_needreq_unlink(this);	break;
	case STATE_NO_NEEDREQ:	swarm_sched()->piece_no_needreq_unlink(this);	break;
	case STATE_NONE:	break;
	default:		DBG_ASSERT( 0 );
	}
	// set the state_cur to the state_new
	state_cur	= state_new;
	// do the state_new
	switch( state_cur ){
	case STATE_DO_NEEDREQ:	swarm_sched()->piece_do_needreq_dolink(this);	break;
	case STATE_NO_NEEDREQ:	swarm_sched()->piece_no_needreq_dolink(this);	break;
	case STATE_NONE:	break;
	default:		DBG_ASSERT( 0 );
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        request_db_t handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief DoLink a do_needreq bt_swarm_sched_block_t to this bt_swarm_sched_piece_t
 * 
 * - it is made more complex as it is needed to ensure to keep this piece in the proper
 *   state and list do_needreq/no_needreq
 */
void	bt_swarm_sched_piece_t::block_do_needreq_dolink(bt_swarm_sched_block_t *sched_block) 	throw()
{
	// link the bt_swarm_sched_block_t
	bool	succeed	= block_do_needreq_db.insert(std::make_pair(sched_block->totfile_beg(),sched_block)).second;
	DBG_ASSERT( succeed );	
	// change the state_cur in case this made the current state changes
	state_change_if_needed();
}

/** \brief UnLink a do_needreq bt_swarm_sched_block_t from this bt_swarm_sched_piece_t
 * 
 * - it is made more complex as it is needed to ensure to keep this piece in the proper
 *   state and list do_needreq/no_needreq
 */
void 	bt_swarm_sched_piece_t::block_do_needreq_unlink(bt_swarm_sched_block_t *sched_block)	throw()
{
	// sanity check - the sched_block MUST be in the block_do_needreq_db
	DBG_ASSERT( block_do_needreq_db.find(sched_block->totfile_beg()) != block_do_needreq_db.end() );	
	// unlink the bt_swarm_sched_block_t	
	block_do_needreq_db.erase(sched_block->totfile_beg());
	// change the state_cur in case this made the current state changes
	state_change_if_needed();
}

/** \brief DoLink a no_needreq bt_swarm_sched_block_t to this bt_swarm_sched_piece_t
 */
void	bt_swarm_sched_piece_t::block_no_needreq_dolink(bt_swarm_sched_block_t *sched_block) 	throw()
{
	// link the bt_swarm_sched_block_t
	bool	succeed	= block_no_needreq_db.insert(std::make_pair(sched_block->totfile_beg(),sched_block)).second;
	DBG_ASSERT( succeed );
	// change the state_cur in case this made the current state changes
	state_change_if_needed();
}

/** \brief UnLink a no_needreq bt_swarm_sched_block_t from this bt_swarm_sched_piece_t
 */
void 	bt_swarm_sched_piece_t::block_no_needreq_unlink(bt_swarm_sched_block_t *sched_block)	throw()
{
	// sanity check - the sched_block MUST be in the block_no_needreq_db
	DBG_ASSERT( block_no_needreq_db.find(sched_block->totfile_beg()) != block_no_needreq_db.end() );	
	// unlink the bt_swarm_sched_block_t	
	block_no_needreq_db.erase(sched_block->totfile_beg());
	// change the state_cur in case this made the current state changes
	state_change_if_needed();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the piece is fully downloaded
 * 
 * - NOTE: it does not imply the piece hash have been checked
 */
bool	bt_swarm_sched_piece_t::is_fully_dloaded()	const throw()
{
	// if there are block which do need request, return false
	if( !block_do_needreq_db.empty() )	return false;
	// if there are block which dont need request, return false
	if( !block_no_needreq_db.empty() )	return false;
	// if nevereq_inval is not empty, return false
	if( !nevereq_inval.empty() )		return false;
	// if this point is reached, the bt_swarm_sched_piece_t is fully downloaded
	return true;
}

/** \brief Perform the post process when a piece is fully downloaded
 * 
 * - aka launch a bt_piece_cpuhash_t if bt_mfile.do_piecehash()
 *   else launch nohash_zerotimer
 */
void	bt_swarm_sched_piece_t::fully_dloaded_post_process()			throw()
{
	bt_swarm_t *		bt_swarm	= swarm_sched()->bt_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();	
	// sanity check - the piece MUST fully downloaded
	DBG_ASSERT( is_fully_dloaded() );
	// if bt_mfile.do_piecehash() is false, the piece is declared available without checking
	// - it is done in a zerotimer_t avoid any nested notification issue
	if( bt_mfile.do_piecehash() == false ){
		nohash_zerotimer.append(this, NULL);
		return;
	}
	// sanity check - no piece_cpuhash MUST be running
	DBG_ASSERT( !piece_cpuhash );
	// launch the bt_piece_cpuhash_t to check its content
	// - it start immediatly without start() function
	bt_io_vapi_t *	bt_io_vapi	= bt_swarm->io_vapi();
	piece_cpuhash	= nipmem_new bt_piece_cpuhash_t(pieceidx(), bt_mfile, bt_io_vapi, this, NULL);
}


/** \brief notify that a given bt_swarm_sched_block_t is completed
 * 
 * - NOTE: the bt_swarm_sched_piece_t and the bt_swarm_sched_block_t are NEVER deleted
 *   during this function.
 */
void	bt_swarm_sched_piece_t::notify_block_completion(bt_swarm_sched_block_t *sched_block)	throw()
{
	bt_swarm_t *	bt_swarm	= swarm_sched()->bt_swarm();
	// sanity check - this piece_idx precedence MUST NOT bt_pieceprec_t::NOTNEEDED
	DBG_ASSERT( bt_swarm->pselect_vapi()->pieceprec(pieceidx()).is_needed() );
	// remove the data made available by this sched_block completion from isavail_inval
	isavail_inval	+= sched_block->totfile_range().to_inval_item();
	// sanity check - the isavail_inval and the nevereq_inval MUST ALWAYS be distinct
	DBG_ASSERT( isavail_inval.is_distinct(nevereq_inval) );

	// if the bt_swarm_sched_piece_t is not yet fully downloaded, return now
	if( !is_fully_dloaded() )	return;
	
	// do the post processing
	fully_dloaded_post_process();
}

/** \brief Return the expire_timeout for all bt_swarm_sched_request_t
 * 
 * - used when just entering in_endgame() to restart with a shorter delay
 */
void	bt_swarm_sched_piece_t::restart_timeout_for_allreq(const delay_t &expire_delay)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// go thru the whole block_no_needreq_db
	// - unless to get in the block_do_needreq_db as all their requests are already timedout
	block_db_t::iterator	iter;
	for(iter = block_no_needreq_db.begin(); iter != block_no_needreq_db.end(); iter++ ){
		bt_swarm_sched_block_t *sched_block 	= iter->second;
		// forward the restart_timeout_for_allreq to this sched_block
		sched_block->restart_timeout_for_allreq(expire_delay); 
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the nohash_zerotimer expire
 * 
 * - only used to avoid any nested notification issue
 */
bool	bt_swarm_sched_piece_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	bt_swarm_t *	bt_swarm	= swarm_sched()->bt_swarm();
	size_t		pieceidx_copy	= this->pieceidx();
	// sanity check - the bt_swarm->get_mfile().do_piecehash() MUST be false
	DBG_ASSERT( !bt_swarm->get_mfile().do_piecehash() );
	// autodelete
	// - NOTE: this is done before the notification thus this piece is no more 
	//   considered pending during the notification
	nipmem_delete	this;	
	// declare this piece available
	bool	tokeep	= bt_swarm->declare_piece_newly_avail(pieceidx_copy);
	if( !tokeep )	return false;
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        bt_piece_cpuhash_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
 */
bool	bt_swarm_sched_piece_t::neoip_bt_piece_cpuhash_cb(void *cb_userptr, bt_piece_cpuhash_t &cb_piece_cpuhash
				, const bt_err_t &cpuhash_err, const bt_id_t &piecehash)	throw()
{
	bt_swarm_t *		bt_swarm	= swarm_sched()->bt_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	size_t			pieceidx_copy	= this->pieceidx();	
	// log to debug
	KLOG_DBG("enter piece_cpuhash_err=" << cpuhash_err << " piecehash=" << piecehash);

	// if the bt_piece_cpuhash_t failed, notify a bt_swarm_event_t::DISK_ERROR
	if( cpuhash_err.failed() )	return bt_swarm->notify_disk_error(cpuhash_err);

	// delete the bt_piece_cpuhash_t
	nipmem_zdelete	piece_cpuhash;

	// autodelete
	// - NOTE: this is done before the notification thus this piece is no more 
	//   considered pending during the notification
	nipmem_delete	this;
	
	// if the piecehash is equal to its value in bt_mfile, mark the block as locally available
	if( bt_mfile.piecehash_arr()[pieceidx_copy] == piecehash ){
		// log to debug
		KLOG_DBG("hash of piece " << pieceidx_copy << " OK!");
		// notify the piece as newly available
		// - NOTE: this function may delete the bt_swarm_t
		bool	tokeep = bt_swarm->declare_piece_newly_avail(pieceidx_copy);
		if( !tokeep )	return false;
	}else{
		// update the bt_swarm_stats_t
		bt_swarm_stats_t &	swarm_stats	= bt_swarm->swarm_stats();
		swarm_stats.nb_hash_failed( swarm_stats.nb_hash_failed() + 1 );
		// log the event
		KLOG_INFO("hash of piece " << pieceidx()
				<< " is supposed to be " << bt_mfile.piecehash_arr()[pieceidx()]
				<< " but has be computed as " << piecehash);
	}

	// return dontkeep - as it has just been deleted
	return false;
}

NEOIP_NAMESPACE_END





