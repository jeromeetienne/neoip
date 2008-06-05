/*! \file
    \brief Declaration of the router_itor_cnx_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_router_itor_cnx_wikidbg.hpp"
#include "neoip_router_itor_cnx.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref router_itor_cnx_wikidbg_t defines the wikidbg stuff for \ref router_itor_cnx_t
 */
class router_itor_cnx_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	router_itor_cnx_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, router_itor_cnx_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl"	, router_itor_cnx_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, router_itor_cnx_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"			, router_itor_cnx_wikidbg_t::page);

	keyword_db.insert_html("tableheader"		, router_itor_cnx_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"		, router_itor_cnx_wikidbg_t::tablerow);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string router_itor_cnx_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	router_itor_cnx_t *	itor_cnx	= (router_itor_cnx_t *)object_ptr;
	std::ostringstream	oss;

	oss <<  wikidbg_html("oneword", itor_cnx->socket_client);

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
std::string router_itor_cnx_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	router_itor_cnx_t *	itor_cnx	= (router_itor_cnx_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", itor_cnx), wikidbg_html("page_title_attr", itor_cnx));
	oss << wikidbg_html("oneword", itor_cnx);
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
std::string router_itor_cnx_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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

/** \brief Handle the "tableheader" keyword
 */
std::string router_itor_cnx_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "socket_client"		<< h.e_b() << h.e_th();
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
std::string router_itor_cnx_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	router_itor_cnx_t*	itor_cnx	= (router_itor_cnx_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", itor_cnx)	<< h.e_td();
	oss << h.e_tr();

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
httpd_err_t router_itor_cnx_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	router_itor_cnx_t *	itor_cnx	= (router_itor_cnx_t *)object_ptr;
	router_itor_t *		router_itor	= itor_cnx->router_itor;
	router_peer_t *		router_peer	= router_itor->router_peer;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", router_peer);

	// put the title
	oss << h.pagetitle("router_itor_cnx_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "router_itor"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << itor_cnx->router_itor	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "socket_client"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << itor_cnx->socket_client	<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







