/*! \file
    \brief Declaration of the ntudp_pserver_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_pserver_wikidbg.hpp"
#include "neoip_ntudp_pserver.hpp"
#include "neoip_ntudp_pserver_tunnel.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ntudp_pserver_wikidbg_t defines the wikidbg stuff for \ref ntudp_pserver_t
 */
class ntudp_pserver_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	wikidbg_page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_menu_item(const std::string &keyword, void *object_ptr)		throw();
	static httpd_err_t	wikidbg_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	ntudp_pserver_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, ntudp_pserver_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("page_title_attr", ntudp_pserver_wikidbg_t::wikidbg_page_title_attr);
	keyword_db.insert_html("menu_item"	, ntudp_pserver_wikidbg_t::wikidbg_menu_item);
	keyword_db.insert_page("page"		, ntudp_pserver_wikidbg_t::wikidbg_page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string ntudp_pserver_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_pserver_t *	ntudp_pserver	= (ntudp_pserver_t *)object_ptr;
	std::ostringstream	oss;


	oss << ntudp_pserver->tunnel_db.size();
	oss << "/";
	oss << ntudp_pserver->extcnx_db.size();
	oss << "/";
	oss << ntudp_pserver->reverse_db.size();

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
std::string ntudp_pserver_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_pserver_t *	ntudp_pserver	= (ntudp_pserver_t *)object_ptr;
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "Connection: ";
	oss << ntudp_pserver->tunnel_db.size()	<< " tunnel";
	oss << " / ";
	oss << ntudp_pserver->extcnx_db.size()	<< " external";
	oss << " / ";
	oss << ntudp_pserver->reverse_db.size()	<< " reverse";
	oss << "\"";
	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu_item" keyword
 */
std::string ntudp_pserver_wikidbg_t::wikidbg_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_pserver_t *	ntudp_pserver	= (ntudp_pserver_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", ntudp_pserver), wikidbg_html("page_title_attr", ntudp_pserver));
	oss << "pserver(";
	oss << wikidbg_html("oneword", ntudp_pserver);
	oss << ")";
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
httpd_err_t ntudp_pserver_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request)	throw()
{
	ntudp_pserver_t *	ntudp_pserver	= (ntudp_pserver_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", ntudp_pserver->ntudp_peer);

	// put the title
	oss << h.pagetitle("ntudp_pserver_t Page");
	
	// display all the tunnels
	oss << h.s_table();
	std::list<ntudp_pserver_tunnel_t *>::iterator	iter;
	for( iter = ntudp_pserver->tunnel_db.begin(); iter != ntudp_pserver->tunnel_db.end(); iter++ ){
		ntudp_pserver_tunnel_t *	pserver_tunnel	= *iter;
		// if it is the first element, add the tableheader
		if( iter == ntudp_pserver->tunnel_db.begin() )
			oss << wikidbg_html("tableheader", pserver_tunnel);
		// display this row
		oss << wikidbg_html("tablerow", pserver_tunnel);
	}
	oss << h.e_table();
	oss << h.br();	

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







