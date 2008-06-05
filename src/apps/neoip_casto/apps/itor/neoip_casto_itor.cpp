/*! \file
    \brief Definition of the \ref casto_itor_t
    
\par Brief Description
casto_itor_t handles the convertion from the mdata_srv_uri/cast_name to a bt_cast_mdata_t.
additionnaly it stored all the bt_httpo_full_t for this mdata_srv_uri/cast_name.
- when the operation is completed, it notifies a bt_cast_mdata_t and bt_err_t
  - if bt_err.failed(), the convertion failed and bt_cast_mdata_t is invalid
  - if bt_err.succeed(), the convertion succeed and bt_cast_mdata_t is valid
    - it it up to the caller to create a casto_swarm_t for this mdata_srv_uri/cast_name
      and to forward all the httpo_full_t for it.

*/

/* system include */
/* local include */
#include "neoip_casto_itor.hpp"
#include "neoip_casto_apps.hpp"
#include "neoip_casto_helper.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_bt_cast_mdata_client.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
casto_itor_t::casto_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_casto_apps	= NULL;
	m_mdata_client	= NULL;
}

/** \brief Destructor
 */
casto_itor_t::~casto_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink this object from the bt_casto_apps_t
	if( m_casto_apps )	m_casto_apps->itor_unlink(this);
	// delete the httpo_full_db with http error 503 "Service Unavailable" - rfc2616.10.5.4
	httpo_full_db_dtor(http_status_t::SERVICE_UNAVAILABLE);
	// delete the bt_cast_mdata_client_t if needed
	nipmem_zdelete	m_mdata_client;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t casto_itor_t::start(casto_apps_t *p_casto_apps, const http_uri_t &m_mdata_srv_uri
				, const std::string &m_cast_name
				, const std::string &m_cast_privhash
				, bt_httpo_full_t *httpo_full
				, casto_itor_cb_t *callback, void *userptr)	throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_ERR("enter");
	// copy the parameter
	this->m_casto_apps	= p_casto_apps;
	this->m_cast_name	= m_cast_name;
	this->m_cast_privhash	= m_cast_privhash;
	this->m_mdata_srv_uri	= m_mdata_srv_uri;
	this->callback		= callback;
	this->userptr		= userptr;
	// link this object to the bt_casto_apps_t
	m_casto_apps->itor_dolink(this);

	// add this bt_httpo_full_t
	httpo_full_push(httpo_full);

	// start the bt_cast_mdata_client_t
	m_mdata_client	= nipmem_new bt_cast_mdata_client_t();
	bt_err		= m_mdata_client->start(mdata_srv_uri(), cast_name(), cast_privhash(), this, NULL);
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			httpo_full_db management
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief push a new bt_httpo_full_t on this casto_itor_t
 */
void	casto_itor_t::httpo_full_push(bt_httpo_full_t *httpo_full)	throw()
{
	// add the new httpo_full to the database
	m_httpo_full_db.push_back(httpo_full);
}

/** \brief pop a bt_httpo_full_t from the casto_itor
 */
bt_httpo_full_t *	casto_itor_t::httpo_full_pop()			throw()
{
	// if the database is empty, return null
	if( m_httpo_full_db.empty() )	return NULL;
	// get the first of the database
	bt_httpo_full_t * httpo_full	= m_httpo_full_db.front();
	// remove it from the database
	m_httpo_full_db.pop_front();
	// return its value
	return httpo_full;	
}

/** \brief Delete the whole http_full_db returning a http error if needed
 */
void	casto_itor_t::httpo_full_db_dtor(const http_status_t &status_code
						, const std::string &reason)	throw()
{
	// delete the httpo_full_db if needed
	while( !m_httpo_full_db.empty() ){
		bt_httpo_full_t * httpo_full	= m_httpo_full_db.front();
		// remote it from the m_httpo_full_db 
		m_httpo_full_db.pop_front();
		// reply the http error
		casto_helper_t::reply_err_httpo_full(httpo_full, status_code, reason);
		// delete the object itself
		nipmem_zdelete httpo_full;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_cast_mdata_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_cast_mdata_client_t to provide event
 */
bool	casto_itor_t::neoip_bt_cast_mdata_client_cb(void *cb_userptr, bt_cast_mdata_client_t &cb_mdata_client
		, const bt_err_t &bt_err, const bt_cast_mdata_t &cast_mdata)	throw() 
{
	// log to debug
	KLOG_ERR("enter bt_err=" << bt_err << " cast_mdata=" << cast_mdata);
	// if bt_cast_mdata_client_t failed, notify all bt_httpo_full_t of the proper reason
	if( bt_err.failed() )	httpo_full_db_dtor(http_status_t::NOT_FOUND, bt_err.reason());

	// simply forward the result to the caller
	// - NOTE: up to it to start a casto_swarm_t and to have
	//   the bt_httpo_full_t attached to it
	return notify_callback(bt_err, cast_mdata);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool casto_itor_t::notify_callback(const bt_err_t &bt_err, const bt_cast_mdata_t &cast_mdata)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_casto_itor_cb(userptr, *this, bt_err, cast_mdata);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END;




