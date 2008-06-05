/*! \file
    \brief Declaration of the udp_resp_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_udp_resp_wikidbg.hpp"
#include "neoip_udp_resp.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref udp_resp_wikidbg_t defines the wikidbg stuff for \ref udp_resp_t
 */
class udp_resp_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	wikidbg_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	udp_resp_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, udp_resp_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("oneword_pageurl"	, udp_resp_wikidbg_t::wikidbg_oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, udp_resp_wikidbg_t::wikidbg_page_title_attr);	
	keyword_db.insert_page("page"			, udp_resp_wikidbg_t::wikidbg_page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string udp_resp_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	udp_resp_t *		udp_resp	= (udp_resp_t *)object_ptr;	
	std::ostringstream	oss;

	oss << "udp_resp " << udp_resp->listen_addr();

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
std::string udp_resp_wikidbg_t::wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	udp_resp_t *		udp_resp	= (udp_resp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", udp_resp), wikidbg_html("page_title_attr", udp_resp));
	oss << wikidbg_html("oneword", udp_resp);
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
std::string udp_resp_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t udp_resp_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	udp_resp_t *		udp_resp	= (udp_resp_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("udp_resp_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "listen_addr"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << udp_resp->listen_addr()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "fdwatch_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", udp_resp->fdwatch)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(udp_resp->callback)	<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// return no error
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END







