/*! \file
    \brief Header of the \ref kad_xmlrpc_t class
    
*/


#ifndef __NEOIP_KAD_XMLRPC_HPP__ 
#define __NEOIP_KAD_XMLRPC_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_kad_xmlrpc_profile.hpp"
#include "neoip_httpd_handler_cb.hpp"
#include "neoip_html_builder.hpp"
#include "neoip_httpd_request.hpp"
#include "neoip_xmlrpc_except.hpp"
#include "neoip_kad.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Allow to perform all the kad_listener_t operations thru xmlrpc
 * 
 * - it is a pure Helper for kad_listener_t, so it is completly external to kad_listener_t
 */
class kad_xmlrpc_t : NEOIP_COPY_CTOR_DENY, private httpd_handler_cb_t {
private:
	kad_listener_t *	kad_listener;	//!< back pointer on the kad_listener_t
	kad_xmlrpc_profile_t	profile;	//!< the profile attached to this object
	std::string 		url_path;	//!< the url_path of this kad_xmlrpc_t in the 
						//!< session httpd. (if .empty() no http handler is 
						//!< setup)

	httpd_err_t	neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw();

	/*************** command handling 	*******************************/
	class					xmlrpc_cmd_t;
	std::list<xmlrpc_cmd_t *>		xmlrpc_cmd_db;
	void cmd_link(xmlrpc_cmd_t *cmd)	throw()	{ xmlrpc_cmd_db.push_back(cmd);	}
	void cmd_unlink(xmlrpc_cmd_t *cmd)	throw()	{ xmlrpc_cmd_db.remove(cmd);	}

	/*************** session handling 	*******************************/
	class					xmlrpc_sess_t;
	std::list<xmlrpc_sess_t *>		xmlrpc_sess_db;
	slotpool_t				sess_slotpool;	//!< to store the sesssion_slotid
	void sess_link(xmlrpc_sess_t *sess)	throw()	{ xmlrpc_sess_db.push_back(sess);	}
	void sess_unlink(xmlrpc_sess_t *sess)	throw()	{ xmlrpc_sess_db.remove(sess);		}

	/*************** utility function	*******************************/
	kad_listener_t &	get_kad_listener()	const throw()	{ return *kad_listener;	}	
public:
	/*************** ctor/dtor	***************************************/
	kad_xmlrpc_t(kad_listener_t *kad_listener, const std::string &url_path = ""
				, const kad_xmlrpc_profile_t &profile = kad_xmlrpc_profile_t())	throw();
	~kad_xmlrpc_t()										throw();

	
	httpd_err_t	httpd_handler(httpd_request_t &request)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_XMLRPC_HPP__  */



