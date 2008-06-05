/*! \file
    \brief Declaration of the router_acache_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_router_acache_wikidbg.hpp"
#include "neoip_router_acache.hpp"
#include "neoip_router_peer.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref router_acache_http_t defines the wikidbg stuff for \ref router_acache_t
 */
class router_acache_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	router_acache_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, router_acache_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", router_acache_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", router_acache_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, router_acache_wikidbg_t::page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string router_acache_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	router_acache_t *	router_acache	= (router_acache_t *)object_ptr;
	std::ostringstream	oss;

	oss << "acache (" << router_acache->size() << ")";

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
std::string router_acache_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	router_acache_t *	router_acache	= (router_acache_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", router_acache), wikidbg_html("page_title_attr", router_acache));
	oss << wikidbg_html("oneword", router_acache);
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
std::string router_acache_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	router_acache_t *	router_acache	= (router_acache_t *)object_ptr;
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "Inner address cache (containing " << router_acache->size() << " elements)";
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
httpd_err_t router_acache_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	router_acache_t *	router_acache	= (router_acache_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("router_acache_t Page");

	// display the table of all the current router_itor_t
	oss << h.s_sub1title() << "List of all current item in the router_acache_t: "
					<< router_acache->size() << h.e_sub1title();
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "local_iaddr"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "remote_iaddr"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "remote_dnsname"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "expire_delay"		<< h.e_b() << h.e_th();
	oss << h.e_tr();
	// go thru the whole router_acache_t
	for(size_t i = 0; i < router_acache->size(); i++){
		const router_acache_item_t &	acache_item	= (*router_acache)[i];
		oss << h.s_tr();
		oss << h.s_td() << acache_item.local_iaddr()		<< h.e_td();
		oss << h.s_td() << acache_item.remote_iaddr()		<< h.e_td();
		oss << h.s_td() << acache_item.remote_dnsname()		<< h.e_td();
		oss << h.s_td() << string_t::delay_string(router_acache->expire_delay_at(i))	<< h.e_td();
		oss << h.e_tr();
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







