/*! \file
    \brief Declaration of the rtmp_parse_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_rtmp_parse_wikidbg.hpp"
#include "neoip_rtmp_parse.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref rtmp_parse_http_t defines the wikidbg stuff for \ref rtmp_parse_t
 */
class rtmp_parse_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	rtmp_parse_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw()
{
	// add the keywords
	keyword_db.insert_html("oneword"	, rtmp_parse_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", rtmp_parse_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", rtmp_parse_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, rtmp_parse_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, rtmp_parse_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, rtmp_parse_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string rtmp_parse_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	//rtmp_parse_t *		rtmp_parse	= (rtmp_parse_t *)object_ptr;
	std::ostringstream	oss;

	oss << "rtmp_parse_t";

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
std::string rtmp_parse_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	rtmp_parse_t *		rtmp_parse	= (rtmp_parse_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", rtmp_parse), wikidbg_html("page_title_attr", rtmp_parse));
	oss << wikidbg_html("oneword", rtmp_parse);
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
std::string rtmp_parse_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t rtmp_parse_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	rtmp_parse_t *		rtmp_parse	= (rtmp_parse_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("rtmp_parse_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "m_buffer"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << h.pre(rtmp_parse->m_buffer.to_string())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "zerotimer_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &rtmp_parse->zerotimer)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(rtmp_parse->callback)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// display the table of all the current rtmp_parse_chanctx_t
	oss << h.s_sub1title() << "List of all current rtmp_parse_chanctx_t: "
					<< rtmp_parse->chanctx_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole cam_listener_t::resp_chanctx_db
	std::list<rtmp_parse_chanctx_t *> &		chanctx_db = rtmp_parse->chanctx_db;
	std::list<rtmp_parse_chanctx_t *>::iterator	iter_chanctx;
	for( iter_chanctx = chanctx_db.begin(); iter_chanctx != chanctx_db.end(); iter_chanctx++ ){
		rtmp_parse_chanctx_t *	resp_cnx	= *iter_chanctx;
		// if it is the first element, add the tableheader
		if( iter_chanctx == chanctx_db.begin() )	oss << wikidbg_html("tableheader", resp_cnx);
		// display this row
		oss << wikidbg_html("tablerow", resp_cnx);
	}
	oss << h.e_table();

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
std::string rtmp_parse_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
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
std::string rtmp_parse_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	rtmp_parse_t *		rtmp_parse	= (rtmp_parse_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", rtmp_parse)	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}
NEOIP_NAMESPACE_END







