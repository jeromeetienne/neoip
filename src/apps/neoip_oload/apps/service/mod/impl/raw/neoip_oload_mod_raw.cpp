/*! \file
    \brief Class to handle the oload_mod_raw_t

- TODO to comment

*/

/* system include */
/* local include */
#include "neoip_oload_mod_raw.hpp"
#include "neoip_oload_apps.hpp"
#include "neoip_bt_httpo_listener.hpp"
#include "neoip_bt_httpo_resp.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_event.hpp"
#include "neoip_http_resp_mode.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
oload_mod_raw_t::oload_mod_raw_t()	throw()
{
	// zero some variable
	m_oload_apps	= NULL;
	m_httpo_resp	= NULL;
	m_type		= oload_mod_type_t::RAW;
}

/** \brief Destructor
 */
oload_mod_raw_t::~oload_mod_raw_t()		throw()
{
	// unlink this object from the attached oload_apps_t
	if( m_oload_apps )	oload_apps()->mod_unlink(this);
	// delete the bt_httpo_resp_t if needed
	nipmem_zdelete	m_httpo_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Setup function 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	oload_mod_raw_t::start(oload_apps_t *m_oload_apps)	throw()
{
	bt_err_t	bt_err;
	// copy the parameters
	this->m_oload_apps	= m_oload_apps;
	// link this object to the attached oload_apps_t
	oload_apps()->mod_dolink(this);

	// start the bt_httpo_resp_t
	m_httpo_resp	= nipmem_new bt_httpo_resp_t();
	bt_err		= m_httpo_resp->start(oload_apps()->httpo_listener(), "http://0.0.0.0"
					, http_resp_mode_t::ACCEPT_SUBPATH, this, NULL);
	if( bt_err.failed() )	return bt_err;
	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    bt_httpo_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
			
/** \brief callback notified by \ref bt_httpo_resp_t when to notify an event
 */
bool	oload_mod_raw_t::neoip_bt_httpo_resp_cb(void *cb_userptr, bt_httpo_resp_t &cb_bt_httpo_resp
						, const bt_httpo_event_t &httpo_event)	throw()
{
	// log to debug
	KLOG_ERR("enter httpo_event=" << httpo_event);

	// sanity check - the bt_httpo_event_t MUST BE is_resp_ok()
	DBG_ASSERT( httpo_event.is_resp_ok() );
	// sanity check - the bt_httpo_event_t MUST BE a CNX_ESTABLISHED
	DBG_ASSERT( httpo_event.is_cnx_established() );

	// get variable from the bt_httpo_full_t
	bt_httpo_full_t *	httpo_full	= httpo_event.get_cnx_established();

	// put the oload_mod_vapi_t pointer in the userptr
	httpo_full->userptr(this);
	
	// call the oload_apps processing for post etablishement
	oload_apps()->httpo_full_post_esta(httpo_full);
	
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END





