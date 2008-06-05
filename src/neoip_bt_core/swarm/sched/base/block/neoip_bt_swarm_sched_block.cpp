/*! \file
    \brief Class to handle the bt_swarm_sched_block_t

\par State description
- STATE_DO_NEEDREQ: a bt_swarm_sched_block_t is do_needreq means
  - it contains it doesnt contains any bt_swarm_sched_request_t in pending
  - it may contains zero or more expired request
  - aka a block is to be requested, if it needs more requests (i) because it never
    have been or (ii) because all the current requests have expired.
- STATE_NO_NEEDREQ: a bt_swarm_sched_block_t is no_needreq means
  - either it contains bt_swarm_sched_request in pending.
  - or its content have been delivered and is currently written on disk (bt_io_write not null)

- TODO this directly implies that a single pending request is on a given bt_swarm_schedblock_t
  - it is a tight link between the arch and the policy... aka something bad
  - should i remove this link ? to allow what ?

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_sched_block.hpp"
#include "neoip_bt_swarm_sched_piece.hpp"
#include "neoip_bt_swarm_sched_request.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm_profile.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_io_write.hpp"
#include "neoip_bt_io_vapi.hpp"
#include "neoip_file_range.hpp"
#include "neoip_pkt.hpp"
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
bt_swarm_sched_block_t::bt_swarm_sched_block_t(bt_swarm_sched_piece_t *sched_piece
					, const file_range_t &m_totfile_range)	throw()
{
	// zero some field
	bt_io_write		= NULL;	
	// copy the parameter
	this->sched_piece	= sched_piece;
	this->m_totfile_range	= m_totfile_range;
	// set the state_cur to STATE_NONE to trigger the registration in the new state_t
	state_cur	= STATE_NONE;
	state_goto( STATE_DO_NEEDREQ );
}

/** \brief Destructor
 */
