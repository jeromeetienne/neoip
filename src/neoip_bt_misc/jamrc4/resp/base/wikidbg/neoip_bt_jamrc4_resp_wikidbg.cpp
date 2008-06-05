/*! \file
    \brief Declaration of the bt_jamrc4_resp_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_jamrc4_resp_wikidbg.hpp"
#include "neoip_bt_jamrc4_resp.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_jamrc4_resp_http_t defines the wikidbg stuff for \ref bt_jamrc4_resp_t
 */
class bt_jamrc4_resp_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static std::string	subpage(const std::string &keyword, void *object_ptr)		throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_jamrc4_resp_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_jamrc4_resp_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_jamrc4_resp_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_jamrc4_resp_wikidbg_t::page_title_attr);
	keyword_db.insert_html("subpage"	, bt_jamrc4_resp_wikidbg_t::subpage);
	keyword_db.insert_page("page"		, bt_jamrc4_resp_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_jamrc4_resp_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bt_jamrc4_resp_t";

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
std::string bt_jamrc4_resp_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_jamrc4_resp_t *	jamrc4_resp	= (bt_jamrc4_resp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", jamrc4_resp), wikidbg_html("page_title_attr", jamrc4_resp));
	oss << wikidbg_html("oneword", jamrc4_resp);
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
std::string bt_jamrc4_resp_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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

/** \brief Handle the "subpage" keyword
 */
std::string bt_jamrc4_resp_wikidbg_t::subpage(const std::string &keyword, void *object_ptr) throw()
{
	bt_jamrc4_resp_t *	jamrc4_resp	= (bt_jamrc4_resp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	// put the html data
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "resp_nonce"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << jamrc4_resp->m_resp_nonce
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "m_recv_buffer"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << jamrc4_resp->m_recv_buffer
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "m_xmit_buffer"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << jamrc4_resp->m_xmit_buffer
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
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
httpd_err_t bt_jamrc4_resp_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_jamrc4_resp_t *	jamrc4_resp	= (bt_jamrc4_resp_t *)object_ptr;
	std::ostringstream &	oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_jamrc4_resp_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "resp_nonce"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << jamrc4_resp->m_resp_nonce
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "m_recv_buffer"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << jamrc4_resp->m_recv_buffer
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "m_xmit_buffer"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << jamrc4_resp->m_xmit_buffer
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
		
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







