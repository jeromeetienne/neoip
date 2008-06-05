/*! \file
    \brief Declaration of the udp_full_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_udp_full_wikidbg.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_udp_event.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref udp_full_wikidbg_t defines the wikidbg stuff for \ref udp_full_t
 */
class udp_full_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_oneword(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();	
	static httpd_err_t	wikidbg_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)		throw();

	static httpd_err_t	wikidbg_notify_event(const std::string &keyword, void *object_ptr, httpd_request_t &request)		throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	udp_full_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw()
{
	// add the keywords
	keyword_db.insert_html("oneword"	, udp_full_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("page_title_attr", udp_full_wikidbg_t::wikidbg_page_title_attr);
	keyword_db.insert_html("oneword_pageurl", udp_full_wikidbg_t::wikidbg_oneword_pageurl);	
	keyword_db.insert_page("page"		, udp_full_wikidbg_t::wikidbg_page);
	
	keyword_db.insert_page("notify_event"	, udp_full_wikidbg_t::wikidbg_notify_event);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string udp_full_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	udp_full_t *		udp_full	= (udp_full_t *)object_ptr;
	std::ostringstream	oss;

	oss << udp_full->get_local_addr();
	oss << "/";
	oss << udp_full->get_remote_addr();

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
std::string udp_full_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "Fully established udp connection";
	oss << "\"";
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
std::string udp_full_wikidbg_t::wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	udp_full_t *		udp_full	= (udp_full_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", udp_full), wikidbg_html("page_title_attr", udp_full));
	oss << wikidbg_html("oneword", udp_full);
	oss << h.e_link();

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
httpd_err_t udp_full_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request)	throw()
{
	udp_full_t *		udp_full	= (udp_full_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("udp_full_t Page");


	oss << h.s_table_packed_noborder();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "Local Address"	<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << udp_full->get_local_addr();
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "Remote Address"	<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << udp_full->get_remote_addr();
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "Path MTU Discovery"	<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << (udp_full->mtu_pathdisc() ? "Enabled" : "Disabled");
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "recv_max_len"		<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << udp_full->recv_max_len_get();
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "callback"		<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << wikidbg_html_callback(udp_full->callback);
		oss << h.e_tr();
	oss << h.e_table();
	
	oss << h.sub1title("Event Notification");
	oss << "<form method=\"get\"" << " action=\"" << wikidbg_url_rootpath() << "\">"
				<< "<input type=\"submit\" value=\"CNX_CLOSED\">"
				<< wikidbg_form_hidden_param("notify_event", udp_full)
				<< "<input type=\"hidden\" name=\"event_name\" value=\"CNX_CLOSED\">"
				<< "</form>";
	oss << "<form method=\"get\"" << " action=\"" << wikidbg_url_rootpath() << "\">"
				<< "<input type=\"submit\" value=\"MTU_CHANGE\">"
				<< "<input type=\"text\" name=\"new_mtu_value\" title=\"The new MTU to notify\" size=\"20\">"
				<< wikidbg_form_hidden_param("notify_event", udp_full)
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
httpd_err_t udp_full_wikidbg_t::wikidbg_notify_event(const std::string &keyword, void *object_ptr
						, httpd_request_t &request)	throw()
{
	udp_full_t *	udp_full	= (udp_full_t *)object_ptr;
	std::string	event_name	= request.get_variable("event_name");
	udp_event_t	udp_event;
	
	if( event_name == "CNX_CLOSED" ){
		udp_event	= udp_event_t::build_cnx_closed("Triggered by wikidbg");
		udp_full->notify_callback(udp_event);
	}else if( event_name == "MTU_CHANGE" ){
		size_t			new_mtu = 0;
		std::istringstream	new_mtu_iss(request.get_variable("new_mtu_value"));
		new_mtu_iss >> new_mtu;
		udp_event	= udp_event_t::build_mtu_change(new_mtu);
		udp_full->notify_callback(udp_event);		
	}else{
		// return BOGUS_REQUEST - as this case SHOULD NOT happen
		return httpd_err_t::BOGUS_REQUEST;
	}
		
	// return to the referer page
	return httpd_err_t(httpd_err_t::MOVED_TEMP, request.get_header("Referer"));
}


NEOIP_NAMESPACE_END







