/*! \file
    \brief Definition of the \ref httpd_t class

\par Possible Improvement
- to port on top of socket_t once it works well
- to handle the maysend_on/off in transmit

*/

/* system include */
/* local include */
#include "neoip_lib_httpd.hpp"
#include "neoip_httpd.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_utils.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_slotpool.hpp"
#include "neoip_wikidbg_http.hpp"
#include "neoip_cpp_demangle.hpp"
#include "neoip_nipmem_tracker_http.hpp"
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
lib_httpd_t::lib_httpd_t(lib_session_t *lib_session)	throw()
{
	this->lib_session	= lib_session;
	// set the httpd root path
	rootpath		= "/neoip_lib";
	// start the httpd itself
	httpd			= nipmem_new httpd_t();
	// add the basic handler
	httpd->handler_add(get_rootpath(), this, NULL );
	httpd->handler_add(get_rootpath() + "/handler_list.html", this, NULL);
	httpd->handler_add("/", this, NULL );
	httpd->handler_add("/index.html", this, NULL);
	httpd->handler_add("/apps_info.js", this, NULL);
	httpd->handler_add("/neoip_apps_info_jsrest.js", this, NULL);
	httpd->handler_add("/favicon.ico", this, NULL);
	// init the wikidbg_http_t
	wikidbg_http		= nipmem_new wikidbg_http_t(httpd, get_rootpath() + "/wikidbg");
	// zero nipmem_tracker_http_t
	nipmem_tracker_http	= NULL;
}

/** \brief Destructor
 */
