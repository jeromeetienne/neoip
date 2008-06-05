/*! \file
    \brief Header of the \ref kad_xmlrpc_t class
    
*/


#ifndef __NEOIP_KAD_XMLRPC_CMD_HPP__ 
#define __NEOIP_KAD_XMLRPC_CMD_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_xmlrpc.hpp"
#include "neoip_xmlrpc_except.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class xmlrpc_parse_t;
class xmlrpc_build_t;

/** \brief Define the context for xmlrpc command sent to kad_xmlrpc_t
 */
class kad_xmlrpc_t::xmlrpc_cmd_t : NEOIP_COPY_CTOR_DENY, private kad_peer_cmd_cb_t {
private:
	kad_xmlrpc_t *	kad_xmlrpc;	//!< back pointer on the kad_xmlrpc_t
	std::string	method;		//!< the called method



	/*************** sync command parsing	*******************************/
	httpd_err_t	start_sync(xmlrpc_parse_t &xmlrpc_parse, httpd_request_t &request)	throw();
	kad_err_t	parse_session_start(xmlrpc_parse_t &xmlrpc_parse, slot_id_t *sess_slotid_out)
										throw(xml_except_t);
	kad_err_t	parse_session_stop(xmlrpc_parse_t &xmlrpc_parse)	throw(xml_except_t);

							
	/*************** async command parsing	*******************************/
	httpd_err_t	start_async(xmlrpc_parse_t &xmlrpc_parse, httpd_request_t &request)	throw();
	kad_err_t	parse_publish(xmlrpc_parse_t &xmlrpc_parse)		throw(xml_except_t);
	kad_err_t	parse_delete(xmlrpc_parse_t &xmlrpc_parse)		throw(xml_except_t);
	kad_err_t	parse_getsome(xmlrpc_parse_t &xmlrpc_parse)		throw(xml_except_t);
	kad_err_t	parse_getall(xmlrpc_parse_t &xmlrpc_parse)		throw(xml_except_t);
	kad_err_t	parse_nsearch(xmlrpc_parse_t &xmlrpc_parse)		throw(xml_except_t);
	kad_err_t	parse_ping_rpc(xmlrpc_parse_t &xmlrpc_parse)		throw(xml_except_t);
	kad_err_t	parse_store_rpc(xmlrpc_parse_t &xmlrpc_parse)		throw(xml_except_t);
	kad_err_t	parse_findnode_rpc(xmlrpc_parse_t &xmlrpc_parse)	throw(xml_except_t);
	kad_err_t	parse_findsomeval_rpc(xmlrpc_parse_t &xmlrpc_parse)	throw(xml_except_t);
	kad_err_t	parse_findallval_rpc(xmlrpc_parse_t &xmlrpc_parse)	throw(xml_except_t);
	kad_err_t	parse_delete_rpc(xmlrpc_parse_t &xmlrpc_parse)		throw(xml_except_t);


	httpd_request_t async_request;		//!< contain the httpd_request_t which triggered 
						//!< this command.
	slot_id_t	async_cmdid;		//!< the slot_id of the peer_cmd on the kad_listener_t
	slot_id_t	async_peer_slotid;	//!< the slot_id of the realm on which this cmd happens

	bool		neoip_kad_peer_cmd_cb(void *cb_userptr, slot_id_t &cmd_slot_id
						, const kad_event_t &kad_event)	throw();
	kad_err_t	convert_event_to_xml(xmlrpc_build_t &xmlrpc_build
						, const kad_event_t &kad_event)	throw();
	
	/*************** utility function	*******************************/
	kad_listener_t &	get_kad_listener()	const throw()
			{ return kad_xmlrpc->get_kad_listener();	}
	slot_id_t	get_peer_slotid_from_session(const slot_id_t &session_slotid)	throw();


public:
	/*************** ctor/dtor	***************************************/
	xmlrpc_cmd_t(kad_xmlrpc_t *kad_xmlrpc)	throw();
	~xmlrpc_cmd_t()			throw();

	/*************** setup function 	*******************************/
	httpd_err_t	start(httpd_request_t &request)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_XMLRPC_CMD_HPP__  */



