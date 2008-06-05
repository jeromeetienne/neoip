/*! \file
    \brief Declaration of the ndiag_cacheport_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ndiag_cacheport_wikidbg.hpp"
#include "neoip_ndiag_cacheport.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ndiag_cacheport_http_t defines the wikidbg stuff for \ref ndiag_cacheport_t
 */
class ndiag_cacheport_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	ndiag_cacheport_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, ndiag_cacheport_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", ndiag_cacheport_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", ndiag_cacheport_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, ndiag_cacheport_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string ndiag_cacheport_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	ndiag_cacheport_t *	ndiag_cacheport	= (ndiag_cacheport_t *)object_ptr;	
	std::ostringstream	oss;

	oss << "ndiag_cacheport (" << ndiag_cacheport->size() << ")";

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
std::string ndiag_cacheport_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	ndiag_cacheport_t *	ndiag_cacheport	= (ndiag_cacheport_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", ndiag_cacheport), wikidbg_html("page_title_attr", ndiag_cacheport));
	oss << wikidbg_html("oneword", ndiag_cacheport);
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
std::string ndiag_cacheport_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t ndiag_cacheport_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	ndiag_cacheport_t *	ndiag_cacheport	= (ndiag_cacheport_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("ndiag_cacheport_t Page");


	// display the table of all the current router_itor_t
	oss << h.s_sub1title() << "List of all current item in the ndiag_cacheport_t: "
					<< ndiag_cacheport->item_db.size() << h.e_sub1title();
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "key"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "sockfam"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "port"		<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	const ndiag_cacheport_t::item_db_t &		item_db = ndiag_cacheport->item_db;
	// serialize each item
	for(size_t i = 0; i < item_db.size(); i++){
		const ndiag_cacheport_item_t &item	= item_db[i];
		oss << h.s_tr();
		oss << h.s_td() << item.key()		<< h.e_td();
		oss << h.s_td() << item.sockfam()	<< h.e_td();
		oss << h.s_td() << item.port()		<< h.e_td();
		oss << h.e_tr();
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