bt_swarm_sched_block_t::~bt_swarm_sched_block_t()		throw()
{
	// delete all the pending bt_swarm_sched_request_t if needed
	while( !pending_req_db.empty() )	nipmem_delete	*pending_req_db.begin();	
	while( !expired_req_db.empty() )	nipmem_delete	*expired_req_db.begin();	
	// delete the bt_io_write_t if needed
	nipmem_zdelete	bt_io_write;
	// go back to STATE_NONE
	state_goto( STATE_NONE );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        state management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Compute the current state based on the current block_do_needreq_db and block_no_needreq_db
 */
bt_swarm_sched_block_t::state_t	bt_swarm_sched_block_t::state_cpu()		const throw()
{
	// if the block is writting the data, it is considered in STATE_NO_NEEDREQ
	if( bt_io_write )		return STATE_NO_NEEDREQ;
	// if no pending request exist, it is considered in STATE_NO_NEEDREQ
	if( !pending_req_db.empty() )	return STATE_NO_NEEDREQ;
	// otherwise it is considered in STATE_DO_NEEDREQ
	return STATE_DO_NEEDREQ;
}

/** \brief Compute the current state based on the current block_do_needreq_db and block_no_needreq_db
 */
void	bt_swarm_sched_block_t::state_change_if_needed()			throw()
{
	// compute what the current state should be
	state_t	state_should	= state_cpu();
	// if state_cur is different that state_should, goto the state_should
	if( state_cur != state_should )	state_goto( state_should );
}

/** \brief Change the current state
 */
void	bt_swarm_sched_block_t::state_goto(state_t state_new)			throw()
{
	// undo the state_cur
	switch( state_cur ){
	case STATE_DO_NEEDREQ:	sched_piece->block_do_needreq_unlink(this);	break;
	case STATE_NO_NEEDREQ:	sched_piece->block_no_needreq_unlink(this);	break;
	case STATE_NONE:	break;
	default:		DBG_ASSERT( 0 );
	}
	// set the state_cur to the state_new
	state_cur	= state_new;
	// do the state_new
	switch( state_cur ){
	case STATE_DO_NEEDREQ:	sched_piece->block_do_needreq_dolink(this);	break;
	case STATE_NO_NEEDREQ:	sched_piece->block_no_needreq_dolink(this);	break;
	case STATE_NONE:	break;
	default:		DBG_ASSERT( 0 );
	}
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        request_db_t handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief DoLink a pending bt_swarm_sched_request_t to this bt_swarm_sched_block_t
 * 
 * - additionnaly, it update how this sched_block is linked in the sched_piece
 */
void	bt_swarm_sched_block_t::pending_req_dolink(bt_swarm_sched_request_t *sched_request) 	throw()
{
	// link the sched_request in the pending_req_db
	bool	succeed	= pending_req_db.insert(sched_request).second;
	DBG_ASSERT( succeed );
	// change the state_cur in case this made the current state changes
	state_change_if_needed();
}

/** \brief UnLink a pending bt_swarm_sched_request_t from this bt_swarm_sched_block_t
 * 
 * - additionnaly, it update how this sched_block is linked in the sched_piece
 */
void 	bt_swarm_sched_block_t::pending_req_unlink(bt_swarm_sched_request_t *sched_request)	throw()
{
	// sanity check - the sched_request MUST be in the pending_req_db
	DBG_ASSERT( pending_req_db.find(sched_request) != pending_req_db.end() );
	// unlink the bt_swarm_sched_request_t
	pending_req_db.erase(sched_request);
	// change the state_cur in case this made the current state changes
	state_change_if_needed();
}

/** \brief DoLink a expired bt_swarm_sched_request_t to this bt_swarm_sched_block_t
 */
void	bt_swarm_sched_block_t::expired_req_dolink(bt_swarm_sched_request_t *sched_request) 	throw()
{
	// link the bt_swarm_sched_request_t
	bool	succeed	= expired_req_db.insert(sched_request).second;
	DBG_ASSERT( succeed );
	// change the state_cur in case this made the current state changes
	state_change_if_needed();
}

/** \brief UnLink a expired bt_swarm_sched_request_t from this bt_swarm_sched_block_t
 */
void 	bt_swarm_sched_block_t::expired_req_unlink(bt_swarm_sched_request_t *sched_request)	throw()
{
	// sanity check - the sched_request MUST be in the expired_req_db
	DBG_ASSERT( pending_req_db.find(sched_request) != expired_req_db.end() );
	// unlink the bt_swarm_sched_request_t	
	expired_req_db.erase(sched_request);
	// change the state_cur in case this made the current state changes
	state_change_if_needed();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if this bt_swarm_sched_block_t is requested by this sched_cnx
 */
bool	bt_swarm_sched_block_t::is_reqed_by(bt_swarm_sched_cnx_vapi_t *sched_cnx)	const throw()
{
	request_db_t::const_iterator	iter;
	// log to debug
	KLOG_WARN("enter sched_cnx=" << sched_cnx);
	// go thru the whole pending_req_db to test if one is on sched_cnx
	for(iter = pending_req_db.begin(); iter != pending_req_db.end(); iter++){
		bt_swarm_sched_request_t * sched_req	= *iter;
		// if this sched_request is on sched_cnx, return true;
		if( sched_req->get_sched_cnx() == sched_cnx )	return true;
	}
	// go thru the whole expired_req_db to test if one is on sched_cnx
	for(iter = expired_req_db.begin(); iter != expired_req_db.end(); iter++){
		bt_swarm_sched_request_t * sched_req	= *iter;
		// if this sched_request is on sched_cnx, return true;
		if( sched_req->get_sched_cnx() == sched_cnx )	return true;
	}
	// if this point is reached, this sched_block is not requested on this sched_cnx, so return false
	return false;	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Function called when the BLOCK_REP have been received from this bt_swarm_sched_cnx_vapi_t
 */
void	bt_swarm_sched_block_t::notify_complete_from(bt_swarm_sched_cnx_vapi_t *sched_cnx
							, datum_t &reply_datum)	throw()
{
	bt_swarm_t *	bt_swarm	= sched_piece->swarm_sched()->bt_swarm();
	// sanity check - the reply_pkt->size() MUST BE equal to the length
	DBG_ASSERT( reply_datum.size() == length().to_size_t() );
	// delete all the pending sched_request for this sched_block
	while( !pending_req_db.empty() ){
		bt_swarm_sched_request_t *	sched_request	= *pending_req_db.begin();
		sched_request->notify_complete_from(sched_cnx);
	}
	// delete all the expired sched_request for this sched_block
	while( !expired_req_db.empty() ){
		bt_swarm_sched_request_t *	sched_request	= *expired_req_db.begin();
		sched_request->notify_complete_from(sched_cnx);
	}
	// launch the bt_io_write_t for it
	bt_io_write	= bt_swarm->io_vapi()->write_ctor(totfile_range(), reply_datum, this, NULL);

	// change the state_cur in case this made the current state changes
	// NOTE: it MUST be done after the bt_io_write_t as it now STATE_NO_NEEDREQ
	state_change_if_needed();
}

/** \brief Return the expire_timeout for all bt_swarm_sched_request_t
 * 
 * - used when just entering in_endgame() to restart with a shorter delay
 */
void	bt_swarm_sched_block_t::restart_timeout_for_allreq(const delay_t &expire_delay)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// go thru the whole pending_req_db
	// - unless to get in the expire_req_db as all their requests are already timedout
	request_db_t::iterator	iter;
	for(iter = pending_req_db.begin(); iter != pending_req_db.end(); iter++ ){
		bt_swarm_sched_request_t *	sched_request	= *iter;
		// forward the restart_timeout_for_allreq to this sched_block
		sched_request->restart_timeout(expire_delay); 
	}
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        bt_io_write_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
 */
bool	bt_swarm_sched_block_t::neoip_bt_io_write_cb(void *cb_userptr, bt_io_write_t &cb_io_write
						, const bt_err_t &writev_err)	throw()
{
	bt_swarm_t *	bt_swarm	= sched_piece->swarm_sched()->bt_swarm();	
	// backup the sched_piece pointer to use it after the autodeletion
	bt_swarm_sched_piece_t *sched_piece	= this->sched_piece;
	// log to debug
	KLOG_DBG("enter writev_err=" << writev_err);
	
	// if the bt_io_write_t failed, notify an error
	if( writev_err.failed() )	return bt_swarm->notify_disk_error(writev_err);

	// unlink this bt_swarm_sched_block_t from the sched_piece
	// - this allow the sched_piece to know this sched_block is nomore
	state_goto( STATE_NONE );
	
	// notify the sched_piece that this block is completed
	// - NOTE: nor sched_piece or the sched_block got deleted during this function
	sched_piece->notify_block_completion(this);

	// autodelete this sched_block and return dontkeep
	nipmem_delete	this;
	return false;
}

NEOIP_NAMESPACE_END





