/*! \file
    \brief Definition of the \ref bt_cast_mdata_dopublish_t

\par Brief Description
bt_cast_mdata_dopublish_t publish information about a bt_cast_mdata_t provider
to a remote server, called the cast_mdata_echo_server

\par TODO
- BUG it doesnt handle the callback returning a bt_cast_mdata_t::is_null()
  - this case is rare BUT happen

\par Type of publication
- there is 2 type of publications:
- "pull" when cast_mdata_echo_server store only the address of the local apps
  - when another apps requests the bt_cast_mdata_t, cast_mdata_echo_server
    contact the locate apps to get the bt_cast_mdata_t and then forward it to the
    original requesters
  - it is periodically refreshed in cast_mdata_echo_server in order to be able
    to detect stale registration
  - IMPORTANT: the local apps MUST be reachable by the cast_echo_server
- "push" when cast_mdata_echo_server store the whole bt_cast_mdata_t of this apps
  - when another apps requests it, cast_mdata_echo_server directly serves it
    without contacting this apps
  - so the local apps need to update very frequently the bt_cast_mdata_t in
    cast_mdata_echo_server
    - quite frequently is like once every 10-sec minimum
    - EVEN if there is not query on this bt_cast_mdata_t. waste of bandwidth

\par Autodetermination of m_publish_type
- "pull" is definitly better from a network efficiency point of view
  - but it requires the local apps to be reachable by casti_mdata_echo_server
  - it may not be the case (firewall, no available upnp router)
- so it need to be able to fallback on "push" if the reachability requirement
  is not met.
1. start a set_cast_mdata_pull
2. on set_cast_mdata_pull completion, start a get_cast_mdata
3. if get_cast_mdata succeed, the local apps IS reachable by cast_mdata_echo_server
   - so publish_type == "pull" and start the periodic_set with low frequency
3. if get_cast_mdata failed, the local apps IS NOT reachable by cast_mdata_echo_server
   - so publish_type == "push" and start the periodic_set with high frequency


*/

/* system include */
/* local include */
#include "neoip_bt_cast_mdata_dopublish.hpp"
#include "neoip_casti_swarm.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_xmlrpc_client.hpp"
#include "neoip_xmlrpc_client_helper.hpp"
#include "neoip_xmlrpc_err.hpp"

#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_cast_mdata_dopublish_t::bt_cast_mdata_dopublish_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_published	= false;
	m_probe_set	= NULL;
	m_probe_get	= NULL;
	m_periodic_set	= NULL;
}

/** \brief Destructor
 */
bt_cast_mdata_dopublish_t::~bt_cast_mdata_dopublish_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete m_probe_set if needed
	nipmem_zdelete	m_probe_set;
	// delete m_probe_get if needed
	nipmem_zdelete	m_probe_get;
	// delete m_periodic_set if needed
	nipmem_zdelete	m_periodic_set;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
