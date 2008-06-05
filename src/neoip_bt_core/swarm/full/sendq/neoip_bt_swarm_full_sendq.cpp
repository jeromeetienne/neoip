/*! \file
    \brief Class to handle the bt_swarm_full_t

\par Brief Description
- this object is done to queue the command to send until there is enought
  room in the socket_full_t sendbuf to receive them
  - the data of the PIECE_BFIELD and BLOCK_REP are built on the fly
  - those are the 2 commands which requires a significant amount of data
  - so this queue is of unlimited length and all the rest of the code doesnt
    have to handle the sendbuf
- additionnaly it is in charge of sending KEEP_ALIVE when the connection has
  been idle for too long.
  
\par about atomic block_rep
- this is important to keep the block_rep writing atomic
- thus if the block_rep is deleted within the queue, either it is 
  fully sent thru the socket_full_t or it is not sent at all.
- if it was only partially sent thru the socket_full_t, the remote peer will
  wait for ever the block_rep.
  - and likely timeout the connection as being idle for too long. 

\par Possible Improvement
- currently the BLOCK_REP are really read on the fly, it may be good later
  to implement a disk cache with a readahead.
  - it is a good idea but if so, it should be handled in the bt_io_vapi_t
    and not here.

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_full_sendq.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_io_read.hpp"
#include "neoip_bt_io_vapi.hpp"
#include "neoip_file_range.hpp"
#include "neoip_socket_full.hpp"
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
bt_swarm_full_sendq_t::bt_swarm_full_sendq_t(bt_swarm_full_t *swarm_full)	throw()
{
	const bt_swarm_profile_t &swarm_profile	= swarm_full->bt_swarm->profile();	
	// copy the parameters
	this->swarm_full	= swarm_full;
	// zero some fields
	bt_io_read		= NULL;
	// start the kalive_timeout
	xmit_kalive_timeout.start(swarm_profile.keepalive_period(), this, NULL);	
}

/** \brief Destructor
 */
