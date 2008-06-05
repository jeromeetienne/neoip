/*! \file
    \brief Declaration of the upnp_watch_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_upnp_watch_wikidbg.hpp"
#include "neoip_upnp_watch.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref upnp_watch_http_t defines the wikidbg stuff for \ref upnp_watch_t
 */
class upnp_watch_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	upnp_watch_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, upnp_watch_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", upnp_watch_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", upnp_watch_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, upnp_watch_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string upnp_watch_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "upnp_watch";

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
std::string upnp_watch_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	upnp_watch_t *		upnp_watch	= (upnp_watch_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", upnp_watch), wikidbg_html("page_title_attr", upnp_watch));
	oss << wikidbg_html("oneword", upnp_watch);
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
std::string upnp_watch_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t upnp_watch_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	upnp_watch_t *		upnp_watch	= (upnp_watch_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("upnp_watch_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "upnp_disc"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", upnp_watch->upnp_disc)
						<< " (next scheduled in "
						<< wikidbg_html("oneword_pageurl", &upnp_watch->disc_timeout)	
						<< ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "upnp_isavail"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << (upnp_watch->upnp_isavail() ? "yes" : "no")
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "call_extipaddr"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", upnp_watch->call_extipaddr)
						<< " (next scheduled in "
						<< wikidbg_html("oneword_pageurl", &upnp_watch->extipaddr_timeout)	
						<< ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "extipaddr"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << upnp_watch->extipaddr()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "current_disc_res"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &upnp_watch->current_disc_res())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "upnp_portcleaner"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", upnp_watch->upnp_portcleaner)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(upnp_watch->callback)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();
		
	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