bt_cast_mdata_dopublish_t &	bt_cast_mdata_dopublish_t::profile(const bt_cast_mdata_dopublish_profile_t &m_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( m_profile.check() == bt_err_t::OK );
	// copy the parameter
	this->m_profile	= m_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 *
 * - NOTE: just copy the data, the publication is not started
 *   before bt_cast_mdata_dopublish_t::start_publishing() is called
 */
bt_err_t bt_cast_mdata_dopublish_t::start(const http_uri_t &m_mdata_srv_uri
			, const std::string &m_cast_privtext
			, bt_cast_mdata_dopublish_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameter
	this->m_mdata_srv_uri	= m_mdata_srv_uri;
	this->m_cast_privtext	= m_cast_privtext;
	this->callback		= callback;
	this->userptr		= userptr;

	// determine the initial m_publish_type
	// - if set pull is possible, try it first
	// - if the attemps fails, fall back on "push" later
	if( may_set_pull() )	m_publish_type	= "pull";
	else			m_publish_type	= "push";
	// start the m_periodic_timeout
	m_periodic_timeout.start(delay_t::from_sec(0), this, NULL);

	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief return true if it is possible to do "set_cast_mdata_pull" xmlrpc, false otherwise
 */
bool	bt_cast_mdata_dopublish_t::may_set_pull()	throw()
{
	uint16_t	port_pview	= 0;
	// get the data which may be published from the callback
	bool	tokeep	= notify_callback(NULL, NULL, &port_pview, NULL);
	DBG_ASSERT( tokeep == true );	// NOTE: in fact it has tokeep for regularity only :)

	// if the port_pview is not null, "pull" is possible, so return true
	if( port_pview )	return true;
	// else return false;
	return false;
}

/** \brief Return the periodic_delay depending on the m_publish_type
 */
const delay_t &	bt_cast_mdata_dopublish_t::periodic_delay()	const throw()
{
	// sanity check - m_publish_type MUST have been set
	DBG_ASSERT( m_publish_type == "pull" || m_publish_type == "push" );

	// return the periodic_delay from the profile depending on m_publish_type
	if( m_publish_type == "pull" )	return m_profile.periodic_delay_pull();
	if( m_publish_type == "push" )	return m_profile.periodic_delay_push();

	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return m_profile.periodic_delay_push();	// NOTE: just to avoid a compiler warning
}

/** \brief build a xmlrpc call for a set_cast_mdata_pull/set_cast_mdata_push depending
 *         on the type passed as parameter
 */
datum_t bt_cast_mdata_dopublish_t::build_call_set_mdata(const std::string &type)	throw()
{
	// init the datas to get from the callback
	bt_cast_mdata_t	cast_mdata;
	uint16_t	port_lview	= 0;
	uint16_t	port_pview	= 0;
	std::string	casti_uri_pathquery;
	// get the data which may be published from the callback
	bool	tokeep	= notify_callback(&cast_mdata, &port_lview, &port_pview
							, &casti_uri_pathquery);
	DBG_ASSERT( tokeep == true );	// NOTE: in fact it has tokeep for regularity only :)
	if( !tokeep )	return datum_t();

	// sanity check - cast_mdata MUST have been provided
	DBG_ASSERT( !cast_mdata.is_null() );
	DBG_ASSERT( port_lview != 0 );
	DBG_ASSERT( casti_uri_pathquery.empty() == false );
	// sanity check - if "pull" then port_pview MUST NOT be 0
	if( type == "pull" )	DBG_ASSERT( port_pview );

	// log to debug
	KLOG_ERR("port_lview="		<< port_lview);
	KLOG_ERR("port_pview="		<< port_pview);
	KLOG_ERR("casti_uri_pathquery=" << casti_uri_pathquery);

	// build the call_datum
	xmlrpc_build_t	xmlrpc_build;
	if( type == "pull" ){
		xmlrpc_build << xmlrpc_build_t::CALL_BEG("set_cast_mdata_pull");
		xmlrpc_build << xmlrpc_build_t::PARAM_BEG << cast_mdata.cast_name()	<< xmlrpc_build_t::PARAM_END;
		xmlrpc_build << xmlrpc_build_t::PARAM_BEG << m_cast_privtext		<< xmlrpc_build_t::PARAM_END;
		xmlrpc_build << xmlrpc_build_t::PARAM_BEG << cast_mdata.cast_id()	<< xmlrpc_build_t::PARAM_END;
		xmlrpc_build << xmlrpc_build_t::PARAM_BEG << port_lview			<< xmlrpc_build_t::PARAM_END;
		xmlrpc_build << xmlrpc_build_t::PARAM_BEG << port_pview			<< xmlrpc_build_t::PARAM_END;
		xmlrpc_build << xmlrpc_build_t::PARAM_BEG << casti_uri_pathquery	<< xmlrpc_build_t::PARAM_END;
		xmlrpc_build << xmlrpc_build_t::CALL_END;
	}else{
		xmlrpc_build << xmlrpc_build_t::CALL_BEG("set_cast_mdata_push");
		xmlrpc_build << xmlrpc_build_t::PARAM_BEG << cast_mdata.cast_name()	<< xmlrpc_build_t::PARAM_END;
		xmlrpc_build << xmlrpc_build_t::PARAM_BEG << m_cast_privtext		<< xmlrpc_build_t::PARAM_END;
		xmlrpc_build << xmlrpc_build_t::PARAM_BEG << cast_mdata			<< xmlrpc_build_t::PARAM_END;
		xmlrpc_build << xmlrpc_build_t::CALL_END;
	}
	// return the datum_t containing the just built xmlrpc call
	return xmlrpc_build.to_datum();
}

/** \brief build a xmlrpc call for a get_cast_mdata
 */
datum_t bt_cast_mdata_dopublish_t::build_call_get_mdata(const std::string &cast_privhash)	throw()
{
	// init the datas to get from the callback
	bt_cast_mdata_t	cast_mdata;
	// get the data which may be published from the callback
	bool	tokeep	= notify_callback(&cast_mdata, NULL, NULL, NULL);
	DBG_ASSERT( tokeep == true );	// NOTE: in fact it has tokeep for regularity only :)
	if( !tokeep )	return datum_t();

	// sanity check - cast_mdata MUST have been provided
	DBG_ASSERT( !cast_mdata.is_null() );

	// build the call_datum
	xmlrpc_build_t	xmlrpc_build;
	xmlrpc_build << xmlrpc_build_t::CALL_BEG("get_cast_mdata");
	xmlrpc_build << xmlrpc_build_t::PARAM_BEG << cast_mdata.cast_name()	<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build << xmlrpc_build_t::PARAM_BEG << cast_privhash		<< xmlrpc_build_t::PARAM_END;
	xmlrpc_build << xmlrpc_build_t::CALL_END;

	// return the datum_t containing the just built xmlrpc call
	return xmlrpc_build.to_datum();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     xmlrpc_client_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notifying the result of xmlrpc_client_t
 */
bool	bt_cast_mdata_dopublish_t::neoip_xmlrpc_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &cb_xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw()
{
	// forward the callback to the proper subhandler
	if( m_probe_set == &cb_xmlrpc_client )	return probe_set_client_cb(cb_userptr, cb_xmlrpc_client, cb_xmlrpc_err, xmlrpc_resp);
	if( m_probe_get == &cb_xmlrpc_client )	return probe_get_client_cb(cb_userptr, cb_xmlrpc_client, cb_xmlrpc_err, xmlrpc_resp);
	if( m_periodic_set== &cb_xmlrpc_client)	return periodic_set_client_cb(cb_userptr, cb_xmlrpc_client, cb_xmlrpc_err, xmlrpc_resp);
	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     m_probe_set callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notifying the result of m_probe_set
 */
bool	bt_cast_mdata_dopublish_t::probe_set_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &cb_xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw()
{
	xmlrpc_err_t	xmlrpc_err;
	// sanity check - m_publish_type MUST NOT be set
	DBG_ASSERT( m_publish_type.empty() );
	// delete the xmlrpc_client_t
	nipmem_zdelete	m_probe_set;

	// test if the xmlrpc_client_t succeed
	xmlrpc_err	= xmlrpc_client_helper_t::has_succeed(cb_xmlrpc_err, xmlrpc_resp);
	// parse the xmlrpc response to get the m_cast_privhash
	if( xmlrpc_err.succeed() ){
		try {
			// set the document for xmlrpc_parse_t
			xmlrpc_parse_t	xmlrpc_parse;
			xmlrpc_parse.set_document(xmlrpc_resp);
			// passwd the reply
			xmlrpc_parse >> xmlrpc_parse_t::RESP_BEG;
			xmlrpc_parse	>> m_cast_privhash;
			xmlrpc_parse >> xmlrpc_parse_t::RESP_END;
		} catch(xml_except_t &e) {
			xmlrpc_err	= xmlrpc_err_t(xmlrpc_err_t::ERROR, "unable to parse response as m_cast_privhash due to " + e.what());
		}
	}
	// if xmlrpc_client_t failed, log the event
	if( xmlrpc_err.failed() ){
		KLOG_ERR("probe_set failed due to " << xmlrpc_err);
		// m_publish_type default to "push"
		m_publish_type	= "push";
		// start m_perodic_timeout_t
		m_periodic_timeout.start(delay_t::from_sec(0), this, NULL);
		// return dont keep
		return false;
	}


	// NOTE: here the set_cast_mdata_pull succeed, so now launch a get_cast_mdata to test
	// if this neoip-casti is reachable by the cast_mdata_echo_server

	// get the datum_t of the xmlrpc_call
	datum_t	xmlrpc_call	= build_call_get_mdata(m_cast_privhash);
	// start the xmlrpc_client_t
	m_probe_get	= nipmem_new xmlrpc_client_t();
	xmlrpc_err	= m_probe_get->start(m_mdata_srv_uri, xmlrpc_call, this, NULL);
	DBG_ASSERT( xmlrpc_err.succeed() );

	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     m_probe_get callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notifying the result of m_probe_get
 */
bool	bt_cast_mdata_dopublish_t::probe_get_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &cb_xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw()
{
	xmlrpc_err_t	xmlrpc_err;
	// sanity check - m_publish_type MUST NOT be set
	DBG_ASSERT( m_publish_type.empty() );
	// delete the xmlrpc_client_t
	nipmem_zdelete	m_probe_get;

	// test if the xmlrpc_client_t succeed
	xmlrpc_err	= xmlrpc_client_helper_t::has_succeed(cb_xmlrpc_err, xmlrpc_resp);
	// if get_cast_mdata failed,
	if( xmlrpc_err.failed() ){
		// set the m_publish_type to "pull"
		m_publish_type	= "push";
		// start m_perodic_timeout_t - immediatly to overwrite the not-working pull
		m_periodic_timeout.start(delay_t::from_sec(0), this, NULL);
	}else{
		// mark the bt_cast_mdata_t as published
		m_published	= true;
		// set the m_publish_type to "pull"
		m_publish_type	= "pull";
		// start m_perodic_timeout_t - after normal delay as it doesnt need to overwrite
		m_periodic_timeout.start(periodic_delay(), this, NULL);
	}

	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			m_periodic_timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool bt_cast_mdata_dopublish_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// sanity check - m_publish_type MUST have been set
	DBG_ASSERT( m_publish_type == "pull" || m_publish_type == "push" );
	// sanity check - m_periodic_set MUST NOT be running
	DBG_ASSERT( m_periodic_set == NULL );

	// stop the timeout_t
	m_periodic_timeout.stop();

	// get the datum_t of the xmlrpc_call
	datum_t	xmlrpc_call	= build_call_set_mdata(m_publish_type);

	// log to debug
	KLOG_ERR("xmlrpc_call=" << xmlrpc_call.to_stdstring());

	// start the xmlrpc_client_t
	xmlrpc_err_t	xmlrpc_err;
	m_periodic_set	= nipmem_new xmlrpc_client_t();
	xmlrpc_err	= m_periodic_set->start(m_mdata_srv_uri, xmlrpc_call, this, NULL);
	DBG_ASSERT( xmlrpc_err.succeed() );

	// return tokeep
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     m_periodic_set callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notifying the result of m_periodic_set
 */
bool	bt_cast_mdata_dopublish_t::periodic_set_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &cb_xmlrpc_err
					, const datum_t &xmlrpc_resp)		throw()
{
	xmlrpc_err_t	xmlrpc_err;
	// sanity check - m_periodic_timeout MUST NOT be running
	DBG_ASSERT( !m_periodic_timeout.is_running() );

	// test if the xmlrpc_client_t succeed
	xmlrpc_err	= xmlrpc_client_helper_t::has_succeed(cb_xmlrpc_err, xmlrpc_resp);
	// if xmlrpc_client_t failed, log the event
	if( xmlrpc_err.failed() )	KLOG_ERR("publishing failed due to " << xmlrpc_err);


	// parse the xmlrpc response to get the m_cast_privhash
	if( xmlrpc_err.succeed() ){
		try {
			// set the document for xmlrpc_parse_t
			xmlrpc_parse_t	xmlrpc_parse;
			xmlrpc_parse.set_document(xmlrpc_resp);
			// passwd the reply
			xmlrpc_parse >> xmlrpc_parse_t::RESP_BEG;
			xmlrpc_parse	>> m_cast_privhash;
			xmlrpc_parse >> xmlrpc_parse_t::RESP_END;
		} catch(xml_except_t &e) {
			xmlrpc_err	= xmlrpc_err_t(xmlrpc_err_t::ERROR, "unable to parse response as m_cast_privhash due to " + e.what());
		}
	}

	// mark that the m_published according to the success of the xmlrpc_client_t
	m_published	= xmlrpc_err.succeed() ? true : false;



	// delete the xmlrpc_client_t
	nipmem_zdelete	m_periodic_set;

	// restart the timeout_t
	m_periodic_timeout.start(periodic_delay(), this, NULL);

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the a whole lot of parameters to return :)
 */
bool bt_cast_mdata_dopublish_t::notify_callback(bt_cast_mdata_t *cast_mdata_out
					, uint16_t *port_lview_out, uint16_t *port_pview_out
					, std::string *casti_uri_pathquery_out)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_cast_mdata_dopublish_cb(userptr, *this
					, cast_mdata_out, port_lview_out, port_pview_out
					, casti_uri_pathquery_out);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}
NEOIP_NAMESPACE_END;