bt_swarm_full_sendq_t::~bt_swarm_full_sendq_t()					throw()
{
	// delete the bt_io_read_t if needed
	nipmem_zdelete	bt_io_read;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool 	bt_swarm_full_sendq_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// queue a bt_cmd_t::KEEPALIVE
	queue_cmd( bt_cmd_t::build_keepalive() );
	// return tokeep
	return true;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief queue a command in the send queue
 */
void	bt_swarm_full_sendq_t::queue_cmd(const bt_cmd_t &bt_cmd)	throw()
{
	// log to debug
	KLOG_DBG("enter cmd=" << bt_cmd);
	// sanity check - the bt_cmd_t MUST NOT be null
	DBG_ASSERT( !bt_cmd.is_null() );
	// put the bt_cmd_t at the end of the queue
	cmd_queue.push_back( bt_cmd );
	// try to fill the socket sendbuf
	fill_socket_xmitbuf();
}

/** \brief For the caller to notify the maysend event on the socket_full_t
 */
void	bt_swarm_full_sendq_t::notify_maysend()	throw()
{
	// try to fill the socket
	fill_socket_xmitbuf();
}

/** \brief Remove the BLOCK_REP matching the one passed as parameter
 * 
 * - this function is used when a BLOCK_DEL is received from an remote peer
 */
void	bt_swarm_full_sendq_t::remove_one_block_rep(const bt_cmd_t &bt_cmd)	throw()
{
	std::list<bt_cmd_t>::iterator	iter;
	// sanity check - the bt_cmd_t MUST be a BLOCK_REP
	DBG_ASSERT( bt_cmd.cmdtype() == bt_cmdtype_t::BLOCK_REP );
	// try to find bt_cmd_t in the queue
	for(iter = cmd_queue.begin(); iter != cmd_queue.end() && *iter != bt_cmd; iter++);
	// if the bt_cmd_t has not been found, return now
	// - this may happen when, for example, the BLOCK_REP has been already removed
	//   due to the local peer sending a UNAUTH_REQ and the remote peer sending
	//   a BLOCK_DEL before receiving the UNAUTH_REQ
	if( iter == cmd_queue.end() )	return;
	// if the bt_cmd_t is the first command and that a bt_io_read is in progress, delete the bt_io_read_t
	if( iter == cmd_queue.begin() && bt_io_read )	nipmem_zdelete	bt_io_read;
	// remove the bt_cmd from the queue
	cmd_queue.erase(iter);
	// fill the socket sendbuf
	fill_socket_xmitbuf();
}

/** \brief Remove all the BLOCK_REP in the cmd_queue
 * 
 * - intended to be used after a bt_swarm_full_t is forbidden to request
 */
void	bt_swarm_full_sendq_t::remove_all_block_rep()			throw()
{
	// remove ALL the BLOCK_REP from the cmd_queue
	// 2 nested loop in order to remove element in the cmd_queue while scanning it
	// - TODO why not a copy of the list ? less complex
	// - it is required as removing an element destroy any iterator on the list
	// - in theory, it may be done in a single loop with iter = cmd_queue.erase(iter)
	//   but i dont trust this, as it would require that .end()++ == end() and i dont
	//   think it is garanteed
	while( true ){
		std::list<bt_cmd_t>::iterator	iter;
		// go thru the whole cmd_queue
		for(iter = cmd_queue.begin(); iter != cmd_queue.end(); iter++){
			bt_cmd_t &	bt_cmd	= *iter;
			// if this command is not a BLOCK_REP, skip it
			if( bt_cmd.cmdtype() != bt_cmdtype_t::BLOCK_REP )	continue;
			// remove this element
			cmd_queue.erase(iter);
			// leave the loop - as the erase() invalidate any iterator on cmd_queue
			break;
		}
		// if this iteration of the inner loop found no BLOCK_REP, leave the outter loop
		if( iter == cmd_queue.end() )	break;
	}
	// if bt_io_read_t is inprogress, delete it
	if( bt_io_read )	nipmem_zdelete	bt_io_read;
	// try to fill the socket
	fill_socket_xmitbuf();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief fill the socket sendbuf if possible
 */
void	bt_swarm_full_sendq_t::fill_socket_xmitbuf()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// loop until the cmd_queue is empty
	while( !cmd_queue.empty() ){
		bool	not_sent;
		// send the front bt_cmd_t of the cmd_queue
		not_sent	= send_front_cmd();
		// if sending the front bt_cmd_t failed, return now
		if( not_sent )	return;
	}
}

/** \brief try to send the front bt_cmd_t
 * 
 * @return a not_sent aka false if a command have been sent, true if it is not possible 
 *         to send it now (no more room in the socket_full_t sendbuf or bt_io_read_t in progress)
 */
bool	bt_swarm_full_sendq_t::send_front_cmd()	throw()
{
	const bt_swarm_profile_t &swarm_profile	= swarm_full->bt_swarm->profile();		
	socket_full_t *		socket_full	= swarm_full->socket_full;
	bt_swarm_t *		bt_swarm	= swarm_full->bt_swarm;
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	// log to debug
	KLOG_DBG("enter");
	// reset the xmit_kalive_timeout
	xmit_kalive_timeout.start(swarm_profile.keepalive_period(), this, NULL);	
	// sanity check - the cmd_queue MUST NOT be empty
	DBG_ASSERT( !cmd_queue.empty() );
	// get the next command
	bt_cmd_t	next_cmd	= cmd_queue.front();
	// get the command length
	size_t		cmd_len		= next_cmd.get_datum_len(*bt_swarm);
	// if the socket_full sendbuf has not the room to be receive the next cmd, ask to 
	// be warned when it is and return not_sent
	if( socket_full->xmitbuf_freelen() < cmd_len ){
		socket_full->maysend_tshold(cmd_len);
		return true;
	}
	// if next_cmd is a bt_cmdtype_t::BLOCK_REP, it is a special case
	// - launch a bt_io_read_t for the data to reply if not already done
	// - the command+data are written in the socket_full_t sendbuf in the bt_io_read_t callback
	if( next_cmd.cmdtype() == bt_cmdtype_t::BLOCK_REP ){
		const bt_prange_t &	cmd_prange	= next_cmd.prange();
		// if bt_io_read_t is already launched, return not_sent now
		if( bt_io_read )	return true;
		// launch the bt_io_read_t
		bt_io_vapi_t *	io_vapi	= bt_swarm->io_vapi();
		bt_io_read	= io_vapi->read_ctor(cmd_prange.to_totfile_range(bt_mfile), this, NULL);
		// return not_sent
		return true;
	}
	// dequeue the next command
	cmd_queue.pop_front();
	// convert the next_cmd to a datum
	datum_t	cmd_datum	= next_cmd.to_datum(*bt_swarm);
	DBG_ASSERT( cmd_datum.size() == cmd_len );
	// update the dloaded_fulllen in bt_swarm_stats_t
	bt_swarm_stats_t & swarm_stats	= bt_swarm->swarm_stats();
	swarm_stats.uloaded_fulllen(swarm_stats.uloaded_fulllen()+cmd_datum.size());
	// log to debug
	KLOG_DBG("send a " << cmd_datum );
	// push it in socket_full_t sendbuf
	socket_full->send( cmd_datum );
	// return false - to show the front bt_cmd_t HAS been sent
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        bt_io_read_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
 */
bool	bt_swarm_full_sendq_t::neoip_bt_io_read_cb(void *cb_userptr, bt_io_read_t &cb_io_read
				, const bt_err_t &read_err, const datum_t &read_data)	throw()
{
	bt_swarm_t *		bt_swarm	= swarm_full->bt_swarm;
	bt_swarm_stats_t &	swarm_stats	= bt_swarm->swarm_stats();
	socket_full_t *		socket_full	= swarm_full->socket_full;	
	// log to debug
	KLOG_WARN("enter read_err=" << read_err << " read_data size=" << read_data.size());

	// if the bt_io_read_t failed, notify a bt_swarm_event_t::DISK_ERROR
	if( read_err.failed() )	return bt_swarm->notify_disk_error(read_err);	

	// sanity check - the next_cmd MUST be a bt_cmdtype_t::BLOCK_REP
	DBG_ASSERT( cmd_queue.front().cmdtype() == bt_cmdtype_t::BLOCK_REP );
	// get the next command
	bt_cmd_t 		next_cmd	= cmd_queue.front();
	const bt_prange_t &	cmd_prange	= next_cmd.prange();
	// sanity check - the next_cmd MUST have a data_len equal to the read_data.size()
	DBG_ASSERT( cmd_prange.length() == read_data.size() );
	// dequeue the next command
	cmd_queue.pop_front();

	// build the payload header to send
	bytearray_t	bytearray;
	bytearray << uint32_t(9 + read_data.size());	// length of the payload
	bytearray << next_cmd.cmdtype();		// command type
	bytearray << cmd_prange.piece_idx();
	bytearray << cmd_prange.offset();
	
	// update the bt_swarm_stats_t in bt_swarm_t
	swarm_stats.uloaded_datalen( swarm_stats.uloaded_datalen() + cmd_prange.length() );
	swarm_stats.uloaded_fulllen( swarm_stats.uloaded_fulllen() + bytearray.length() + read_data.size() );
	// update the xmit_rate
	swarm_full->m_xmit_rate.update(cmd_prange.length());
	
	// sanit check - the socket_full xmitbuf MUST have enougth room to receive the data
	DBG_ASSERT( socket_full->xmitbuf_freelen() >= bytearray.size() + read_data.size() );
	// send the data to the socket_full_t
	socket_full->send( bytearray.to_datum(datum_t::NOCOPY) );
	socket_full->send( read_data );

	// delete the bt_io_read_t
	nipmem_zdelete	bt_io_read;
	
	// try to fill the socket
	fill_socket_xmitbuf();
		
	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END





