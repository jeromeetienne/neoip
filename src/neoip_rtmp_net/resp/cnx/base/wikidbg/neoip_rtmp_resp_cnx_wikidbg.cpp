/*! \file
    \brief Declaration of the rtmp_resp_cnx_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_rtmp_resp_cnx_wikidbg.hpp"
#include "neoip_rtmp_resp_cnx.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref rtmp_resp_cnx_http_t defines the wikidbg stuff for \ref rtmp_resp_cnx_t
 */
class rtmp_resp_cnx_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	rtmp_resp_cnx_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw()
{
	// add the keywords
	keyword_db.insert_html("oneword"	, rtmp_resp_cnx_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", rtmp_resp_cnx_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", rtmp_resp_cnx_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, rtmp_resp_cnx_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, rtmp_resp_cnx_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, rtmp_resp_cnx_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string rtmp_resp_cnx_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	rtmp_resp_cnx_t *		rtmp_resp_cnx	= (rtmp_resp_cnx_t *)object_ptr;
	std::ostringstream	oss;

	oss << "rtmp_resp_cnx on " << rtmp_resp_cnx->socket_full;

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
std::string rtmp_resp_cnx_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	rtmp_resp_cnx_t *		rtmp_resp_cnx	= (rtmp_resp_cnx_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", rtmp_resp_cnx), wikidbg_html("page_title_attr", rtmp_resp_cnx));
	oss << wikidbg_html("oneword", rtmp_resp_cnx);
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
std::string rtmp_resp_cnx_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t rtmp_resp_cnx_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	rtmp_resp_cnx_t *	rtmp_resp_cnx	= (rtmp_resp_cnx_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("rtmp_resp_cnx_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "rtmp_resp_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", rtmp_resp_cnx->rtmp_resp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "socket_full_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", rtmp_resp_cnx->socket_full)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "rtmp_resp_cnx_state_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << rtmp_resp_cnx->m_state
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "m_recved_data"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << h.pre(rtmp_resp_cnx->m_recved_data.to_string())
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// return no error
	return httpd_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//rtmp_resp_cnx
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "tableheader" keyword
 */
std::string rtmp_resp_cnx_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
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
std::string rtmp_resp_cnx_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	rtmp_resp_cnx_t *	rtmp_resp_cnx	= (rtmp_resp_cnx_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", rtmp_resp_cnx)	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}
NEOIP_NAMESPACE_END







