/*! \file
    \brief Declaration of the oload_flash_xdom_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_oload_flash_xdom_wikidbg.hpp"
#include "neoip_oload_flash_xdom.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref oload_flash_xdom_http_t defines the wikidbg stuff for \ref oload_flash_xdom_t
 */
class oload_flash_xdom_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	oload_flash_xdom_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, oload_flash_xdom_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", oload_flash_xdom_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", oload_flash_xdom_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, oload_flash_xdom_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string oload_flash_xdom_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "oload_flash_xdom_t";

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
std::string oload_flash_xdom_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	oload_flash_xdom_t *	flash_xdom	= (oload_flash_xdom_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", flash_xdom), wikidbg_html("page_title_attr", flash_xdom));
	oss << wikidbg_html("oneword", flash_xdom);
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
std::string oload_flash_xdom_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t oload_flash_xdom_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	oload_flash_xdom_t *	flash_xdom	= (oload_flash_xdom_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("oload_flash_xdom_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_sresp"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", flash_xdom->m_http_sresp)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();


	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







