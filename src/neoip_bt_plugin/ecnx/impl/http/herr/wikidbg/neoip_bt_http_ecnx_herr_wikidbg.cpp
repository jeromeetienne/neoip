/*! \file
    \brief Declaration of the bt_http_ecnx_herr_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_http_ecnx_herr_wikidbg.hpp"
#include "neoip_bt_http_ecnx_herr.hpp"
#include "neoip_bt_http_ecnx_pool.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_http_ecnx_herr_t
 */
class bt_http_ecnx_herr_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr
								, httpd_request_t &request)	throw();
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
void	bt_http_ecnx_herr_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_http_ecnx_herr_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_http_ecnx_herr_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_http_ecnx_herr_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_http_ecnx_herr_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_http_ecnx_herr_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_http_ecnx_herr_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_http_ecnx_herr_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_herr_t *	ecnx_herr	= (bt_http_ecnx_herr_t *)object_ptr;
	std::ostringstream	oss;

	oss << ecnx_herr->get_hostport_str();

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
std::string bt_http_ecnx_herr_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_herr_t *	ecnx_herr	= (bt_http_ecnx_herr_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", ecnx_herr), wikidbg_html("page_title_attr", ecnx_herr));
	oss << wikidbg_html("oneword", ecnx_herr);
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
std::string bt_http_ecnx_herr_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "ecnx_herr established connection";
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
httpd_err_t bt_http_ecnx_herr_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_http_ecnx_herr_t *	ecnx_herr	= (bt_http_ecnx_herr_t *)object_ptr;
	bt_http_ecnx_pool_t *	ecnx_pool	= ecnx_herr->get_ecnx_pool();
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", ecnx_pool);
	// put the title
	oss << h.pagetitle("bt_http_ecnx_herr_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "hostport_str"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << ecnx_herr->get_hostport_str()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "last_error"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << ecnx_herr->last_error()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "retry_timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &ecnx_herr->retry_timeout)
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
std::string bt_http_ecnx_herr_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "hostport_str"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "retry_timeout"	<< h.e_b() << h.e_th();
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
std::string bt_http_ecnx_herr_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_herr_t *	ecnx_herr	= (bt_http_ecnx_herr_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", ecnx_herr)			<< h.e_td();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", &ecnx_herr->retry_timeout)	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







