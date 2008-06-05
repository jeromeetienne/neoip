/*! \file
    \brief Declaration of the ntudp_relpeer_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_relpeer_wikidbg.hpp"
#include "neoip_ntudp_relpeer.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ntudp_relpeer_wikidbg_t defines the wikidbg stuff for \ref ntudp_relpeer_t
 */
class ntudp_relpeer_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_oneword(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_menu_item(const std::string &keyword, void *object_ptr)		throw();
	static httpd_err_t	wikidbg_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)		throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	ntudp_relpeer_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw()
{
	// add the keywords
	keyword_db.insert_html("oneword"	, ntudp_relpeer_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("page_title_attr", ntudp_relpeer_wikidbg_t::wikidbg_page_title_attr);
	keyword_db.insert_html("menu_item"	, ntudp_relpeer_wikidbg_t::wikidbg_menu_item);	
	keyword_db.insert_page("page"		, ntudp_relpeer_wikidbg_t::wikidbg_page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string ntudp_relpeer_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_relpeer_t *	ntudp_relpeer	= (ntudp_relpeer_t *)object_ptr;
	std::ostringstream	oss;

	oss << ntudp_relpeer->tunnel_db.size();

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
std::string ntudp_relpeer_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_relpeer_t *	ntudp_relpeer	= (ntudp_relpeer_t *)object_ptr;
	std::ostringstream	oss;

	oss << "title=\"";
	oss << ntudp_relpeer->tunnel_db.size()	<< " tunnel established or in establishment";
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
std::string ntudp_relpeer_wikidbg_t::wikidbg_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_relpeer_t *	ntudp_relpeer	= (ntudp_relpeer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", ntudp_relpeer), wikidbg_html("page_title_attr", ntudp_relpeer));
	oss << "relpeer(";
	oss << wikidbg_html("oneword", ntudp_relpeer);
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
httpd_err_t ntudp_relpeer_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request)	throw()
{
	ntudp_relpeer_t *	ntudp_relpeer	= (ntudp_relpeer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", ntudp_relpeer->ntudp_peer);

	// put the title
	oss << h.pagetitle("ntudp_relpeer_t Page");
	
	// display all the tunnels
	oss << h.s_table();
	std::set<ntudp_relpeer_tunnel_t *>::iterator	iter;
	for( iter = ntudp_relpeer->tunnel_db.begin(); iter != ntudp_relpeer->tunnel_db.end(); iter++ ){
		ntudp_relpeer_tunnel_t *	relpeer_tunnel	= *iter;
		// if it is the first element, add the tableheader
		if( iter == ntudp_relpeer->tunnel_db.begin() )
			oss << wikidbg_html("tableheader", relpeer_tunnel);
		// display this row
		oss << wikidbg_html("tablerow", relpeer_tunnel);
	}
	oss << h.e_table();
	oss << h.br();	
	
	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







