/*! \file
    \brief Declaration of the xmlrpc_resp_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_resp_wikidbg.hpp"
#include "neoip_xmlrpc_resp.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref xmlrpc_resp_t
 */
class xmlrpc_resp_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	xmlrpc_resp_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, xmlrpc_resp_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", xmlrpc_resp_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", xmlrpc_resp_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, xmlrpc_resp_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, xmlrpc_resp_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, xmlrpc_resp_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string xmlrpc_resp_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "xmlrpc_resp_t";

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
std::string xmlrpc_resp_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	xmlrpc_resp_t *		xmlrpc_resp	= (xmlrpc_resp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", xmlrpc_resp), wikidbg_html("page_title_attr", xmlrpc_resp));
	oss << wikidbg_html("oneword", xmlrpc_resp);
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
std::string xmlrpc_resp_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "xmlrpc_resp_t";
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
httpd_err_t xmlrpc_resp_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	xmlrpc_resp_t *		xmlrpc_resp	= (xmlrpc_resp_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	
	// put the title
	oss << h.pagetitle("xmlrpc_resp_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmlrpc_listener"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", xmlrpc_resp->m_xmlrpc_listener)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(xmlrpc_resp->callback)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// display the table of all the current bt_xmlrpc_resp_cnx_t
	oss << h.s_sub1title() << "List of all current method_name: "
					<< xmlrpc_resp->m_method_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_xmlrpc_resp_t::m_method_db
	std::list<std::string> &		m_method_db = xmlrpc_resp->m_method_db;
	std::list<std::string>::iterator	iter;
	for( iter = m_method_db.begin(); iter != m_method_db.end(); iter++ ){
		const std::string &	method_name	= *iter;
		// if it is the first element, add the tableheader
		if( iter == m_method_db.begin() ){
			oss << h.s_tr();
			oss << h.s_th() << h.s_b() << "method_name"<< h.e_b() << h.e_th();
			oss << h.e_tr();
		}	
		// display this row
		oss << h.s_tr();
		oss << h.s_td() << method_name	<< h.e_td();
		oss << h.e_tr();
	}
	oss << h.e_table();
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
std::string xmlrpc_resp_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "xmlrpc_resp"		<< h.e_b() << h.e_th();
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
std::string xmlrpc_resp_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	xmlrpc_resp_t *		xmlrpc_resp	= (xmlrpc_resp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", xmlrpc_resp)		<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}
NEOIP_NAMESPACE_END







