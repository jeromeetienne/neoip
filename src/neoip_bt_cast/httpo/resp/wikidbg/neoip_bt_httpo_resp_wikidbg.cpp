/*! \file
    \brief Declaration of the bt_httpo_resp_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_httpo_resp_wikidbg.hpp"
#include "neoip_bt_httpo_resp.hpp"
#include "neoip_http_resp.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_httpo_resp_t
 */
class bt_httpo_resp_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_httpo_resp_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_httpo_resp_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_httpo_resp_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_httpo_resp_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_httpo_resp_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_httpo_resp_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_httpo_resp_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_httpo_resp_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_httpo_resp_t *	httpo_resp	= (bt_httpo_resp_t *)object_ptr;
	http_resp_t *		http_resp	= httpo_resp->m_resp_get;
	std::ostringstream	oss;

	oss << http_resp->get_listen_uri();

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
std::string bt_httpo_resp_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_httpo_resp_t *	httpo_resp	= (bt_httpo_resp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", httpo_resp), wikidbg_html("page_title_attr", httpo_resp));
	oss << wikidbg_html("oneword", httpo_resp);
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
std::string bt_httpo_resp_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	bt_httpo_resp_t *	httpo_resp	= (bt_httpo_resp_t *)object_ptr;	
	http_resp_t *		http_resp	= httpo_resp->m_resp_get;
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "bt_httpo_resp_t on " << http_resp->get_listen_uri();
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
httpd_err_t bt_httpo_resp_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_httpo_resp_t *	httpo_resp	= (bt_httpo_resp_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	
	// put the title
	oss << h.pagetitle("bt_httpo_resp_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_httpo"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", httpo_resp->httpo_listener())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_resp for GET"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", httpo_resp->m_resp_get)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_resp for HEAD"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", httpo_resp->m_resp_head)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(httpo_resp->callback)
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
std::string bt_httpo_resp_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "listen uri"		<< h.e_b() << h.e_th();
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
std::string bt_httpo_resp_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_httpo_resp_t *	httpo_resp	= (bt_httpo_resp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", httpo_resp)		<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}
NEOIP_NAMESPACE_END







