/*! \file
    \brief Definition of the \ref kad_xmlrpc_xmlrpc_cmd_t class

\par Brief Description
This implement the remote command via xmlrpc for the NeoIP Kademlia.

*/

/* system include */
/* local include */
#include "neoip_kad_xmlrpc_cmd.hpp"
#include "neoip_kad_xmlrpc_sess.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_kad_listener.hpp"
#include "neoip_kad_caddr_arr.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_httpd.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_xmlrpc_t::xmlrpc_cmd_t::xmlrpc_cmd_t(kad_xmlrpc_t *kad_xmlrpc)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// copy some parameter
	this->kad_xmlrpc	= kad_xmlrpc;
	// set the slot_id
	async_cmdid		= slotpool_t::NONE;
	async_peer_slotid	= slotpool_t::NONE;
	// link itself to kad_xmlrpc_t
	kad_xmlrpc->cmd_link(this);
}

/** \brief Destructor
 */
kad_xmlrpc_t::xmlrpc_cmd_t::~xmlrpc_cmd_t()			throw()
{
	// log to debug
	KLOG_ERR("enter");
	// TODO what about a potential http cnx in delayed reply
	// BUG BUG BUG BUG
	// - this http cnx MUST BE deleted
	
	// stop the current peer_cmd if needed - only used for async xmlrpc
	if( async_cmdid != slotpool_t::NONE )
		get_kad_listener().stop_command(async_cmdid, async_peer_slotid);
	// unlink itself from kad_xmlrpc_t
	kad_xmlrpc->cmd_unlink(this);
}

/** \brief return the peer_slotid from this session_slotid
 * 
 * - notify the session a new command occurs on this session
 */
