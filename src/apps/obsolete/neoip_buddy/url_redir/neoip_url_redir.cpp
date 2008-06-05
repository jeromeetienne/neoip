/*! \file
    \brief Definition of the \ref url_redir_t class

\par Brief Description
This module performs url redirection thru dnsgrab_t and httpd_t
    
*/

/* system include */
/* local include */
#include "neoip_url_redir.hpp"
#include "neoip_httpd.hpp"
#include "neoip_dnsgrab.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// TODO just for debug - to remove
#include "neoip_ip_netaddr.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
url_redir_t::url_redir_t()	throw()
{
	// zero the httpd_srv
	httpd_srv	= NULL;
	dnsgrab		= NULL;
	
	add_hosturl("movies"	, "http://127.0.0.1:9080/neoip_lib/fdir/movies2?dir_order=n&dir_view=thumbnail");
	add_hosturl("music"	, "http://127.0.0.1:9080/neoip_lib/fdir/music");
	add_hosturl("books"	, "http://127.0.0.1:9080/neoip_lib/fdir/books");
	add_hosturl("photo"	, "http://127.0.0.1:9080/neoip_lib/fdir/home/jerome/work/photo/ijay_vs_rain");
	add_hosturl("teen"	, "http://127.0.0.1:9080/neoip_lib/fdir/movies2/teen?dir_order=n&dir_view=thumbnail");
	add_hosturl("completed"	, "http://127.0.0.1:9080/neoip_lib/fdir/slota/completed?dir_order=n&dir_view=thumbnail");
	add_hosturl("serpent"	, "http://127.0.0.1:9080/neoip_lib/fdir/music/ogg/guem/serpent");
	add_hosturl("telephone"	, "http://127.0.0.1:9080/neoip_lib/fdir/music/T%C3%A9l%C3%A9phone/Rappels%202");
	
}

/** \brief Destructor
 */
url_redir_t::~url_redir_t()	throw()
{
	// delete the httpd_srv if needed
	if( httpd_srv )		nipmem_delete httpd_srv;	
	// delete the dnsgrab_t if needed
	if( dnsgrab )	nipmem_delete dnsgrab;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             Start operation
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 * 
 * @return false if no error occurs, true otherwise
 */
bool	url_redir_t::start()	throw()
{
	// create the httpd_srv
	httpd_srv	= nipmem_new httpd_t();

#if 0
/* NOTE:
 * - ok this mechanism allow you to get all the address
 * - but what is the port 80 is already bound on any ?
 *   - i should detect this case, and fails the init 
 *     - how can i detect this case ?
 *       - if bound in any:80, nobody else can listen on 80
 *   - else test the local addresses until one is ok
 */
	ip_netaddr_t	ip_netaddr("127.0.0.0/8");
	ip_addr_t	ip_addr	= "127.0.0.1";
	
	for( size_t i = 0; i < 20; i++ ){
		KLOG_ERR("ip_addr=" << ip_addr );
		ip_addr	= ip_netaddr.get_next_addr(ip_addr);
	}
#endif

	// TODO try to bind a localhost 127.0.0.0/8 on port 80
	// - how to scanall the localhost address
	//   - do a function in ip_netaddr_t
	//   - .get_next(ipaddr);
	bool	failed	=  httpd_srv->start("127.0.0.2:80");
	DBG_ASSERT( !failed );	// TODO poor error management
	// add the handler for any path on this server	
	httpd_srv->handler_add("", this, NULL, httpd_handler_t::DEEPER_PATH_OK);

	// create and start the dnsgrab_t
	dnsgrab_err_t	dnsgrab_err;
	dnsgrab		= nipmem_new dnsgrab_t(this, NULL);
	dnsgrab_err	= dnsgrab->start("127.0.0.1", "system", 800, delay_t::from_sec(30), "url_redir");
	DBG_ASSERT( !dnsgrab_err.failed() );	// TODO poor error management

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            database function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Add a url_str into the database for this host_str
 */
void	url_redir_t::add_hosturl(const std::string &host_str, const std::string &url_str)	throw()
{
	bool	succeed	= hosturl_db.insert(std::make_pair(host_str, url_str)).second;
	DBG_ASSERT( succeed );
}

/** \brief Return the url_str attached to this host_str, or an empty string is none is found
 */
std::string url_redir_t::get_hosturl(const std::string &host_str)			const throw()
{
	std::map<std::string, std::string>::const_iterator	iter;
	// try to find this host_str in the database
	iter	= hosturl_db.find(host_str);
	// if the host_str is not found, return ""
	if( iter == hosturl_db.end() )	return "";
	// else return the associated url_str
	return iter->second;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             HTTPD callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback notified when a httpd request is received by this neoip_httpd_handler
 */
httpd_err_t url_redir_t::neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)
										throw()
{
	// log to debug
	KLOG_ERR("request method=" << request.get_method() << " path=" << request.get_path() 
					<< " host=" << request.get_header("Host"));

	std::string	host_str	= request.get_header("Host");
	KLOG_ERR("request for host=" << host_str);
	std::string	url_str		= get_hosturl(host_str);
	if( url_str.empty() )	return httpd_err_t::NOT_FOUND;

	// redirect to elsewhere
	return httpd_err_t(httpd_err_t::MOVED_TEMP, url_str);
}

/** \brief dnsgrab_t callback to received request	
 */
bool url_redir_t::neoip_dnsgrab_cb(void *cb_userptr, dnsgrab_request_t &request)	throw()
{
	// log the request
	KLOG_ERR("Received a request of " << request.get_request_name() 
			<< " From " << request.get_addr_family()
			<< " by " << (request.is_request_by_name() ? "name" : "address"));

	// if the request is NOT for AF_INET, report notfound
// TODO i fails in the libnss library and if a AF_INET6 comes it does retry in
// AF_INET, before it did.... fix the libnss
// - then reenable this line
	if( request.get_addr_family() != "AF_INET" )		goto report_notfound;
	// if the request is NOT by name, report false for NOT OK
	if( !request.is_request_by_name() )			goto report_notfound;
	// if the request is for a unknown name, report false for NOT OK
	if( get_hosturl(request.get_request_name()).empty() )	goto report_notfound;

	KLOG_ERR("request handled for " << request.get_request_name());

/*
 * if the name start by "neoip." or "buddy." AND
 * doesnt end by .fr .org .com etc...
 * 
 * then keep it
 */
//	char *	required_prefix[]	= { "neoip.", "buddy." };
//	char *	forbidden_suffix[]	= { ".org", ".com", ".net" };

 

	// set the reply with dummy data before notifying it
	request.get_reply_present()	= true;
	request.get_reply_name()	= "neoip.localhost";
//	request.get_reply_aliases().push_back("jerome.etienne");
//	request.get_reply_aliases().push_back("jeje");
	request.get_reply_addresses().push_back("127.0.0.2");
	
	// return tokeep
	return true;
	
report_notfound:
	// make the reply as not found
	request.get_reply_present()	= true;
	request.get_reply_name()	= std::string();	
	return true;
}

NEOIP_NAMESPACE_END


