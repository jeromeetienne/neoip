/*! \file
    \brief Declaration of the xmlrpc_listener_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_listener_wikidbg.hpp"
#include "neoip_xmlrpc_listener.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref xmlrpc_listener_http_t defines the wikidbg stuff for \ref xmlrpc_listener_t
 */
class xmlrpc_listener_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	xmlrpc_listener_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, xmlrpc_listener_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", xmlrpc_listener_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", xmlrpc_listener_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, xmlrpc_listener_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string xmlrpc_listener_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "xmlrpc_listener_t";

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
std::string xmlrpc_listener_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	xmlrpc_listener_t *	xmlrpc_listener	= (xmlrpc_listener_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", xmlrpc_listener), wikidbg_html("page_title_attr", xmlrpc_listener));
	oss << wikidbg_html("oneword", xmlrpc_listener);
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
std::string xmlrpc_listener_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t xmlrpc_listener_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	xmlrpc_listener_t *	xmlrpc_listener	= (xmlrpc_listener_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("xmlrpc_listener_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_listener"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", xmlrpc_listener->m_http_listener)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_sresp_jsrest"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", xmlrpc_listener->m_sresp_jsrest)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_sresp_xmlrpc"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", xmlrpc_listener->m_sresp_xmlrpc)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current xmlrpc_resp_t
	oss << h.s_sub1title() << "List of all current xmlrpc_resp_t: "
					<< xmlrpc_listener->resp_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole xmlrpc_listener_t::resp_db
	std::list<xmlrpc_resp_t *> &		resp_db = xmlrpc_listener->resp_db;
	std::list<xmlrpc_resp_t *>::iterator	iter;
	for( iter = resp_db.begin(); iter != resp_db.end(); iter++ ){
		xmlrpc_resp_t *	xmlrpc_resp	= *iter;
		// if it is the first element, add the tableheader
		if( iter == resp_db.begin() )	oss << wikidbg_html("tableheader", xmlrpc_resp);	
		// display this row
		oss << wikidbg_html("tablerow", xmlrpc_resp);
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END







