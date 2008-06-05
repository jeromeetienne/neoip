/*! \file
    \brief Definition of the tcp_full_t

\par TODO
- the api for the maysend_is_set or sendbuf_is_limited is crap
  - do a constant in the tcp_full_t and compare it with the get function

*/

/* system include */
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

/* local include */
#include "neoip_tcp_full.hpp"
#include "neoip_fdwatch.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref tcp_full_t constant
const size_t	tcp_full_t::RCVDATA_MAXLEN_NOREAD	= 0;
const size_t	tcp_full_t::SENDBUF_UNLIMITED		= std::numeric_limits<size_t>::max();
const size_t	tcp_full_t::MAYSEND_TSHOLD_UNSET	= 0;
// end of constants definition



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
tcp_full_t::tcp_full_t(const ipport_addr_t &local_addr, const ipport_addr_t &remote_addr
							, fdwatch_t *fdwatch)	throw()
{
	// copy the parameter
	this->local_addr	= local_addr;
	this->remote_addr	= remote_addr;
	this->fdwatch		= fdwatch;
	// zero some field
	callback		= NULL;
	// set default parameters
	recv_max_len		= profile.recv_max_len();
	sendbuf_max_len		= profile.sendbuf_max_len();
	maysend_threshold	= -1;
}

/** \brief destructor
 */
