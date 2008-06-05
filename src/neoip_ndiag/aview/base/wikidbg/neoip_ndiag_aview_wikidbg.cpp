/*! \file
    \brief Declaration of the ndiag_aview_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ndiag_aview_wikidbg.hpp"
#include "neoip_ndiag_aview.hpp"
#include "neoip_ndiag_watch.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ndiag_aview_http_t defines the wikidbg stuff for \ref ndiag_aview_t
 */
class ndiag_aview_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static httpd_err_t	notify_event(const std::string &keyword, void *object_ptr, httpd_request_t &request)		throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	ndiag_aview_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, ndiag_aview_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", ndiag_aview_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", ndiag_aview_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, ndiag_aview_wikidbg_t::page);

	keyword_db.insert_page("notify_event"	, ndiag_aview_wikidbg_t::notify_event);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string ndiag_aview_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "ndiag_aview";

	// return the built string
	return oss.str();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword_pageurl" keyword
 */
std::string ndiag_aview_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	ndiag_aview_t *		ndiag_aview	= (ndiag_aview_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", ndiag_aview), wikidbg_html("page_title_attr", ndiag_aview));
	oss << wikidbg_html("oneword", ndiag_aview);
	oss << h.e_link();
	
	// return the built string
	return oss.str();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "page_title_attr" keyword
 */
std::string ndiag_aview_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "Click for more info";
	oss << "\"";
	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "page" keyword
 */
httpd_err_t ndiag_aview_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	ndiag_aview_t *		ndiag_aview	= (ndiag_aview_t *)object_ptr;
	ndiag_watch_t *		ndiag_watch	= ndiag_watch_get();
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("ndiag_aview_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ipport_aview_init"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << ndiag_aview->ipport_aview_init()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "sockfam"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << ndiag_aview->sockfam()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "desc_str"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << ndiag_aview->desc_str()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ipport_pview_curr"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << ndiag_aview->ipport_pview_curr()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "upnp_bindport"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ndiag_aview->upnp_bindport)
					<< " and " << (ndiag_aview->is_bindport_needed() ? "IS" : " IS NOT") << " needed."
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(ndiag_aview->callback)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ndiag_watch"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ndiag_watch)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	oss << h.sub1title("new_ipport_pview Notification :");
	oss << " Warning this bypass all the ndiag_aview_t local fields!";
	oss << "<form method=\"get\"" << " action=\"" << wikidbg_url_rootpath() << "\">"
				<< "<input type=\"submit\" value=\"new_ipport_pview\">"
				<< "<input type=\"text\" name=\"new_ipport_addr\" title=\"The new ipport_pview to notify\" size=\"20\">"
				<< wikidbg_form_hidden_param("notify_event", ndiag_aview)
				<< "</form>";
	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "notify_event" keyword
 */
httpd_err_t ndiag_aview_wikidbg_t::notify_event(const std::string &keyword, void *object_ptr
						, httpd_request_t &request)	throw()
{
	ndiag_aview_t *	ndiag_aview	= (ndiag_aview_t *)object_ptr;
	std::string	ipport_pview_str= string_t::unescape(request.get_variable("new_ipport_addr"));
	// get the ipport_addr_t from the httpd_request and notify it
	ipport_addr_t	ipport_pview	= ipport_pview_str;
	ndiag_aview->notify_callback(ipport_pview);		
	// log to debug
	KLOG_DBG("ipport_pview_str="<< ipport_pview_str << " parsed in ipport_pview=" << ipport_pview);
	// return to the referer page
	return httpd_err_t(httpd_err_t::MOVED_TEMP, request.get_header("Referer"));
}

NEOIP_NAMESPACE_END







