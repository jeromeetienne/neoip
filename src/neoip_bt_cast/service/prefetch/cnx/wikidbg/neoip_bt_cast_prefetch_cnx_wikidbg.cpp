/*! \file
    \brief Declaration of the bt_cast_prefetch_cnx_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_cast_prefetch_cnx_wikidbg.hpp"
#include "neoip_bt_cast_prefetch_cnx.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_cast_prefetch_cnx_http_t defines the wikidbg stuff for \ref bt_cast_prefetch_cnx_t
 */
class bt_cast_prefetch_cnx_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_cast_prefetch_cnx_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_cast_prefetch_cnx_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_cast_prefetch_cnx_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_cast_prefetch_cnx_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_cast_prefetch_cnx_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_cast_prefetch_cnx_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_cast_prefetch_cnx_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_cast_prefetch_cnx_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_cast_prefetch_cnx_t *prefetch_cnx	= (bt_cast_prefetch_cnx_t *)object_ptr;
	std::ostringstream	oss;

	oss << "cast_prefetch for " << prefetch_cnx->http_uri();

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
std::string bt_cast_prefetch_cnx_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_cast_prefetch_cnx_t *prefetch_cnx	= (bt_cast_prefetch_cnx_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", prefetch_cnx), wikidbg_html("page_title_attr", prefetch_cnx));
	oss << wikidbg_html("oneword", prefetch_cnx);
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
std::string bt_cast_prefetch_cnx_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_cast_prefetch_cnx_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_cast_prefetch_cnx_t *prefetch_cnx	= (bt_cast_prefetch_cnx_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_cast_prefetch_cnx_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_cast_prefetch_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", prefetch_cnx->m_cast_prefetch)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_uri"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << prefetch_cnx->http_uri()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "m_http_client"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", prefetch_cnx->m_http_client)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "m_socket_full"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", prefetch_cnx->m_socket_full)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// return no error
	return httpd_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "tableheader" keyword
 */
std::string bt_cast_prefetch_cnx_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "object"		<< h.e_b() << h.e_th();
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
std::string bt_cast_prefetch_cnx_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_cast_prefetch_cnx_t *prefetch_cnx	= (bt_cast_prefetch_cnx_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", prefetch_cnx)	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}
NEOIP_NAMESPACE_END







