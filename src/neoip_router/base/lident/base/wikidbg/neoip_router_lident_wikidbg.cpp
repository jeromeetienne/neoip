/*! \file
    \brief Declaration of the router_lident_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_router_lident_wikidbg.hpp"
#include "neoip_router_lident.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref router_lident_http_t defines the wikidbg stuff for \ref router_lident_t
 */
class router_lident_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	router_lident_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, router_lident_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", router_lident_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", router_lident_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, router_lident_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string router_lident_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	router_lident_t *	router_lident	= (router_lident_t *)object_ptr;
	std::ostringstream	oss;

	oss << router_lident->dnsname();

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
std::string router_lident_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	router_lident_t *	router_lident	= (router_lident_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", router_lident), wikidbg_html("page_title_attr", router_lident));
	oss << wikidbg_html("oneword", router_lident);
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
std::string router_lident_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t router_lident_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	router_lident_t *	router_lident	= (router_lident_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("router_lident_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "peerid"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << router_lident->peerid()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "privkey"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << router_lident->privkey()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cert"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << router_lident->cert()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "dnsname"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << router_lident->dnsname()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
			
	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