lib_httpd_t::~lib_httpd_t()					throw()
{
	// free the nipmem_tracker_http_t
	nipmem_zdelete	nipmem_tracker_http;
	// free the wikidbg_http_t
	nipmem_zdelete	wikidbg_http;
	// remove the handler
	httpd->handler_del(get_rootpath(), this, NULL);
	httpd->handler_del(get_rootpath() + "/handler_list.html", this, NULL);
	httpd->handler_del("/", this, NULL);
	httpd->handler_del("/index.html", this, NULL);
	httpd->handler_del("/apps_info.js", this, NULL);
	httpd->handler_del("/neoip_apps_info_jsrest.js", this, NULL);
	httpd->handler_del("/favicon.ico", this, NULL);
	// stop the httpd itself
	nipmem_zdelete	httpd;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//               start function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
bool lib_httpd_t::start()	throw()
{
	ipport_addr_t	listen_addr	= listen_addr_from_conf();
	bool		failed;

	// start the httpd_t
	failed	= httpd->start(listen_addr);
	if( failed )	return true;

	// initialize the the nipmem_tracker_http
	nipmem_tracker_http	= nipmem_new nipmem_tracker_http_t();
	
	// return no error
	return false;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Get the listen_addr from the lib_session_t config
 */
ipport_addr_t	lib_httpd_t::listen_addr_from_conf()			throw()
{
	const strvar_db_t &session_conf	= lib_session->session_conf();
	ip_addr_t	listen_ipaddr	= session_conf.get_first_value("wikidbg_listen_addr","127.0.0.1");
	uint16_t	port_min	= atoi(session_conf.get_first_value("wikidbg_listen_port_min","9080").c_str());
	uint16_t	port_max	= atoi(session_conf.get_first_value("wikidbg_listen_port_max","9090").c_str());
	
	// sanity check - port_min MUST be <= than port_max
	if( port_min > port_max ){
		KLOG_INFO("in config file, wikidbg_listen_port_min is greater than wikidbg_listen_port_max!! aborting");
		return ipport_addr_t();
	}
	
	// go thru all the port 
	for(uint16_t cur_port = port_min; cur_port <= port_max; cur_port++){
		ipport_addr_t	listen_ipport(listen_ipaddr, cur_port);
		inet_err_t	inet_err;
		tcp_resp_t *	tcp_resp;
		// try to start a tcp_resp_t on the cur_ipport
		tcp_resp	= nipmem_new tcp_resp_t();
		inet_err	= tcp_resp->start(listen_ipport, NULL, NULL);
		nipmem_zdelete	tcp_resp;
		// if the start() failed, goto the next
		if( inet_err.failed() )	continue;
		// if the binding succeed, return this value
		return listen_ipport;
	}
	
	// NOTE: here no configured wikidbg_listen_addr/port succeed
	KLOG_WARN("unable to use the configured wikidbg_listen_addr/port. trying a dynamic port");
	
	// try to get a dynamic port
	ipport_addr_t	listen_ipport(listen_ipaddr, 0);
	inet_err_t	inet_err;
	tcp_resp_t *	tcp_resp;
	// try to start a tcp_resp_t on the cur_ipport
	tcp_resp	= nipmem_new tcp_resp_t();
	inet_err	= tcp_resp->start(listen_ipport, NULL, NULL);
	nipmem_zdelete	tcp_resp;
	// if the start() succeed, use this address
	if( inet_err.succeed() )	return listen_ipport;

	// NOTE: here no wikidbg_listen_addr/port AT ALL succeed
	KLOG_WARN("unable to find any port for wikidbg. aborting");
	
	// if this point is reached, no wikidbg_listen_addr/port has been found 
	return ipport_addr_t();	
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the listen address of this httpd_t
 */
ipport_addr_t	lib_httpd_t::get_listen_addr() const throw()
{
	return httpd->get_listen_addr();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			add/del handler
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add a handler in \ref lib_httpd_t
 */
void lib_httpd_t::handler_add(std::string path, httpd_handler_cb_t *callback, void *userptr
					, httpd_handler_flag_t handler_flag )	throw()
{
	httpd->handler_add(path, callback, userptr, handler_flag);
}

/** \brief delete a handler in \ref lib_httpd_t
 */
void lib_httpd_t::handler_del(std::string path, httpd_handler_cb_t *callback, void *userptr
					, httpd_handler_flag_t handler_flag )	throw()
{
	httpd->handler_del(path, callback, userptr, handler_flag);
}

/** \brief to notify a delayed reply
 */
void	lib_httpd_t::notify_delayed_reply(const httpd_request_t &request, const httpd_err_t &httpd_err)
										throw()
{
	// just forward it to httpd
	httpd->notify_delayed_reply(request, httpd_err);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			HTTPD callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a httpd request is received by this neoip_httpd_handler
 * 
 * - raw internal handler specific to the httpd itself
 */
httpd_err_t lib_httpd_t::neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request) throw()
{
	// log to debug
	KLOG_DBG("enter request.get_path()=" << request.get_path());
	// forwrad the httpd_request according to its path
	if( request.get_path() == get_rootpath()+ "" )			return handler_root_cb(request);
	if( request.get_path() == get_rootpath()+ "/handler_list.html")	return handler_list_cb(request);
	if( request.get_path() == "/" )					return handler_root_cb(request);
	if( request.get_path() == "/index.html" )			return handler_root_cb(request);
	if( request.get_path() == "/apps_info.js" )			return handler_apps_info_cb(request);
	if( request.get_path() == "/neoip_apps_info_jsrest.js" )	return handler_apps_info_jsrest_cb(request);
	if( request.get_path() == "/favicon.ico")			return handler_static_file_cb(request);
	return httpd_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             default handler 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Display root page 
 */
httpd_err_t lib_httpd_t::handler_root_cb( httpd_request_t &request )	throw()
{
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	std::ostringstream	&oss		= request.get_reply();
	html_builder_t		h;
	
	oss << h.pagetitle("NeoIP Library Root");

	oss << h.s_link(get_rootpath() + "/handler_list.html") << "handler_list" << h.e_link()
			<< ": the list of all the handler for this httpd";
	oss << h.br();


	oss << h.sub1title("APPS INFO:");
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "canon_name"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << lib_apps->canon_name()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "human_name"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << lib_apps->human_name()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "version"			<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << lib_apps->version()		<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "summary"			<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << lib_apps->summary()		<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "longdesc"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << lib_apps->longdesc()		<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "apps_type"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << lib_apps->apps_type()	<< h.e_td();
			oss << h.e_tr();
		oss << h.e_table();	
	oss << h.br();
		
	
	return httpd_err_t::OK;
}

/** \brief Return the apps_info.js
 * 
 * - NOTE: used to autodiagnoze the application listening on this httpd
 * - TODO: to remove - obsolete stuff. use the apps_info_jsrest instead
 */
httpd_err_t lib_httpd_t::handler_apps_info_cb( httpd_request_t &request )	throw()
{
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	std::ostringstream	&oss		= request.get_reply();
	// log to debug
	KLOG_ERR("enter");	
	// set the mimetype of the reply
	request.get_reply_mimetype()	= "text/javascript";

	// write the javascript
	// - NOTE: currently a stub
	oss << "canon_name='"	<< lib_apps->canon_name()	<< "';\n";
	oss << "summary='"	<< lib_apps->summary()		<< "';\n";
	// return no error	
	return httpd_err_t::OK;
}

/** \brief Return the neoip_apps_info_jsrest.js
 * 
 * - NOTE: used to autodiagnoze the application listening on this httpd
 */
httpd_err_t lib_httpd_t::handler_apps_info_jsrest_cb( httpd_request_t &request )	throw()
{
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	std::ostringstream	&oss		= request.get_reply();
	// log to debug
	KLOG_DBG("enter");	
	// set the mimetype of the reply
	request.get_reply_mimetype()	= "application/x-javascript";

	// example of a jsrest exchange
	// request:
	// http://127.0.0.1:4550/neoip_oload_appdetect_jsrest.js?obj_id=32188&js_callback=neoip_xdomrpc_cb_callback_from_server&method_name=probe_apps
	// response:
	// window.parent.neoip_xdomrpc_script_reply_var_32188 = {fault: null, returned_val: "0.0.1"};
	
	// emulate a xmlrpc_resp_t for a jsrest handler
	// - this is done crappy and duplicated here. because this old httpd_t
	//   object still handle all the wikidbg. and i dont want to port
	//   all wikidbg handler to the new http_sresp_t even it is is MUCH cleaner
	// - look at xmlrpc_listener_t for details on this jsrest stuff

	// get the variables from the request
	std::string	obj_id		= request.get_variable("obj_id");
	std::string	js_callback	= request.get_variable("js_callback");
	std::string	method_name	= request.get_variable("method_name");
	
	// build the js to return
#if 0	// TODO to remove - old version i dunno what window.parent was used for
	// - likely for experiement with iframe
	oss << "window.parent.neoip_xdomrpc_script_reply_var_" << obj_id << " = ";
#else
	oss << "neoip_xdomrpc_script_reply_var_" << obj_id << " = ";
#endif

	oss << "window.parent.neoip_xdomrpc_script_reply_var_" << obj_id << " = ";
	oss << "{fault: null, returned_val :";
	oss << "{";
	oss << "canon_name: 	\"" << lib_apps->canon_name()	<< "\"";
	oss << ", ";
	oss << "summary:	\"" << lib_apps->summary()	<< "\"";
	oss << "}};";
	
	// return no error	
	return httpd_err_t::OK;
}

/** \brief Display the list of registered handler
 */
httpd_err_t lib_httpd_t::handler_list_cb(httpd_request_t &request)	throw()
{
	std::ostringstream	&oss	= request.get_reply();
	html_builder_t		h;
	std::map<std::string,httpd_handler_t>::iterator	iter;

	oss << h.pagetitle("List of registered handler in lib_httpd");
	
	oss << h.s_table();
	oss << h.th("Path") << h.th("callback") << h.th("Userptr");
	for( iter = httpd->handler_db.begin(); iter != httpd->handler_db.end(); iter++ ){
		httpd_handler_t	&handler = iter->second;
		oss << h.s_tr();
		oss << h.s_td() << h.s_link(handler.get_path()) << handler.get_path()
					<< h.e_link() << h.e_td();
		oss << h.s_td() << handler.get_callback() << " of type " 
					<< neoip_cpp_typename(*(handler.get_callback())) << h.e_td();
		oss << h.s_td() << handler.get_userptr() << h.e_td();
		oss << h.e_tr();
	}
	oss << h.e_table();
	return httpd_err_t::OK;
}

/** \brief Return a static file
 * 
 * - currently it is dedicated to favicon.ico
 */
httpd_err_t	lib_httpd_t::handler_static_file_cb(httpd_request_t &request)	throw()
{
	file_path_t	file_path	= lib_session->conf_rootdir() / "neoip_http.ico";
	datum_t		datum;
	// read the whole file
	file_err_t	file_err;
	file_err	= file_sio_t::readall(file_path, datum);
	if( file_err.failed() )	return httpd_err_t::INTERNAL_ERROR;
	// put the read file into the request.get_reply();
	request.get_reply() << datum.to_stdstring();
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END





