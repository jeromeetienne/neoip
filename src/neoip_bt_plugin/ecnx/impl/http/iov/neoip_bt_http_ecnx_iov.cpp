/*! \file
    \brief Class to handle the bt_http_ecnx_iov_t

\par Brief Decription
bt_http_ecnx_iov_t handle a http_sclient_t for a given bt_iov_t.

\par About progress chunk 
- it uses http_sclient_t with the progress_chunk feature to be able to notify
  the data as soon as they are received.
- It limits the latency because there is no need to wait for the end of http_sclient_t

*/

/* system include */
/* local include */
#include "neoip_bt_http_ecnx_iov.hpp"
#include "neoip_bt_http_ecnx_cnx.hpp"
#include "neoip_bt_http_ecnx_pool.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_http_sclient.hpp"
#include "neoip_http_sclient_res.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_socket_itor_arg.hpp"
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
bt_http_ecnx_iov_t::bt_http_ecnx_iov_t()		throw()
{
	// zero some fields
	m_http_sclient	= NULL;
}

/** \brief Destructor
 */
bt_http_ecnx_iov_t::~bt_http_ecnx_iov_t()		throw()
{
	// delete the http_sclient_t if needed
	nipmem_zdelete	m_http_sclient;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_http_ecnx_iov_t::start(bt_http_ecnx_cnx_t *m_ecnx_cnx, bt_iov_t &m_bt_iov
					, bt_http_ecnx_iov_cb_t *callback, void *userptr)	throw()
{
	const bt_http_ecnx_cnx_profile_t &profile	= m_ecnx_cnx->profile();
	bt_http_ecnx_pool_t *		ecnx_pool	= m_ecnx_cnx->ecnx_pool();
	const http_uri_t &		subfile_uri	= m_ecnx_cnx->subfile_uri_arr[m_bt_iov.subfile_idx()];
	bt_swarm_t *			bt_swarm	= ecnx_pool->get_swarm();
	// sanity check - the subfile_uri MUST NOT be null
	DBG_ASSERT( !subfile_uri.is_null() );
	// log to debug
	KLOG_ERR("enter bt_iov=" << m_bt_iov);
	// copy the parameters
	this->m_ecnx_cnx	= m_ecnx_cnx;
	this->m_bt_iov		= m_bt_iov;
	this->callback		= callback;
	this->userptr		= userptr;

	// create the http_reqhd_t to use with the http_sclient_t
	http_reqhd_t	http_reqhd;
	http_reqhd.uri(subfile_uri).range(m_bt_iov.subfile_range());
	
	// update the bt_swarm_stats_t
	bt_swarm_stats_t&	swarm_stats	= bt_swarm->swarm_stats();
	swarm_stats.uloaded_fulllen(swarm_stats.uloaded_fulllen() + http_reqhd.to_http().size());

	// TODO to factorize - this socket_profile_t stuff is the same in bt_session_t
	//                     bt_swarm_peersrc_t and bt_http_ecnx_iov_t
	// build the socket_profile_t for the socket_itor_t
	const bt_swarm_profile_t &swarm_profile	= ecnx_pool->get_swarm()->profile();
	socket_profile_t	socket_profile(socket_domain_t::TCP);
	if( swarm_profile.xmit_limit_arg().is_valid() )	socket_profile.full().xmit_limit_arg(swarm_profile.xmit_limit_arg());
	if( swarm_profile.recv_limit_arg().is_valid() )	socket_profile.full().recv_limit_arg(swarm_profile.recv_limit_arg());
	// build socket_itor_arg_t for http_sclient_t - no remote_addr to solve it by http_uri_t::host()
	socket_itor_arg_t	itor_arg;
	itor_arg	= socket_itor_arg_t().profile(socket_profile).domain(socket_domain_t::TCP)
						.type(socket_type_t::STREAM);

	// get the http_sclient_profile_t	- used to determine progress_chunk_len
	http_sclient_profile_t	sclient_profile	= profile.http_sclient();
#if 1	// NOTE: to enable/disable the progress_chunk notification in http_sclient_t
	sclient_profile.progress_chunk_len	(bt_swarm->profile().xmit_req_maxlen());
#endif
	// create the http_sclient_t
	http_err_t	http_err;
	m_http_sclient	= nipmem_new http_sclient_t();
	http_err	= m_http_sclient->set_client_pool(ecnx_pool->get_http_client_pool())
					.set_itor_arg(itor_arg)
					.set_profile(sclient_profile)
					.start(http_reqhd, this, NULL);
	if( http_err.failed() )	return bt_err_from_http(http_err);
	
	// return no error
	return bt_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     http_sclient_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
	
/** \brief callback notified by \ref http_client_t to provide event
 */
bool	bt_http_ecnx_iov_t::neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
					, const http_sclient_res_t &sclient_res)	throw() 
{
	bt_http_ecnx_pool_t *	ecnx_pool	= m_ecnx_cnx->ecnx_pool();
	bt_swarm_t *		bt_swarm	= ecnx_pool->get_swarm();
	bt_swarm_stats_t&	swarm_stats	= bt_swarm->swarm_stats();
	const bytearray_t &	reply_body	= sclient_res.reply_body();
	const http_rephd_t &	http_rephd	= sclient_res.http_rephd();
	// log to debug
	KLOG_ERR("enter http_sclient_res=" << sclient_res);
	
	// update the bt_http_ecnx_cnx_stats_t from the result of this bt_http_ecnx_iov_t
	m_ecnx_cnx->cnx_stats().update_from_iov_res(this, sclient_res);

	// update the bt_swarm_stats_t dloaded_fulllen with the http_rephd
	swarm_stats.dloaded_fulllen(swarm_stats.dloaded_fulllen() + http_rephd.to_http().size()
							+ reply_body.size());

	// if the http_sclient_t has a connection error, return an error
	if( sclient_res.cnx_err().failed() ){
		bt_err_t	bt_err	= bt_err_t(bt_err_t::ERROR, sclient_res.cnx_err().to_string());
		return notify_callback_err( bt_err );
	}

	// if an error occured and the partial get failed, return a bt_err_t::FROM_HTTP
	// - the FROM_HTTP is used to determine that the error is at the file level
	if( !sclient_res.part_get_ok() ){
		bt_err_t	bt_err	= bt_err_t(bt_err_t::FROM_HTTP, "failed request");
		return notify_callback_err( bt_err );
	}

	// update the bt_swarm_stats_t dloaded_datalen
	swarm_stats.dloaded_datalen(swarm_stats.dloaded_datalen() + reply_body.size());
	
	// sanity check - the reply_body MUST BE as long as the bt_iov
	DBG_ASSERT( reply_body.size() == m_bt_iov.length().to_size_t() );
	
	// report the data
	bool	tokeep	= notify_callback(bt_err_t::OK, reply_body.to_datum(datum_t::NOCOPY)
						, http_rephd.is_connection_close());
	if( !tokeep )	return false;

	// return dontkeep
	return false;
}


/** \brief callback notified only when http_sclient_t has new progress_chunk available
 */
bool	bt_http_ecnx_iov_t::neoip_http_sclient_progress_chunk_cb(void *cb_userptr
					, http_sclient_t &cb_sclient)		throw()
{
	const http_sclient_res_t &	sclient_res	= m_http_sclient->current_res();
	const bytearray_t &		reply_body	= sclient_res.reply_body();
	// log to debug
	KLOG_DBG("enter");

	// compute the bt_iov_t for the current reply_body (aka a partial one)
	bt_iov_t	cb_bt_iov(bt_iov().subfile_idx(), bt_iov().subfile_beg(), reply_body.size());

	// report the progress_chunk to the caller
	bool	tokeep	= notify_callback_progress_chunk(cb_bt_iov
					, reply_body.to_datum(datum_t::NOCOPY));
	if( !tokeep )	return false;
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback for a progress_chunk
 */
bool	bt_http_ecnx_iov_t::notify_callback_progress_chunk(const bt_iov_t &bt_iov
						, const datum_t &recved_data)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_http_ecnx_iov_progress_chunk_cb(userptr, *this
								, bt_iov, recved_data);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

/** \brief notify the callback when there is an error
 */
bool	bt_http_ecnx_iov_t::notify_callback_err(const bt_err_t &bt_err)		throw()
{
	// sanity check - bt_err_t MUST be failed
	DBG_ASSERT( bt_err.failed() );
	// forward to the generic callback
	return notify_callback(bt_err, datum_t(), true);
}

/** \brief notify the callback
 */
bool	bt_http_ecnx_iov_t::notify_callback(const bt_err_t &bt_err, const datum_t &recved_data
						, bool is_cnx_close)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_http_ecnx_iov_cb(userptr, *this, bt_err, recved_data, is_cnx_close);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





