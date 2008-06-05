/*! \file
    \brief Declaration of the apps_httpdetect_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_apps_httpdetect_wikidbg.hpp"
#include "neoip_apps_httpdetect.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref apps_httpdetect_http_t defines the wikidbg stuff for \ref apps_httpdetect_t
 */
class apps_httpdetect_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	apps_httpdetect_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, apps_httpdetect_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", apps_httpdetect_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", apps_httpdetect_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, apps_httpdetect_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string apps_httpdetect_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "apps_httpdetect";

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
std::string apps_httpdetect_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	apps_httpdetect_t *	apps_httpdetect	= (apps_httpdetect_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", apps_httpdetect), wikidbg_html("page_title_attr", apps_httpdetect));
	oss << wikidbg_html("oneword", apps_httpdetect);
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
std::string apps_httpdetect_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t apps_httpdetect_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	apps_httpdetect_t *	apps_httpdetect	= (apps_httpdetect_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("apps_httpdetect_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmlrpc_listener"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", apps_httpdetect->m_xmlrpc_listener)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmlrpc_resp"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", apps_httpdetect->m_xmlrpc_resp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_sresp"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", apps_httpdetect->m_http_sresp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "canon_name"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << apps_httpdetect->m_canon_name
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "version"			<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << apps_httpdetect->m_version
					<< h.e_td();
			oss << h.e_tr();
		oss << h.e_table();	
	oss << h.br();


	// return no error
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END







