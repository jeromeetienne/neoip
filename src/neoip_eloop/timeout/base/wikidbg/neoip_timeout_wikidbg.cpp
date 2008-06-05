/*! \file
    \brief Declaration of the timeout_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_timeout_wikidbg.hpp"
#include "neoip_timeout.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref timeout_t
 */
class timeout_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static std::string	menu_item(const std::string &keyword, void *object_ptr)		throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	timeout_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, timeout_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", timeout_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", timeout_wikidbg_t::page_title_attr);
	keyword_db.insert_html("menu_item"	, timeout_wikidbg_t::menu_item);
	keyword_db.insert_page("page"		, timeout_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string timeout_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	timeout_t *		timeout	= (timeout_t *)object_ptr;	
	std::ostringstream	oss;

	if( !timeout->is_null() )	oss << timeout->get_expire_delay();
	else				oss << "null timeout_t";

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
std::string timeout_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	timeout_t *		timeout	= (timeout_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", timeout), wikidbg_html("page_title_attr", timeout));
	oss << wikidbg_html("oneword", timeout);
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
std::string timeout_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "timeout";
	oss << "\"";
	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu_item" keyword
 */
std::string timeout_wikidbg_t::menu_item(const std::string &keyword, void *object_ptr) throw()
{
	timeout_t *		timeout	= (timeout_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("page", timeout), wikidbg_html("page_title_attr", timeout));
	oss << wikidbg_html("oneword", timeout);
	oss << h.e_link();

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
httpd_err_t timeout_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	timeout_t *		timeout	= (timeout_t *)object_ptr;
	std::ostringstream	&oss 	= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("timeout_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "period"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << timeout->get_period()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "expire_delay"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << timeout->get_expire_delay()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "is_running"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << timeout->is_running()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(timeout->callback)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();
		
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







