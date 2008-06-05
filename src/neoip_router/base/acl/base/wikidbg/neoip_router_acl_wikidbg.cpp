/*! \file
    \brief Declaration of the router_acl_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_router_acl_wikidbg.hpp"
#include "neoip_router_acl.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref router_acl_http_t defines the wikidbg stuff for \ref router_acl_t
 */
class router_acl_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	router_acl_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, router_acl_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", router_acl_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", router_acl_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, router_acl_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string router_acl_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	router_acl_t *		router_acl	= (router_acl_t *)object_ptr;
	std::ostringstream	oss;

	oss << "acl(" << router_acl->size() << ")";

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
std::string router_acl_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	router_acl_t *		router_acl	= (router_acl_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", router_acl), wikidbg_html("page_title_attr", router_acl));
	oss << wikidbg_html("oneword", router_acl);
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
std::string router_acl_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t router_acl_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	router_acl_t *		router_acl	= (router_acl_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("router_acl_t Page");

	// display the table of all the current router_full_t
	oss << h.s_sub1title() << "List of all current router_acl_item: "
					<< router_acl->item_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole router_peer_t::item_db
	router_acl_t::item_db_t &		item_db = router_acl->item_db;
	router_acl_t::item_db_t::iterator	iter;
	for(iter = item_db.begin(); iter != item_db.end(); iter++){
		const router_acl_item_t &	acl_item	= *iter;
		// if it is the first element, add the tableheader
		if( iter == item_db.begin() ){
			oss << h.s_tr();
			oss << h.s_th() << h.s_b() << "type"	<< h.e_b() << h.e_th();
			oss << h.s_th() << h.s_b() << "pattern"	<< h.e_b() << h.e_th();
			oss << h.e_tr();
		}
		// display this row
		oss << h.s_tr();
		oss << h.s_td() << acl_item.type()	<< h.e_td();
		oss << h.s_td() << acl_item.pattern()	<< h.e_td();
		oss << h.e_tr();
	}
	oss << h.e_table();	

	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







