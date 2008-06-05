/*! \file
    \brief Declaration of the casti_ctrl_wpage_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_casti_ctrl_wpage_wikidbg.hpp"
#include "neoip_casti_ctrl_wpage.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref casti_ctrl_wpage_http_t defines the wikidbg stuff for \ref casti_ctrl_wpage_t
 */
class casti_ctrl_wpage_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	casti_ctrl_wpage_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, casti_ctrl_wpage_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", casti_ctrl_wpage_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", casti_ctrl_wpage_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, casti_ctrl_wpage_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string casti_ctrl_wpage_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "casti_ctrl_wpage_t";

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
std::string casti_ctrl_wpage_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	casti_ctrl_wpage_t *	ctrl_wpage	= (casti_ctrl_wpage_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", ctrl_wpage), wikidbg_html("page_title_attr", ctrl_wpage));
	oss << wikidbg_html("oneword", ctrl_wpage);
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
std::string casti_ctrl_wpage_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t casti_ctrl_wpage_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	casti_ctrl_wpage_t *	ctrl_wpage	= (casti_ctrl_wpage_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("casti_ctrl_wpage_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "casti_apps"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ctrl_wpage->m_casti_apps)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmlrpc_listener"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ctrl_wpage->m_xmlrpc_listener)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmlrpc_resp"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ctrl_wpage->m_xmlrpc_resp)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();


	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







