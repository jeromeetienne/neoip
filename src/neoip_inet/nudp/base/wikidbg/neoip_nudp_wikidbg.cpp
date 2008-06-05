/*! \file
    \brief Declaration of the nudp_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_nudp_wikidbg.hpp"
#include "neoip_nudp.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref nudp_http_t defines the wikidbg stuff for \ref nudp_t
 */
class nudp_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	nudp_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, nudp_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl"	, nudp_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, nudp_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"			, nudp_wikidbg_t::page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string nudp_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	nudp_t *	nudp	= (nudp_t *)object_ptr;
	std::ostringstream	oss;

	oss << nudp->get_listen_addr();

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
std::string nudp_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	nudp_t *	nudp	= (nudp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", nudp), wikidbg_html("page_title_attr", nudp));
	oss << wikidbg_html("oneword", nudp);
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
std::string nudp_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "nudp is for unconnected udp socket.";
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
httpd_err_t nudp_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	nudp_t *		nudp	= (nudp_t *)object_ptr;
	std::ostringstream	&oss 	= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("nudp_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Listen addr"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nudp->get_listen_addr()<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(nudp->callback);
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();	

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







