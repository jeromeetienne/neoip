/*! \file
    \brief Declaration of the upnp_disc_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_upnp_disc_wikidbg.hpp"
#include "neoip_upnp_disc.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref upnp_disc_http_t defines the wikidbg stuff for \ref upnp_disc_t
 */
class upnp_disc_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	upnp_disc_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, upnp_disc_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", upnp_disc_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", upnp_disc_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, upnp_disc_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string upnp_disc_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	upnp_disc_t *		upnp_disc	= (upnp_disc_t *)object_ptr;
	std::ostringstream	oss;

	oss << "upnp disc for " << upnp_disc->service_name();

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
std::string upnp_disc_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	upnp_disc_t *		upnp_disc	= (upnp_disc_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", upnp_disc), wikidbg_html("page_title_attr", upnp_disc));
	oss << wikidbg_html("oneword", upnp_disc);
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
std::string upnp_disc_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t upnp_disc_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	upnp_disc_t *		upnp_disc	= (upnp_disc_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("upnp_disc_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "service_name"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << upnp_disc->service_name()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nudp"			<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", upnp_disc->nudp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_sclient"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", upnp_disc->http_sclient)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(upnp_disc->callback)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







