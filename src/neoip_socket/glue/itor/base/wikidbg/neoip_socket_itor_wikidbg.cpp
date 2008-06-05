/*! \file
    \brief Declaration of the socket_itor_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_socket_itor_wikidbg.hpp"
#include "neoip_socket_itor.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref socket_itor_wikidbg_t defines the wikidbg stuff for \ref socket_itor_t
 */
class socket_itor_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	socket_itor_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, socket_itor_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl"	, socket_itor_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, socket_itor_wikidbg_t::page_title_attr);	
	keyword_db.insert_page("page"			, socket_itor_wikidbg_t::page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string socket_itor_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	socket_itor_t *		socket_itor	= (socket_itor_t *)object_ptr;	
	std::ostringstream	oss;

	oss << "socket_itor to " << socket_itor->remote_addr();

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
std::string socket_itor_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	socket_itor_t *		socket_itor	= (socket_itor_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", socket_itor), wikidbg_html("page_title_attr", socket_itor));
	oss << wikidbg_html("oneword", socket_itor);
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
std::string socket_itor_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t socket_itor_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	socket_itor_t *		socket_itor	= (socket_itor_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("socket_itor_t Page");
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "socket_domain"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << socket_itor->domain()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "local_addr"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << socket_itor->local_addr()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote_addr"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << socket_itor->remote_addr()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(socket_itor->callback)	<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	// displayt the socket_itor_vapi_t subpage
	oss << wikidbg_html("subpage", socket_itor->itor_vapi());

	// return no error
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END







