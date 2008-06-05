/*! \file
    \brief Class to handle the bt_http_ecnx_herr_t

\par Brief Description
\ref bt_http_ecnx_herr_t is the error handler for the bt_http_ecnx_cnx_t.
There are 2 kinds of error: 
1. at the cnx level: e.g. the remote host is unreachable
2. at the http level: e.g. the requested file is no more present
- an cnx error may happen in normal situations
  - it is common for http server to reject requests as a load balancing mechanism
- an http level error doesnt happen in normal situations and is unlikely to recover

*/

/* system include */
/* local include */
#include "neoip_bt_http_ecnx_herr.hpp"
#include "neoip_bt_http_ecnx_pool.hpp"
#include "neoip_bt_http_ecnx_cnx.hpp"
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
bt_http_ecnx_herr_t::bt_http_ecnx_herr_t(bt_http_ecnx_pool_t *ecnx_pool
						, const std::string &hostport_str)	throw()
{
	// log to debug
	KLOG_DBG("enter hostport_str=" << hostport_str);
	// copy the parameter
	this->ecnx_pool		= ecnx_pool;
	this->hostport_str	= hostport_str;

	// sanity check - bt_http_ecnx_herr_t MUST NOT already exist for this hostport_str
	DBG_ASSERT( !ecnx_pool->ecnx_herr_by_hostport_str(hostport_str) );	
	// link this object to bt_http_ecnx_pool_t
	ecnx_pool->ecnx_herr_dolink(this);
	// reset the retry_delaygen
	retry_delaygen	= delaygen_t(ecnx_pool->get_profile().herr_delaygen());	
}

/** \brief Destructor
 */
bt_http_ecnx_herr_t::~bt_http_ecnx_herr_t()		throw()
{
	// unlink this object from bt_http_ecnx_pool_t
	ecnx_pool->ecnx_herr_unlink(this);
}
 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function called to notify that this hostport_str has just received a error at the cnx level
 */
void	bt_http_ecnx_herr_t::notify_cnx_failed(const bt_err_t &bt_err)	throw()
{
	// log to debug
	KLOG_DBG("enter bt_err=" << bt_err);
	// forward the bt_err_t to the common processing 
	notify_common_failed(bt_err);
}

/** \brief function called to notify that this hostport_str has just received a success
 */
void	bt_http_ecnx_herr_t::notify_cnx_succeed()				throw()
{
	// log to debug
	KLOG_DBG("enter");
	// reset the retry_delaygen
	retry_delaygen	= delaygen_t(ecnx_pool->get_profile().herr_delaygen());
}

/** \brief function called to notify that this hostport_str has just received an error at the http level
 */
void	bt_http_ecnx_herr_t::notify_http_failed(const http_uri_t &http_uri
						, const bt_err_t &bt_err)	throw()
{
	// log to debug
	KLOG_DBG("enter http_uri=" << http_uri << " bt_err=" << bt_err);
	// add this http_uri_t to the uri_blacklist - IIF not in there already
	// - NOTE: with concurrent bt_http_ecnx_cnx_t it is possible to get
	//   the same http_uri_t notified multiple time. once per bt_http_ecnx_cnx_t
	if( uri_blacklist.find(http_uri) == uri_blacklist.end() ){
		bool	succeed	= uri_blacklist.insert(http_uri).second;
		DBG_ASSERT( succeed );
	}

	// forward the bt_err_t to the common processing 
	notify_common_failed(bt_err);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     Internal function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief common part of processing of bt_err_t 
 */
void	bt_http_ecnx_herr_t::notify_common_failed(const bt_err_t &bt_err)	throw()
{
	// update the m_last_error
	m_last_error	= bt_err;
	// launch retry_timeout if it is not already running
	if( !retry_timeout.is_running() ){
		delay_t	retry_delay;
		// compute the delay to wait before retrying
		if( retry_delaygen.is_timedout() )	retry_delay	= delay_t::NEVER;
		else					retry_delay	= retry_delaygen.post_inc();
		// set the next timer
		retry_timeout.start(retry_delay, this, NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool 	bt_http_ecnx_herr_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// stop the retry_timeout
	retry_timeout.stop();
	// notify the expiration of the timeout to the delaygen
	retry_delaygen.notify_expiration();

	// create the needed bt_http_ecnx_cnx_t for this hostport_str
	bt_err_t bt_err	= ecnx_pool->launch_ecnx_cnx_if_needed(hostport_str);
	DBG_ASSERT( bt_err.succeed() );	// TODO poor error management

	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END