slot_id_t kad_xmlrpc_t::xmlrpc_cmd_t::get_peer_slotid_from_session(const slot_id_t &sess_slotid)
										throw()
{
	// try to find the session matching this session_slotid
	xmlrpc_sess_t *	xmlrpc_sess = (xmlrpc_sess_t *)kad_xmlrpc->sess_slotpool.find(sess_slotid);
	// if none is found, return slotpool_t::NONE
	if( !xmlrpc_sess )	return slotpool_t::NONE;
	// else return the peer_slotid
	return xmlrpc_sess->get_peer_slotid();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              START function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start The operation
 * 
 * - this may delete the xmlrpc_cmd_t
 */
httpd_err_t kad_xmlrpc_t::xmlrpc_cmd_t::start(httpd_request_t &request)	throw()
{
	xmlrpc_parse_t	xmlrpc_parse;
	kad_err_t	kad_err;

	// parse the xml document
	if( xmlrpc_parse.set_document(request.get_posted_data()) ){
		// build the fault in the reply
		xmlrpc_build_t	xmlrpc_build;
		xmlrpc_build.put_fault(-1, "Cant Parse the XML of this XMLRPC message");
		// put the reply into the httpd reply immediatly
		request.get_reply() << string_t::from_datum(xmlrpc_build.get_xml_output());
		// autodelete
		nipmem_delete	this;
		// return no error in http
		// - from the spec 'Unless there's a lower-level error, always return 200 OK."
		//   what is lower-level error isnt specified, so i assumed it is http and lower
		return httpd_err_t::OK;
	}
	
	// copy the called method
	method	= xmlrpc_parse.get_call_method();
	// log to debug
	KLOG_ERR("xmlrpc_parse method=" << xmlrpc_parse.get_call_method() );
	
	// if the method is session_start() or session_stop(), process them in sync
	if( method == "dht.session_start" || method == "dht.session_stop" )
		return start_sync(xmlrpc_parse, request);
	// for all the others, process them async
	return start_async(xmlrpc_parse, request);
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start for all sync command
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief parse the command from the xml and call parsers specific to each command
 */
httpd_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::start_sync(xmlrpc_parse_t &xmlrpc_parse
						, httpd_request_t &request)	throw()
{
	kad_err_t	kad_err;
	xmlrpc_build_t	xmlrpc_build;
	slot_id_t	sess_slotid;
	try {
		// call the parser specific to each method
		if(method == "dht.session_start"){
			kad_err = parse_session_start(xmlrpc_parse, &sess_slotid);
		}else if(method == "dht.session_stop"){
			kad_err = parse_session_stop(xmlrpc_parse);
		}else{	
			kad_err = kad_err_t(kad_err_t::ERROR, "Unknown method " + method);
		}
	}catch(xml_except_t &e){
		KLOG_ERR("Unable to parse a xmlrpc request due to " << e.what());
		kad_err = kad_err_t(kad_err_t::ERROR, e.what());
	}

 	// if the command initialization succeed, http response is delayed until the command completion
	if( kad_err.failed() ){
		// build the fault in the reply
		xmlrpc_build.put_fault(-1, "Error while executing command due to " + kad_err.to_string());
	}else{
		// setup the xmlrpc_build in a methodResponse
		xmlrpc_build.start_resp();

		// a sucessfull xmlrpc response contains a single parameter - it starts here
		xmlrpc_build << xmlrpc_build_t::PARAM_BEGIN;
		if(method == "dht.session_start")	xmlrpc_build	<< sess_slotid;
		else					xmlrpc_build	<< int32_t(0);
		xmlrpc_build << xmlrpc_build_t::PARAM_END;
	}
	// log to debug
	KLOG_ERR("reply=" << xmlrpc_build.get_xml_output());
	// put the reply into the httpd reply immediatly
	request.get_reply() << string_t::from_datum(xmlrpc_build.get_xml_output());
	// autodelete
	nipmem_delete	this;
	// return no error in http
	return httpd_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    MethodCall parsing for each sync command
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Parse a session_start
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_session_start(xmlrpc_parse_t &xmlrpc_parse
					, slot_id_t *sess_slotid_out)		throw(xml_except_t)
{
	kad_realmid_t	realmid;
	kad_peerid_t	peerid;
	kad_profile_t	profile;
	
	// log to debug
	KLOG_ERR("enter");

	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> realmid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> peerid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> profile	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;
		
	// create a new session for this realm
	xmlrpc_sess_t	*xmlrpc_sess	= nipmem_new xmlrpc_sess_t(kad_xmlrpc);
	kad_err_t	kad_err		= xmlrpc_sess->start(realmid, peerid, profile);
	if( kad_err.failed() )	return kad_err;
	// copy the session id
	*sess_slotid_out	= xmlrpc_sess->get_sess_slotid();

/* TODO
 * - this function should return an array with:
 *   - sess_slotid
 *   - realmid
 *   - peerid
 *   - profile
 * - thus the caller could get info from the realm once opened
 * - or i do another RPC, like get_session_info(sess_slotid) which return 
 *   - realmid
 *   - peerid
 *   - profile
 * - the second seems more sensible as it allows the caller to query the session
 *   at any time when it is opened without having to keep the data around
 */
	// return no error
	return kad_err_t::OK;
}

/** \brief Parse session_stop
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_session_stop(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	slot_id_t	sess_slotid;	
	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> sess_slotid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;	

	// try to find the session of this
	xmlrpc_sess_t *	xmlrpc_sess = (xmlrpc_sess_t *)kad_xmlrpc->sess_slotpool.find( sess_slotid );
	if( !xmlrpc_sess )	return kad_err_t(kad_err_t::ERROR, "No session matching this slot_id");
	// delete the session
	nipmem_delete xmlrpc_sess;

	// return no error
	return kad_err_t::OK;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start for all async command
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief parse the command from the xml and call parsers specific to each command
 */
httpd_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::start_async(xmlrpc_parse_t &xmlrpc_parse
						, httpd_request_t &request)	throw()
{
	kad_err_t	kad_err;

	try {
		// call the parser specific to each method
		if(method == "dht.publish"){			kad_err = parse_publish(xmlrpc_parse);
		}else if(method == "dht.delete"){		kad_err = parse_delete(xmlrpc_parse);
		}else if(method == "dht.getsome"){		kad_err = parse_getsome(xmlrpc_parse);
		}else if(method == "dht.getall"){		kad_err = parse_getall(xmlrpc_parse);
		}else if(method == "dht.nsearch"){		kad_err = parse_nsearch(xmlrpc_parse);
		}else if(method == "dht.ping_rpc"){		kad_err = parse_ping_rpc(xmlrpc_parse);
		}else if(method == "dht.store_rpc"){		kad_err = parse_store_rpc(xmlrpc_parse);
		}else if(method == "dht.findnode_rpc"){		kad_err = parse_findnode_rpc(xmlrpc_parse);
		}else if(method == "dht.findsomeval_rpc"){	kad_err = parse_findsomeval_rpc(xmlrpc_parse);
		}else if(method == "dht.findallval_rpc"){	kad_err = parse_findallval_rpc(xmlrpc_parse);
		}else if(method == "dht.delete_rpc"){		kad_err = parse_delete_rpc(xmlrpc_parse);
		}else{		kad_err = kad_err_t(kad_err_t::ERROR, "Unknown method " + method);
		}
	}catch(xml_except_t &e){
		KLOG_ERR("Unable to parse a xmlrpc request due to " << e.what());
		kad_err = kad_err_t(kad_err_t::ERROR, e.what());
	}

 	// if the command initialization succeed, http response is delayed until the command completion
	if( !kad_err.failed() ){
		// copy the request parameter to use it during the reply
		async_request	= request;
		return httpd_err_t::DELAYED_REPLY;
	}

	// build the fault in the reply
	xmlrpc_build_t	xmlrpc_build;
	xmlrpc_build.put_fault(-1, "Error while initializing command due to " + kad_err.to_string());
	// put the reply into the httpd reply immediatly
	request.get_reply() << string_t::from_datum(xmlrpc_build.get_xml_output());
	// autodelete
	nipmem_delete	this;
	// return no error in http
	return httpd_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//              methodCall parsing for each async command
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Parse and start a publish
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_publish(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	kad_listener_t &	kad_listener	= get_kad_listener();
	kad_err_t	kad_err;
	kad_rec_t	kad_rec;
	delay_t		expire_delay;
	slot_id_t	async_sessid;
	// log to debug
	KLOG_ERR("enter");

	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> kad_rec	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> async_sessid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;

	// get the async_peer_slotid from the session slotid
	async_peer_slotid	= get_peer_slotid_from_session(async_sessid);
	if( async_peer_slotid == slotpool_t::NONE )	return kad_err_t(kad_err_t::ERROR, "Unknown Session ID");

	// start the publish
	return kad_listener.start_publish(kad_rec
					, async_peer_slotid, expire_delay, this, NULL, &async_cmdid);
}

/** \brief Parse and start a delete
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_delete(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	kad_listener_t &	kad_listener	= get_kad_listener();
	kad_err_t	kad_err;
	kad_recid_t	recid;
	kad_keyid_t	keyid;
	delay_t		expire_delay;
	slot_id_t	async_sessid;
	// log to debug
	KLOG_ERR("enter");
	
	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> recid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> keyid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> async_sessid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;

	// get the async_peer_slotid from the session slotid
	async_peer_slotid	= get_peer_slotid_from_session(async_sessid);
	if( async_peer_slotid == slotpool_t::NONE )	return kad_err_t(kad_err_t::ERROR, "Unknown Session ID");

	// start the delete
	return kad_listener.start_delete(recid, keyid
					, async_peer_slotid, expire_delay, this, NULL, &async_cmdid);
}

/** \brief Parse and start a getsome
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_getsome(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	kad_listener_t &	kad_listener	= get_kad_listener();
	kad_err_t	kad_err;
	kad_keyid_t	keyid;
	uint32_t	max_nb_record;
	delay_t		expire_delay;
	slot_id_t	async_sessid;
	// log to debug
	KLOG_ERR("enter");

	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> keyid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> max_nb_record>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> async_sessid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;

	// get the async_peer_slotid from the session slotid
	async_peer_slotid	= get_peer_slotid_from_session(async_sessid);
	if( async_peer_slotid == slotpool_t::NONE )	return kad_err_t(kad_err_t::ERROR, "Unknown Session ID");

	// start the getsome
	return kad_listener.start_getsome(keyid, max_nb_record
					, async_peer_slotid, expire_delay, this, NULL, &async_cmdid);
}

/** \brief Parse and start a getall
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_getall(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	kad_listener_t &	kad_listener	= get_kad_listener();
	kad_err_t	kad_err;
	kad_keyid_t	keyid;
	uint32_t	max_nb_record;
	delay_t		expire_delay;
	slot_id_t	async_sessid;
	// log to debug
	KLOG_ERR("enter");

	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> keyid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> max_nb_record>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> async_sessid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;

	// get the async_peer_slotid from the session slotid
	async_peer_slotid	= get_peer_slotid_from_session(async_sessid);
	if( async_peer_slotid == slotpool_t::NONE )	return kad_err_t(kad_err_t::ERROR, "Unknown Session ID");

	// start the getall
	return kad_listener.start_getall(keyid, max_nb_record
					, async_peer_slotid, expire_delay, this, NULL, &async_cmdid);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Parse and start a nsearch
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_nsearch(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	kad_listener_t &	kad_listener	= get_kad_listener();
	kad_err_t	kad_err;
	kad_peerid_t	peerid;
	delay_t		expire_delay;
	slot_id_t	async_sessid;
	// log to debug
	KLOG_ERR("enter");

	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> peerid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> async_sessid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;

	// get the async_peer_slotid from the session slotid
	async_peer_slotid	= get_peer_slotid_from_session(async_sessid);
	if( async_peer_slotid == slotpool_t::NONE )	return kad_err_t(kad_err_t::ERROR, "Unknown Session ID");

	// start the nsearch
	return kad_listener.start_nsearch(peerid
					, async_peer_slotid, expire_delay, this, NULL, &async_cmdid);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         ALL RPC
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief Parse and start a ping_rpc
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_ping_rpc(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	kad_listener_t &	kad_listener	= get_kad_listener();
	kad_err_t	kad_err;
	kad_addr_t	dest_addr;
	delay_t		expire_delay;
	slot_id_t	async_sessid;
	// log to debug
	KLOG_ERR("enter");

	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> dest_addr	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> async_sessid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;

	// get the async_peer_slotid from the session slotid
	async_peer_slotid	= get_peer_slotid_from_session(async_sessid);
	if( async_peer_slotid == slotpool_t::NONE )	return kad_err_t(kad_err_t::ERROR, "Unknown Session ID");

	// start the ping_rpc
	return kad_listener.start_ping_rpc(
				dest_addr, async_peer_slotid, expire_delay, this, NULL, &async_cmdid);
}


/** \brief Parse and start a store_rpc
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_store_rpc(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	kad_listener_t &	kad_listener	= get_kad_listener();
	kad_err_t	kad_err;
	kad_recdups_t	kad_recdups;
	cookie_id_t	cookie_id;
	kad_addr_t	dest_addr;
	delay_t		expire_delay;
	slot_id_t	async_sessid;
	// log to debug
	KLOG_ERR("enter");

	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> kad_recdups	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> cookie_id	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> dest_addr	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> async_sessid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;

	// get the async_peer_slotid from the session slotid
	async_peer_slotid	= get_peer_slotid_from_session(async_sessid);
	if( async_peer_slotid == slotpool_t::NONE )	return kad_err_t(kad_err_t::ERROR, "Unknown Session ID");

	// start the store_rpc
	return kad_listener.start_store_rpc(kad_recdups, cookie_id
				, dest_addr, async_peer_slotid, expire_delay, this, NULL, &async_cmdid);
}

/** \brief Parse and start a findnode_rpc
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_findnode_rpc(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	kad_listener_t &	kad_listener	= get_kad_listener();
	kad_err_t	kad_err;
	kad_peerid_t	peerid;
	kad_addr_t	dest_addr;
	delay_t		expire_delay;
	slot_id_t	async_sessid;
	// log to debug
	KLOG_ERR("enter");

	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> peerid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> dest_addr	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> async_sessid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;

	// get the async_peer_slotid from the session slotid
	async_peer_slotid	= get_peer_slotid_from_session(async_sessid);
	if( async_peer_slotid == slotpool_t::NONE )	return kad_err_t(kad_err_t::ERROR, "Unknown Session ID");

	// start the findnode_rpc
	return kad_listener.start_findnode_rpc(peerid
				, dest_addr, async_peer_slotid, expire_delay, this, NULL, &async_cmdid);
}


/** \brief Parse and start a findsomeval_rpc
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_findsomeval_rpc(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	kad_listener_t &	kad_listener	= get_kad_listener();
	kad_err_t	kad_err;
	kad_keyid_t	keyid;
	uint32_t	max_nb_record;
	kad_addr_t	dest_addr;
	delay_t		expire_delay;
	slot_id_t	async_sessid;
	// log to debug
	KLOG_ERR("enter");

	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> keyid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> max_nb_record>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> dest_addr	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> async_sessid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;

	// get the async_peer_slotid from the session slotid
	async_peer_slotid	= get_peer_slotid_from_session(async_sessid);
	if( async_peer_slotid == slotpool_t::NONE )	return kad_err_t(kad_err_t::ERROR, "Unknown Session ID");

	// start the findnode_rpc
	return kad_listener.start_findsomeval_rpc(keyid, max_nb_record
				, dest_addr, async_peer_slotid, expire_delay, this, NULL, &async_cmdid);
}


/** \brief Parse and start a findallval_rpc
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_findallval_rpc(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	kad_listener_t &	kad_listener	= get_kad_listener();
	kad_err_t	kad_err;
	kad_recid_t	recid;
	kad_keyid_t	keyid;
	bool		keyid_ge;
	uint32_t	max_nb_record;
	kad_addr_t	dest_addr;
	delay_t		expire_delay;
	slot_id_t	async_sessid;
	// log to debug
	KLOG_ERR("enter");

	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> recid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> keyid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> keyid_ge	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> max_nb_record>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> dest_addr	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> async_sessid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;

	// get the async_peer_slotid from the session slotid
	async_peer_slotid	= get_peer_slotid_from_session(async_sessid);
	if( async_peer_slotid == slotpool_t::NONE )	return kad_err_t(kad_err_t::ERROR, "Unknown Session ID");

	// start the findnode_rpc
	return kad_listener.start_findallval_rpc(recid, keyid, keyid_ge, max_nb_record
				, dest_addr, async_peer_slotid, expire_delay, this, NULL, &async_cmdid);
}



/** \brief Parse and start a delete_rpc
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::parse_delete_rpc(xmlrpc_parse_t &xmlrpc_parse)
										throw(xml_except_t)
{
	kad_listener_t &	kad_listener	= get_kad_listener();
	kad_err_t	kad_err;
	kad_recid_t	recid;
	cookie_id_t	cookie_id;
	kad_addr_t	dest_addr;
	delay_t		expire_delay;
	slot_id_t	async_sessid;
	// log to debug
	KLOG_ERR("enter");

	// parse the parameter
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_BEGIN;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> recid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> cookie_id	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::PARAM_BEGIN	>> dest_addr	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> async_sessid	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse	>> xmlrpc_parse_t::PARAM_BEGIN	>> expire_delay	>> xmlrpc_parse_t::PARAM_END;
	xmlrpc_parse >> xmlrpc_parse_t::PARAMS_END;

	// check the async_peer_slotid exists
	if( !kad_listener.peer_exist(async_peer_slotid) )
		return kad_err_t(kad_err_t::ERROR, "Unknown realm ID");
	// start the findnode_rpc
	return kad_listener.start_delete_rpc(recid, cookie_id
				, dest_addr, async_peer_slotid, expire_delay, this, NULL, &async_cmdid);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        peer_cmd callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a kad_userxmlrpc_srv_t has an event to notify
 */
bool	kad_xmlrpc_t::xmlrpc_cmd_t::neoip_kad_peer_cmd_cb(void *cb_userptr, slot_id_t &cb_slot_id
						, const kad_event_t &kad_event)	throw()
{
	xmlrpc_build_t		xmlrpc_build;
	// setup the xmlrpc_build in a methodResponse
	xmlrpc_build.start_resp();
	// log to debug
	KLOG_ERR("peer_cmd slotid=" << async_cmdid << " reported event " << kad_event);
	
	// sanity check - the kad_event MUST match the method
	// TODO to replace the internal is_*_ok() by the command one which are more readable
	// - e.g. kad_event.is_rpc_nclosest_store_ok() by is_publish_ok()
	if( method == "dht.publish"){			DBG_ASSERT( kad_event.is_rpc_nclosest_store_ok() );
	}else if(method == "dht.delete"){		DBG_ASSERT( kad_event.is_rpc_nclosest_delete_ok() );
	}else if(method == "dht.getsome"){		DBG_ASSERT( kad_event.is_nsearch_findsomeval_ok());
	}else if(method == "dht.getall"){		DBG_ASSERT( kad_event.is_rpc_nclosest_findallval_ok() );
	}else if(method == "dht.nsearch"){		DBG_ASSERT( kad_event.is_nsearch_findnode_ok() );
	}else if(method == "dht.ping_rpc"){		DBG_ASSERT( kad_event.is_ping_rpc_ok() );
	}else if(method == "dht.store_rpc"){		DBG_ASSERT( kad_event.is_store_rpc_ok() );
	}else if(method == "dht.findnode_rpc"){		DBG_ASSERT( kad_event.is_findnode_rpc_ok() );
	}else if(method == "dht.findsomeval_rpc"){	DBG_ASSERT( kad_event.is_findsomeval_rpc_ok() );
	}else if(method == "dht.findallval_rpc"){	DBG_ASSERT( kad_event.is_findallval_rpc_ok() );
	}else if(method == "dht.delete_rpc"){		DBG_ASSERT( kad_event.is_delete_rpc_ok() );
	}else{						DBG_ASSERT( 0 );
	}

	// convert the event in xml
	kad_err_t	kad_err = convert_event_to_xml(xmlrpc_build, kad_event);
	if( kad_err.failed() ){
		KLOG_ERR("Cant convert the kad_event into xml due to " << kad_err);
		goto error;
	}
	
	// log to debug
	KLOG_ERR("Built xml reply is " << string_t::from_datum(xmlrpc_build.get_xml_output()));

	// put the reply into the httpd reply
	async_request.get_reply() << string_t::from_datum(xmlrpc_build.get_xml_output());
	// notify the reply
	lib_session_get()->get_httpd()->notify_delayed_reply(async_request, httpd_err_t::OK);

	// autodelete
	nipmem_delete this;
	// return 'dontkeep'
	return false;

error:;	// notify the reply
	lib_session_get()->get_httpd()->notify_delayed_reply(async_request, httpd_err_t::BOGUS_REQUEST);
	// autodelete
	nipmem_delete this;
	// return 'dontkeep'
	return false;
}

/** \brief convert a kad_event_t into a xmlrpc reply
 */
kad_err_t	kad_xmlrpc_t::xmlrpc_cmd_t::convert_event_to_xml(xmlrpc_build_t &xmlrpc_build
						, const kad_event_t &kad_event)	throw()
{
	// log to debug
	KLOG_ERR("Convert event " << kad_event);
	// if it is an error, return fault
	if( kad_event.is_fatal() ){
		xmlrpc_build.put_fault(-1, "Error due to " + OSTREAMSTR(kad_event));
		return kad_err_t::OK;
	}

	// a sucessfull xmlrpc response contains a single parameter - it starts here
	xmlrpc_build	<< xmlrpc_build_t::PARAM_BEGIN;

	// handle the kad_event according to its type
	switch(kad_event.get_value()){
	case kad_event_t::COMPLETED:	xmlrpc_build << int32_t(0);
					break;
	case kad_event_t::RECDUPS:{	kad_recdups_t	kad_recdups;
					bool		has_more_record;
					kad_recdups =	kad_event.get_recdups(&has_more_record);
					// put the result in a array
					xmlrpc_build << xmlrpc_build_t::ARRAY_BEG;
					xmlrpc_build	<< has_more_record;
					xmlrpc_build	<< kad_recdups;					
					xmlrpc_build << xmlrpc_build_t::ARRAY_END;
					break;}
	case kad_event_t::ADDR_ARR:{	kad_addr_arr_t 	kad_addr_arr;
					cookie_id_t 	cookie_id;
					kad_addr_arr =	kad_event.get_addr_arr(&cookie_id);
					// put the result in a array
					xmlrpc_build << xmlrpc_build_t::ARRAY_BEG;
					xmlrpc_build	<< cookie_id;
					xmlrpc_build	<< kad_addr_arr;					
					xmlrpc_build << xmlrpc_build_t::ARRAY_END;
					break;}
	case kad_event_t::CADDR_ARR:	xmlrpc_build << kad_event.get_caddr_arr();
					break;
	default:			DBG_ASSERT(0);
	}

	// End of the single parameter of the xmlrpc response
	xmlrpc_build	<< xmlrpc_build_t::PARAM_END;
	
	return kad_err_t::OK;
}



NEOIP_NAMESPACE_END





