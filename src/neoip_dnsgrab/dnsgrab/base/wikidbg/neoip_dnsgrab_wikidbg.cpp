/*! \file
    \brief Declaration of the dnsgrab_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_dnsgrab_wikidbg.hpp"
#include "neoip_dnsgrab.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref dnsgrab_http_t defines the wikidbg stuff for \ref dnsgrab_t
 */
class dnsgrab_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	dnsgrab_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, dnsgrab_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", dnsgrab_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", dnsgrab_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, dnsgrab_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string dnsgrab_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "a dnsgrab";

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
std::string dnsgrab_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	dnsgrab_t *		dnsgrab	= (dnsgrab_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", dnsgrab), wikidbg_html("page_title_attr", dnsgrab));
	oss << wikidbg_html("oneword", dnsgrab);
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
std::string dnsgrab_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t dnsgrab_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	dnsgrab_t *		dnsgrab	= (dnsgrab_t *)object_ptr;
	std::ostringstream	&oss 	= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("dnsgrab_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "tcp_resp"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", dnsgrab->tcp_resp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(dnsgrab->callback)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current dnsgrab_cnx_t
	oss << h.s_sub1title() << "List of all current dnsgrab_cnx_t: "
					<< dnsgrab->cnx_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole dnsgrab_t::cnx_db
	std::list<dnsgrab_cnx_t *> &		cnx_db = dnsgrab->cnx_db;
	std::list<dnsgrab_cnx_t *>::iterator	iter;
	for( iter = cnx_db.begin(); iter != cnx_db.end(); iter++ ){
		dnsgrab_cnx_t *	dnsgrab_cnx	= *iter;
		// if it is the first element, add the tableheader
		if( iter == cnx_db.begin() )	oss << wikidbg_html("tableheader", dnsgrab_cnx);	
		// display this row
		oss << wikidbg_html("tablerow", dnsgrab_cnx);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







