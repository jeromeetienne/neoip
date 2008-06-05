/*! \file
    \brief Definition of the \ref btrelay_itor_t
    
\par Brief Description
btrelay_itor_t handles the convertion from the link_addr/link_type to a bt_mfile_t.

*/

/* system include */
/* local include */
#include "neoip_btrelay_itor.hpp"
#include "neoip_btrelay_apps.hpp"

#include "neoip_bt_lnk2mfile.hpp"

#include "neoip_bt_cast_helper.hpp"
#include "neoip_bt_cast_mdata_client.hpp"
#include "neoip_bt_cast_mdata.hpp"

#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
btrelay_itor_t::btrelay_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	bt_lnk2mfile	= NULL;
	mdata_client	= NULL;
}

/** \brief Destructor
 */
btrelay_itor_t::~btrelay_itor_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// unlink this object from the bt_btrelay_apps_t
	if( btrelay_apps )	btrelay_apps->itor_unlink(this);
	// delete the bt_lnk2mfile_t if needed
	nipmem_zdelete	bt_lnk2mfile;
	// delete the bt_cast_mdata_client_t if needed
	nipmem_zdelete	mdata_client;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t btrelay_itor_t::start(btrelay_apps_t *btrelay_apps, const std::string &link_type
					, const std::string &link_addr
					, btrelay_itor_cb_t *callback, void *userptr)	throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_WARN("enter");
	// copy the parameter
	this->btrelay_apps	= btrelay_apps;
	this->callback		= callback;
	this->userptr		= userptr;
	// link this object to the bt_btrelay_apps_t
	btrelay_apps->itor_dolink(this);

	// start the bt_cast_mdata_client_t
	// - TODO server_uri is hardcoded - find better - same issue in casto_swarm_t
	// - TODO additionnaly it ignored the link_addr	
	if( link_type == "cast" ){
		http_uri_t	server_uri	= "http://jmehost1:9080/cast_mdata";
		mdata_client	= nipmem_new bt_cast_mdata_client_t();
		bt_err		= mdata_client->start(server_uri, "wowstuff", this, NULL);
		if( bt_err.failed() )	return bt_err;
		return bt_err_t::OK;
	}

	// start the bt_lnk2mfile_t
	bt_lnk2mfile	= nipmem_new bt_lnk2mfile_t();
	bt_err		= bt_lnk2mfile->start(link_type, link_addr, this, NULL);
	if( bt_err.failed() )	return bt_err;
	// return no error
	return bt_err_t::OK;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_lnk2mfile_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_lnk2mfile_t to provide event
 */
bool	btrelay_itor_t::neoip_bt_lnk2mfile_cb(void *cb_userptr, bt_lnk2mfile_t &cb_bt_lnk2mfile
				, const bt_err_t &bt_err, const bt_lnk2mfile_type_t &lnk2mfile_type
				, const bt_mfile_t &bt_mfile)		throw() 
{
	// simply forward the result to the caller
	// - NOTE: up to it to start a btrelay_swarm_t and to have
	//   the bt_httpo_full_t attached to it
	return notify_callback(bt_err, bt_mfile, bt_cast_mdata_t());
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_cast_mdata_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_cast_mdata_client_t to provide event
 */
bool	btrelay_itor_t::neoip_bt_cast_mdata_client_cb(void *cb_userptr
					, bt_cast_mdata_client_t &cb_mdata_client
					, const bt_err_t &cb_bt_err
					, const bt_cast_mdata_t &cast_mdata)	throw()
{
	bt_err_t	bt_err	= cb_bt_err;
	// log to debug
	KLOG_ERR("enter bt_err=" << bt_err << " cast_mdata=" << cast_mdata);
	
	// test if the result if the supposed one
	if( bt_err.failed() )	return	notify_callback_failure(bt_err);
	// sanity check - if the bt_cast_mdata_client_t succeed, cast_mdata MUST NOT be null
	DBG_ASSERT( !cast_mdata.is_null() );

	// build the bt_mfile_t for this bt_cast_mdata_t
	bt_mfile_t	bt_mfile;
	bt_mfile	= bt_cast_helper_t::build_mfile(cast_mdata);
	DBG_ASSERT( bt_mfile.is_fully_init() );

	// notify the bt_mfile_t
	bool	tokeep	= notify_callback_success(bt_mfile, cast_mdata);
	if( !tokeep )	return false;
	
	// delete the bt_cast_mdata_client_t
	nipmem_zdelete	mdata_client;	
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief helper to notify the callback when there is a faillure
 */
bool	btrelay_itor_t::notify_callback_failure(const bt_err_t &bt_err)		throw()
{
	return notify_callback( bt_err, bt_mfile_t(), bt_cast_mdata_t() );
}

/** \brief helper to notify the callback when there is a success
 */
bool	btrelay_itor_t::notify_callback_success(const bt_mfile_t &bt_mfile
					, const bt_cast_mdata_t &cast_mdata)	throw()
{
	return notify_callback(bt_err_t::OK, bt_mfile, cast_mdata);
}

/** \brief notify the callback with the tcp_event
 */
bool btrelay_itor_t::notify_callback(const bt_err_t &bt_err, const bt_mfile_t &bt_mfile
				, const bt_cast_mdata_t &cast_mdata)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_btrelay_itor_cb(userptr, *this, bt_err, bt_mfile, cast_mdata);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END;




