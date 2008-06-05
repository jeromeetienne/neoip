/*! \file
    \brief Class to handle the bt_swarm_sched_request_t

\par Note about state
- a bt_swarm_sched_request_t is pending, if the request has not yet expired
- after that it is considered expired.

- TODO in fact the naming of the state and timeout are wrong
  - the expire_timeout should be maydup_timeout
  - no pending
  - only is_maydup or not

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_sched_request.hpp"
#include "neoip_bt_swarm_sched_block.hpp"
#include "neoip_bt_swarm_sched_piece.hpp"
#include "neoip_bt_swarm_sched_cnx_vapi.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm_profile.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_cmd.hpp"
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
bt_swarm_sched_request_t::bt_swarm_sched_request_t(bt_swarm_sched_cnx_vapi_t *sched_cnx
			, bt_swarm_sched_block_t* sched_block, const delay_t &expire_delay)	throw()
{
	// copy the parameter
	this->sched_cnx		= sched_cnx;
	this->sched_block	= sched_block;	
	// start the expire_timeout
	expire_timeout.start(expire_delay, this, NULL);

	// update the bt_swarm_stats_t
	bt_swarm_t *	bt_swarm	= sched_block->sched_piece->swarm_sched()->bt_swarm();
	bt_swarm_stats_t&swarm_stats	= bt_swarm->swarm_stats();
	swarm_stats.total_req_nb( swarm_stats.total_req_nb() + 1 );
	swarm_stats.total_req_len( swarm_stats.total_req_len() + sched_block->length() );
	// if requests already exists for this sched_block, this sched_request is a duplicate, update the bt_swarm_stats_t
	if( sched_block->nb_request() ){
		swarm_stats.dup_req_nb	( swarm_stats.dup_req_nb() + 1 );
		swarm_stats.dup_req_len	( swarm_stats.dup_req_len() + sched_block->length() );
	}

	// link it to the sched_block - it always start as pending
	sched_block->pending_req_dolink(this);
	// link it to the bt_swarm_sched_cnx_vapi_t and start the request command
	sched_cnx->sched_req_dolink_start(this);
}

/** \brief Destructor
 */
bt_swarm_sched_request_t::~bt_swarm_sched_request_t()		throw()
{
	// unlink it from the sched_block
	if( is_pending() )	sched_block->pending_req_unlink(this);
	else			sched_block->expired_req_unlink(this);
	// unlink it from the bt_swarm_sched_cnx_vapi_t
	sched_cnx->sched_req_unlink(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Return the piece_idx of this request
 */
size_t	bt_swarm_sched_request_t::piece_idx()	const throw()
{
	bt_swarm_sched_piece_t *sched_piece	= sched_block->sched_piece;
	// return the piece_idx
	return sched_piece->pieceidx();
}

/** \brief Return the totfile_range requested by this bt_swarm_sched_request_t
 */
const file_range_t &	bt_swarm_sched_request_t::totfile_range()		const throw()
{
	return sched_block->totfile_range();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify that this request have been successfully replied and delete the request
 * 
 * - if the reply is NOT from the same bt_swarm_sched_cnx_vapi_t, send a bt_cmdtype_t::BLOCK_DEL 
 * - delete the object
 */
void	bt_swarm_sched_request_t::notify_complete_from(bt_swarm_sched_cnx_vapi_t *from_sched_cnx)	throw()
{
	// cancel the request IIF the reply is NOT from the same bt_swarm_sched_cnx_vapi_t
	if( sched_cnx != from_sched_cnx )	sched_cnx->sched_req_cancel(this);
	// delete this object
	nipmem_delete	this;
}

/** \brief Restart the timeout if it is more than the max_expire_delay
 */
void	bt_swarm_sched_request_t::restart_timeout(const delay_t &max_expire_delay)	throw()
{
	// if the expire_timeout is not running, do nothing and return now
	if( !expire_timeout.is_running() )				return;
	// if the expire_timeout expire_delay is <= than max_expire_delay, do nothing
	if( expire_timeout.get_expire_delay() <= max_expire_delay )	return;
	// restart the expire_timeout with max_expire_delay
	expire_timeout.start(max_expire_delay, this, NULL);
	
	// NOTE: no handling of the list expired_req/pending_req on purpose
	// - because the expire_timeout is restarted IIF it is already running
	// - thus it is changed IIF it was in the pending_req and it remain
	//   in this list as the expire_timeout is always running
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool 	bt_swarm_sched_request_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// sanity check - the request MUST pending at this point
	DBG_ASSERT( is_pending() );
	// stop the expire_timeout
	expire_timeout.stop();
	// unlink it from the bt_swarm_sched_block_t->pending_req_db
	sched_block->pending_req_unlink(this);
	// dolink it in the bt_swarm_sched_block_t->expired_req_db
	sched_block->expired_req_dolink(this);
	// update the bt_swarm_stats_t
	bt_swarm_t *	bt_swarm	= sched_block->sched_piece->swarm_sched()->bt_swarm();
	bt_swarm_stats_t&swarm_stats	= bt_swarm->swarm_stats();
	swarm_stats.timedout_req_nb( swarm_stats.timedout_req_nb() + 1 );	
	// autodelete
	return true;
}


NEOIP_NAMESPACE_END