tcp_full_t::~tcp_full_t()				throw()
{
	// close the fdwatch if needed
	nipmem_zdelete	fdwatch;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the callback
 */
tcp_full_t &	tcp_full_t::set_callback(tcp_full_cb_t *callback, void *userptr)throw()
{
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// return the object itself
	return *this;
}


/** \brief Set the profile for this object
 */
tcp_full_t &	tcp_full_t::set_profile(const tcp_full_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == inet_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       start
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief start the action
 */
inet_err_t	tcp_full_t::start()						throw()
{
	std::string		errstr;
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == inet_err_t::OK );	

	// check the parameter
	// TODO to change remove this BAD_PARAMTER stuff to assert
	if( callback == NULL )		return inet_err_t(inet_err_t::BAD_PARAMETER, "no callback set");
	if( local_addr.is_null() )	return inet_err_t(inet_err_t::BAD_PARAMETER, "no local_addr set.");
	if( remote_addr.is_null() )	return inet_err_t(inet_err_t::BAD_PARAMETER, "no remote_addr set.");
	if( fdwatch == NULL )		return inet_err_t(inet_err_t::BAD_PARAMETER, "no fdwatch set.");

	recv_max_len		= profile.recv_max_len();
	sendbuf_max_len		= profile.sendbuf_max_len();

	 // set this socket in non blocking for the connection
	if( fcntl(fdwatch->get_fd(), F_SETFL, O_NONBLOCK) < 0 ){
		errstr = "cant put socket in O_NONBLOCK mode. errno=" + inet_oswarp_t::sock_strerror();
		goto error;
	}

#if 0
	// set SO_LINGER (keept just to show an example of how to put a tcp in linger)
	// - TODO unsure it is the good thing to put that ?
	// - clearly linked with the local linger
	struct linger	linger_opt;
	linger_opt.l_onoff	= 1;
	linger_opt.l_linger	= 60;	// wait for 60second (or any other delay :)
	if( setsockopt(fdwatch->get_fd(), SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt)) < 0 ){
		errstr = "cant setsockopt SO_LINGER. errno=" + inet_oswarp_t::sock_strerror();
		goto error;
	}
#endif

	// set the fdwatch to change the callback and set the condition
	fdwatch->change_callback(this, NULL);
	fdwatch->cond(fdwatch_t::INPUT | fdwatch_t::ERROR);
	
	// log to debug
	KLOG_DBG("tcp_full_t created " << *this);		

	// return no error
	return inet_err_t::OK;

// TODO to remove this goto
error:;	return inet_err_t(inet_err_t::SYSTEM_ERR, errstr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       RECV_MAX_LEN get/set
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the maximum number of bytes notified by RECVED_DATA event
 * 
 * @param recv_max_len the maximum number of byte. if == 0, any input on this
 *                     connection will be ignored
 */
inet_err_t	tcp_full_t::recv_max_len_set( ssize_t recv_max_len ) throw()
{
	this->recv_max_len	= recv_max_len;
	// if the recv_max_len is 0, make sure the input IS NOT watched
	if( this->recv_max_len == 0 && fdwatch->cond().is_input() )
		fdwatch->cond( fdwatch->cond() & ~fdwatch_t::INPUT );
	// if the recv_max_len is > 0, make sure the input IS watched
	if( this->recv_max_len > 0 && fdwatch->cond().is_input() == false )
		fdwatch->cond( fdwatch->cond() | fdwatch_t::INPUT );
	// return no error
	return inet_err_t::OK;
}

/** \brief return the recv_max_len
 */
ssize_t tcp_full_t::recv_max_len_get() const throw()
{
	return recv_max_len;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       MAYSEND_THRESHOLD get/set
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the maysend_threshold
 * 
 * - WARNING: this function doesnt notify MAYSEND_ON/OFF events. it is to avoid
 *   hard to detect bug when the caller use this function in constructors
 *   - the caller may use sendbuf_get_free_len() and co to initiate a mechanism
 *     if needed.
 *   - TODO look at rdgram_t there is something about that i think
 */
inet_err_t	tcp_full_t::maysend_set_threshold(ssize_t maysend_threshold) throw()
{
	this->maysend_threshold	= maysend_threshold;
	return inet_err_t::OK;
}

/** \brief return the maysend_threshold
 */
ssize_t	tcp_full_t::maysend_get_threshold()	const throw()
{
	return maysend_threshold;
}

/** \brief return true if the maysend is set, false otherwise
 * 
 * - maysend means something IIF the sendbuf is limited
 */
bool	tcp_full_t::maysend_is_set()		const throw()
{
	return maysend_threshold != -1 && sendbuf_is_limited();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OUTQUEUE_MAX_LEN get/set
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the sendbuf_max_len
 * 
 * @param sendbuf_max_len the new sendbuf maximum length. 
 *                        WARNING: It MUST NOT be less than the current used length.
 */
inet_err_t	tcp_full_t::sendbuf_set_max_len( ssize_t sendbuf_max_len ) throw()
{
	// set the new value	
	this->sendbuf_max_len	= sendbuf_max_len;
	// sanity check - the new sendbuf_max_len MUST NOT be >= the used length
	if( sendbuf_is_limited() )	DBG_ASSERT( (ssize_t)sendbuf.get_len() <= sendbuf_max_len );
	// return no error
	return inet_err_t::OK;
}

/** \brief return the sendbuf_max_len
 */
ssize_t	tcp_full_t::sendbuf_get_max_len() const throw()
{
	return sendbuf_max_len;
}

/** \brief return the used length of the sendbuf
 */
ssize_t tcp_full_t::sendbuf_get_used_len() const throw()
{
	// sanity check - the current sendbuf length MUST always be >= 0 and <= sendbuf_max_len
	if( sendbuf_is_limited() )	DBG_ASSERT( (ssize_t)sendbuf.get_len() <= sendbuf_max_len );
	// return the current size
	return sendbuf.get_len();
}

/** \brief return the amount of free space in the sendbuf
 * 
 * @return the amount of free space in the sendbuf. if the sendbuf is not limited, return -1
 */
ssize_t tcp_full_t::sendbuf_get_free_len()	const throw()
{
	if( sendbuf_is_limited() == false )	return -1;
	return sendbuf_get_max_len() - sendbuf_get_used_len();
}

/** \brief return true if the sendbuf is limited, false otherwise
 */
bool	tcp_full_t::sendbuf_is_limited()	const throw()
{
	return sendbuf_max_len != -1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       fdwatch callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool	tcp_full_t::neoip_fdwatch_cb( void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cond )	throw()
{
	int	readlen = 0;
	// some logging
	KLOG_DBG( *this << " received event " << cond << ". watching " << fdwatch->cond());

	// if the condition is input
	if( cond.is_input() ){
		// sanity check - the recevied condition must be watched
		DBG_ASSERT( fdwatch->cond().is_input() );
#if 0		// TODO here to avoid a memory allocation, the pkt_t could be init from a alloca()
		pkt_t	pkt(recv_max_len);
		pkt.set_size(recv_max_len);
		// read data in the socket
		readlen = read(fdwatch->get_fd(), pkt.get_ptr(), pkt.get_len());
#else
		void *	buffer	= nipmem_alloca(recv_max_len);
		// read data in the socket
		readlen = read(fdwatch->get_fd(), buffer, recv_max_len);
#endif
		// if readlen < 0, treat it as error
		// - due to a weird bug in glib|linux, G_IO_ERR/HUP isnt received
		//   so there is a test if (cond.is_input() && readlen==0) fallthru
		//   and treat it as error
		if( readlen < 0 )	readlen = 0;
		// if some data have been read, notify the caller
		if( readlen > 0 ){
#if 0
			// shrink the pkt back to the used length
			pkt.tail_remove(recv_max_len - readlen);
#else
			pkt_t	pkt;
			pkt.work_on_data_nocopy(buffer, readlen);
#endif
			// build the event to report
			tcp_event_t	tcp_event = tcp_event_t::build_recved_data(&pkt);
			// notify the caller
			return notify_callback( tcp_event );
		}
	}
	
	// if the condition is output
	if( cond.is_output() ){
		ssize_t	old_free_len = sendbuf_get_free_len();
		// sanity check - the recevied condition must be watched
		DBG_ASSERT( fdwatch->cond().is_output() );
		// sanity check - the sendbuf MUST NOT be empty
		DBG_ASSERT( sendbuf.get_len() );
		// try to write the whole sendbuf in the socket
		ssize_t	written_len= write(fdwatch->get_fd(), sendbuf.get_data(), sendbuf.get_len());
		// if the write failed, notify the caller of a CNX_CLOSED
		if( written_len == -1 ){
			std::string	reason = "write failed";
			return notify_callback( tcp_event_t::build_cnx_closed(reason) );	
		}

		// remove all the written data from the sendbuf
		sendbuf.head_consume( written_len );

		// remove the output fdwatch_cond_t if the queue is now empty
		if( sendbuf_get_used_len() == 0 )
			fdwatch->cond( fdwatch->cond() & ~fdwatch_t::OUTPUT );

		// if maysend is set and the sendbuf used length just became
		// larger than or equal to maysend_threshold, notify a MAYSEND_ON
		if( maysend_is_set() && sendbuf_get_free_len() >= maysend_threshold 
						&& old_free_len < maysend_threshold ){
			bool	tokeep	= notify_callback( tcp_event_t::build_maysend_on() );
			if( !tokeep )	return false;
		}
	}

	/* due to glib iowatch, G_IO_ERR isnt recevied for unknown reason ?!?!!?
	 * - the work around is 'if G_IO_IN is set and no data to read, treat it as G_IO_ERR'
	 * - NOTED LATER: it is unclear this is a glib issue as libevent does the same
	 *   it may be a linux issue
 	 */
	// handle a connection error
	if( cond.is_error() || (cond.is_input() && readlen == 0) ){
		std::string	reason	 = "undetermined";
		int		sockopt_val;
		socklen_t	sockopt_len = sizeof(sockopt_val);
		// get the error code
		if(getsockopt(cb_fdwatch.get_fd(), SOL_SOCKET, SO_ERROR, &sockopt_val, &sockopt_len)==0)
			reason = neoip_strerror(sockopt_val);
		// notify the caller
		tcp_event_t	tcp_event = tcp_event_t::build_cnx_closed(reason);
		return notify_callback( tcp_event );
	}
	// return 'tokeep'
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       SEND function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief write data on the tcp_full_t
 * 
 * @return the amount of byte written (NOTE it is between >= 0 and <= to data_len)
 *         or 0 if an error occurs
 */
ssize_t tcp_full_t::send(const void *data_ptr, size_t data_len) 	throw()
{
	ssize_t	sendlen;
	// log to debug
	KLOG_DBG("enter data=" << datum_t(data_ptr, data_len, datum_t::NOCOPY));
	// compute the sendlen
	if( sendbuf_is_limited() )	sendlen = MIN((ssize_t)data_len, sendbuf_max_len - sendbuf_get_used_len());
	else				sendlen = data_len;
	// sanity check - sendlen MUST be >= as the current length is always <= than the max length
	DBG_ASSERT( sendlen >= 0 );
	// add the data to the sendbuf if room is available
	if( sendlen > 0 ){
		ssize_t	old_free_len = sendbuf_get_free_len();

		// add the data to the queue
		sendbuf.append(data_ptr, sendlen);
		
		// if the output isnt yet watch, watch it
		if( !fdwatch->cond().is_output() )
			fdwatch->cond( fdwatch->cond() | fdwatch_t::OUTPUT );

		// if maysend is set and the sendbuf used length just became less than maysend_threshold
		// => notify a MAYSEND_OFF using a zerotimer not to notify callback during an external
		//    function call
		if( maysend_is_set() && sendbuf_get_free_len() < maysend_threshold 
						&& old_free_len >= maysend_threshold ){
			// set the zerotimer to notify the maysend_off
			zerotimer_maysend_off.append(this, NULL);
		}
	}
	// return the sendlen
	return sendlen;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer_maysendoff callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	tcp_full_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// if maysend is set and the sendbuf used length just became less than maysend_threshold
	// => notify a MAYSEND_OFF
	if( maysend_is_set() && sendbuf_get_free_len() < maysend_threshold ){			
		bool to_keep = notify_callback( tcp_event_t::build_maysend_off() );
		if( !to_keep ) return false;
	}
	// return tokeep
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	tcp_full_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return local_addr.to_string() + " to " + remote_addr.to_string();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool tcp_full_t::notify_callback(const tcp_event_t &tcp_event)			throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_tcp_full_event_cb(userptr, *this, tcp_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END



