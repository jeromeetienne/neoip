/*! \file
    \brief Declaration of the tls_itor_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_tls_itor_wikidbg.hpp"
#include "neoip_tls_itor.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref tls_itor_http_t defines the wikidbg stuff for \ref tls_itor_t
 */
class tls_itor_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	tls_itor_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, tls_itor_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", tls_itor_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", tls_itor_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, tls_itor_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string tls_itor_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "tls_itor_t";

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
std::string tls_itor_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	tls_itor_t *	tls_itor	= (tls_itor_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", tls_itor), wikidbg_html("page_title_attr", tls_itor));
	oss << wikidbg_html("oneword", tls_itor);
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
std::string tls_itor_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t tls_itor_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	tls_itor_t *		tls_itor	= (tls_itor_t *)object_ptr;
	std::ostringstream &	oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("tls_itor_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "tls_privctx"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " <<  wikidbg_html("oneword_pageurl", tls_itor->m_tls_privctx)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the buffers
	oss << h.sub1title("recv_buffer")	<< h.s_pre() << tls_itor->m_recv_buffer
						<< h.e_pre();
	oss << h.sub1title("xmit_buffer")	<< h.s_pre() << tls_itor->m_xmit_buffer
						<< h.e_pre();
	
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







