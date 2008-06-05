/*! \file
    \brief Definition of the \ref bt_oload0_torr_t

\par Brief Description
\ref bt_oload0_torr_t is a bt_oload0_t handler which receives a http_uri_t pointing
as a .torrent file. It download the .torrent file via bt_oload0_torr_itor_t
and then start downloading on the swarm and forward it to all the attached
bt_httpo_full_t.


*/

/* system include */
/* local include */
#include "neoip_bt_oload0_torr.hpp"
#include "neoip_bt_oload0_torr_itor.hpp"
#include "neoip_bt_oload0_swarm.hpp"
#include "neoip_bt_oload0.hpp"
#include "neoip_bt_httpo_resp.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_event.hpp"
#include "neoip_http_resp_mode.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_nested_uri.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
bt_oload0_torr_t::bt_oload0_torr_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	bt_oload0	= NULL;
	httpo_resp	= NULL;
}

/** \brief Destructor
 */
bt_oload0_torr_t::~bt_oload0_torr_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all the torr_itor_db if needed
	while( !torr_itor_db.empty() )	nipmem_delete torr_itor_db.front();
	// delete the bt_httpo_resp_t if needed
	nipmem_zdelete	httpo_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
bt_oload0_torr_t &bt_oload0_torr_t::set_profile(const bt_oload0_torr_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_oload0_torr_t::start(bt_oload0_t *bt_oload0)			throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->bt_oload0	= bt_oload0;
	
	// start the bt_httpo_resp_t
	httpo_resp	= nipmem_new bt_httpo_resp_t();
	bt_err		= httpo_resp->start(bt_oload0->get_bt_httpo(), "http://0.0.0.0/torrent"
					, http_resp_mode_t::ACCEPT_SUBPATH, this, NULL);
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer to a bt_oload0_torr_itor_t matching the nested_uri, or NULL if none matches
 */
bt_oload0_torr_itor_t *	bt_oload0_torr_t::itor_by_nested_uri(const http_uri_t &nested_uri) const throw()
{
	std::list<bt_oload0_torr_itor_t *>::const_iterator	iter;
	// go thru the whole torr_itor_db
	for(iter = torr_itor_db.begin(); iter != torr_itor_db.end(); iter++){
		bt_oload0_torr_itor_t *	stat_itor	= *iter;
		// if this bt_oload0_torr_itor_t matches this nested_uri, return its pointer
		if( stat_itor->get_nested_uri() == nested_uri )	return stat_itor;
	}
	// if this point is reached, no matches have been found
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    bt_httpo_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
			
/** \brief callback notified by \ref bt_httpo_resp_t when to notify an event
 */
bool	bt_oload0_torr_t::neoip_bt_httpo_resp_cb(void *cb_userptr, bt_httpo_resp_t &cb_bt_httpo_resp
						, const bt_httpo_event_t &httpo_event)	throw()
{
	bt_oload0_swarm_t *	oload0_swarm;
	bt_oload0_torr_itor_t *	stat_itor;
	// log to debug
	KLOG_WARN("enter httpo_event=" << httpo_event);
	// sanity check - the bt_httpo_event_t MUST BE is_resp_ok()
	DBG_ASSERT( httpo_event.is_resp_ok() );

	// sanity check - the bt_httpo_event_t MUST BE a CNX_ESTABLISHED
	DBG_ASSERT( httpo_event.is_cnx_established() );

	// get variable from the bt_httpo_full_t
	bt_httpo_full_t *	httpo_full	= httpo_event.get_cnx_established();
	const http_reqhd_t &	http_reqhd	= httpo_full->get_http_reqhd();
	file_range_t		req_range	= http_reqhd.range();
	const http_uri_t &	nested_uri	= http_reqhd.uri();
	http_uri_t		outter_uri	= http_nested_uri_t::parse_outter(nested_uri);
	http_uri_t		inner_uri	= http_nested_uri_t::parse_inner(nested_uri);

	// log to debug
	KLOG_WARN("nested_uri="	<< nested_uri);
	KLOG_WARN("outter_uri="	<< outter_uri);
	KLOG_WARN("inner_uri="	<< inner_uri);
	KLOG_WARN("http_reqhd="	<< http_reqhd);
	KLOG_WARN("file_range="	<< req_range);
	
	// if the nested_uri is invalid, delete the bt_httpo_full_t and exit
	if( !http_nested_uri_t::is_valid_nested(nested_uri) ){
		nipmem_delete	httpo_full;
		return true;
	}

	// try to find bt_oload0_swarm_t matchin the nested_uri
	oload0_swarm	= bt_oload0->swarm_by_nested_uri(nested_uri);
	// if there is a matching bt_oload0_swarm_t, add this bt_httpo_full_t to it
	if( oload0_swarm ){
		oload0_swarm->add_httpo_full(httpo_full);
		return true;
	}

	// try to find bt_stat_itor_t matchin the nested_uri
	stat_itor	= itor_by_nested_uri(nested_uri);
	// if there is a matching bt_stat_itor_t, add this bt_httpo_full_t to it
	if( stat_itor ){
		stat_itor->add_httpo_full(httpo_full);
		return true;
	}
	
	// if no swarm or itor match this nested_uri, create a bt_stat_itor_t for it
	bt_err_t	bt_err;
	stat_itor	= nipmem_new bt_oload0_torr_itor_t();
	bt_err		= stat_itor->start(this, nested_uri, httpo_full);
	if( bt_err.failed() )	nipmem_delete stat_itor;

	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END;




