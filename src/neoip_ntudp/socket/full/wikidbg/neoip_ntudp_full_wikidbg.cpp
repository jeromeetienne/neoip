/*! \file
    \brief Declaration of the ntudp_full_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_full_wikidbg.hpp"
#include "neoip_ntudp_full.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ntudp_full_wikidbg_t defines the wikidbg stuff for \ref ntudp_full_t
 */
class ntudp_full_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_oneword(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	wikidbg_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)		throw();
	static httpd_err_t	wikidbg_notify_event(const std::string &keyword, void *object_ptr, httpd_request_t &request)		throw();

	static std::string	wikidbg_tableheader(const std::string &keyword, void *object_ptr)throw();
	static std::string	wikidbg_tablerow(const std::string &keyword, void *object_ptr)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void ntudp_full_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw()
{
	// add the keywords
	keyword_db.insert_html("oneword"	, ntudp_full_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("page_title_attr", ntudp_full_wikidbg_t::wikidbg_page_title_attr);
	keyword_db.insert_page("page"		, ntudp_full_wikidbg_t::wikidbg_page);

	keyword_db.insert_page("notify_event"	, ntudp_full_wikidbg_t::wikidbg_notify_event);
		
	keyword_db.insert_html("tableheader"	, ntudp_full_wikidbg_t::wikidbg_tableheader);
	keyword_db.insert_html("tablerow"	, ntudp_full_wikidbg_t::wikidbg_tablerow);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string ntudp_full_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
//	ntudp_full_t *		ntudp_full	= (ntudp_full_t *)object_ptr;
	std::ostringstream	oss;

	oss << "oneword";

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
std::string ntudp_full_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
//	ntudp_full_t *		ntudp_full	= (ntudp_full_t *)object_ptr;
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "Fully established ntudp connection";
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
httpd_err_t ntudp_full_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request)	throw()
{
	ntudp_full_t *		ntudp_full	= (ntudp_full_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", ntudp_full->ntudp_peer);

	// put the title
	oss << h.pagetitle("ntudp_full_t Page");
	
	oss << h.s_table_packed_noborder();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "Local Address"	<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << ntudp_full->local_addr();
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "Remote Address"	<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << ntudp_full->remote_addr();
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "Path MTU Discovery"	<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << (ntudp_full->mtu_pathdisc() ? "Enabled" : "Disabled");
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "recv_max_len"		<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << ntudp_full->recv_max_len_get();
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "udp_full_t"		<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ntudp_full->udp_full);
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "callback"		<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << wikidbg_html_callback(ntudp_full->callback);
		oss << h.e_tr();
	oss << h.e_table();
	
	oss << h.sub1title("Event Notification");
	oss << "<form method=\"get\"" << " action=\"" << wikidbg_url_rootpath() << "\">"
				<< "<input type=\"submit\" value=\"CNX_CLOSED\">"
				<< wikidbg_form_hidden_param("notify_event", ntudp_full)
				<< "<input type=\"hidden\" name=\"event_name\" value=\"CNX_CLOSED\">"
				<< "</form>";
	oss << "<form method=\"get\"" << " action=\"" << wikidbg_url_rootpath() << "\">"
				<< "<input type=\"submit\" value=\"MTU_CHANGE\">"
				<< "<input type=\"text\" name=\"new_mtu_value\" title=\"The new MTU to notify\" size=\"20\">"
				<< wikidbg_form_hidden_param("notify_event", ntudp_full)
				<< "<input type=\"hidden\" name=\"event_name\" value=\"MTU_CHANGE\">"
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
httpd_err_t ntudp_full_wikidbg_t::wikidbg_notify_event(const std::string &keyword, void *object_ptr
						, httpd_request_t &request)	throw()
{
	ntudp_full_t *	ntudp_full	= (ntudp_full_t *)object_ptr;
	std::string	event_name	= request.get_variable("event_name");
	ntudp_event_t	ntudp_event;
	
	if( event_name == "CNX_CLOSED" ){
		ntudp_event	= ntudp_event_t::build_cnx_closed("Triggered by wikidbg");
		ntudp_full->notify_callback(ntudp_event);
	}else if( event_name == "MTU_CHANGE" ){
		size_t			new_mtu = 0;
		std::istringstream	new_mtu_iss(request.get_variable("new_mtu_value"));
		new_mtu_iss >> new_mtu;
		ntudp_event	= ntudp_event_t::build_mtu_change(new_mtu);
		ntudp_full->notify_callback(ntudp_event);		
	}else{
		// return BOGUS_REQUEST - as this case SHOULD NOT happen
		return httpd_err_t::BOGUS_REQUEST;
	}
		
	// return to the referer page
	return httpd_err_t(httpd_err_t::MOVED_TEMP, request.get_header("Referer"));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "tableheader" keyword
 */
std::string ntudp_full_wikidbg_t::wikidbg_tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "udp_full"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "Local Address"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "Remote Address"		<< h.e_b() << h.e_th();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "tablerow" keyword
 */
std::string ntudp_full_wikidbg_t::wikidbg_tablerow(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_full_t*		ntudp_full	= (ntudp_full_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", ntudp_full->udp_full)	<< h.e_td();
	oss << h.s_td() << ntudp_full->local_addr()					<< h.e_td();
	oss << h.s_td() << ntudp_full->remote_addr()					<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END







