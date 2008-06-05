/*! \file
    \brief Declaration of the router_rident_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_router_rident_wikidbg.hpp"
#include "neoip_router_rident.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref router_rident_http_t defines the wikidbg stuff for \ref router_rident_t
 */
class router_rident_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	tableheader(const std::string &keyword, void *object_ptr)	throw();
	static std::string	tablerow(const std::string &keyword, void *object_ptr)		throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	router_rident_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, router_rident_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", router_rident_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", router_rident_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, router_rident_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, router_rident_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, router_rident_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string router_rident_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	router_rident_t *	router_rident	= (router_rident_t *)object_ptr;
	std::ostringstream	oss;

	oss << router_rident->hostonly_lview();

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
std::string router_rident_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	router_rident_t *	router_rident	= (router_rident_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", router_rident), wikidbg_html("page_title_attr", router_rident));
	oss << wikidbg_html("oneword", router_rident);
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
std::string router_rident_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t router_rident_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	router_rident_t *	router_rident	= (router_rident_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("router_rident_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "peerid"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << router_rident->peerid()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cert"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << router_rident->cert()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "hostonly_lview"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << router_rident->hostonly_lview()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
			
	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "tableheader" keyword
 */
std::string router_rident_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "name"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "peerid"		<< h.e_b() << h.e_th();
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
std::string router_rident_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	router_rident_t *	router_rident	= (router_rident_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// produce the html
	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", router_rident)	<< h.e_td();
	oss << h.s_td() << router_rident->peerid()				<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END







