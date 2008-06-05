/*! \file
    \brief Declaration of the tcp_full_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_tcp_full_wikidbg.hpp"
#include "neoip_tcp_full.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref tcp_full_http_t defines the wikidbg stuff for \ref tcp_full_t
 */
class tcp_full_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	tcp_full_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, tcp_full_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", tcp_full_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", tcp_full_wikidbg_t::page_title_attr);
	keyword_db.insert_html("subpage"	, tcp_full_wikidbg_t::subpage);
	keyword_db.insert_page("page"		, tcp_full_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string tcp_full_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	tcp_full_t *		tcp_full	= (tcp_full_t *)object_ptr;
	std::ostringstream	oss;

	oss << tcp_full->local_addr() << "-" << tcp_full->remote_addr();

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
std::string tcp_full_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	tcp_full_t *		tcp_full	= (tcp_full_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", tcp_full), wikidbg_html("page_title_attr", tcp_full));
	oss << wikidbg_html("oneword", tcp_full);
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
std::string tcp_full_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
std::string tcp_full_wikidbg_t::subpage(const std::string &keyword, void *object_ptr) throw()
{
	tcp_full_t *		tcp_full	= (tcp_full_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	// put the html data
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "local_addr"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << tcp_full->local_addr()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote_addr"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << tcp_full->remote_addr()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(tcp_full->callback)	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "rcvdata_maxlen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(tcp_full->rcvdata_maxlen())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "maysend_tshold"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(tcp_full->maysend_tshold())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmitbuf_maxlen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(tcp_full->xmitbuf_maxlen())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmitbuf_usedlen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(tcp_full->xmitbuf_usedlen())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmitbuf_freelen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(tcp_full->xmitbuf_freelen())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmit_limit"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", tcp_full->m_xmit_limit)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "recv_limit"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", tcp_full->m_recv_limit)
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
httpd_err_t tcp_full_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	tcp_full_t *		tcp_full	= (tcp_full_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	// put the title
	oss << h.pagetitle("tcp_full_t Page");
	// put the subpage
	oss << h.br();
	oss << wikidbg_html("subpage", tcp_full);	
	oss << h.br();		
	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







