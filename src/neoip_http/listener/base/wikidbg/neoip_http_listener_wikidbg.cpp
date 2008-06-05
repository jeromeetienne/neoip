/*! \file
    \brief Declaration of the http_listener_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_http_listener_wikidbg.hpp"
#include "neoip_http_listener.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref http_listener_http_t defines the wikidbg stuff for \ref http_listener_t
 */
class http_listener_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	http_listener_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, http_listener_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", http_listener_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", http_listener_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, http_listener_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string http_listener_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "http_listener";

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
std::string http_listener_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	http_listener_t *	http_listener	= (http_listener_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", http_listener), wikidbg_html("page_title_attr", http_listener));
	oss << wikidbg_html("oneword", http_listener);
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
std::string http_listener_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t http_listener_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	http_listener_t *		http_listener	= (http_listener_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("http_listener_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "socket_resp"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", http_listener->socket_resp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "listen_addr"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << http_listener->listen_addr()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current http_listener_cnx_t
	oss << h.s_sub1title() << "List of all current http_listener_cnx_t: "
					<< http_listener->cnx_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole http_listener_t::cnx_db
	std::list<http_listener_cnx_t *> &		cnx_db = http_listener->cnx_db;
	std::list<http_listener_cnx_t *>::iterator	iter_cnx;
	for( iter_cnx = cnx_db.begin(); iter_cnx != cnx_db.end(); iter_cnx++ ){
		http_listener_cnx_t *	http_listener_cnx	= *iter_cnx;
		// if it is the first element, add the tableheader
		if( iter_cnx == cnx_db.begin() )	oss << wikidbg_html("tableheader", http_listener_cnx);	
		// display this row
		oss << wikidbg_html("tablerow", http_listener_cnx);
	}
	oss << h.e_table();


	// display the table of all the current http_resp_t
	oss << h.s_sub1title() << "List of all current http_resp_t: "
					<< http_listener->resp_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole http_listener_t::resp_db
	std::list<http_resp_t *> &		resp_db = http_listener->resp_db;
	std::list<http_resp_t *>::iterator	iter_resp;
	for( iter_resp = resp_db.begin(); iter_resp != resp_db.end(); iter_resp++ ){
		http_resp_t *	http_resp	= *iter_resp;
		// if it is the first element, add the tableheader
		if( iter_resp == resp_db.begin() )	oss << wikidbg_html("tableheader", http_resp);	
		// display this row
		oss << wikidbg_html("tablerow", http_resp);
	}
	oss << h.e_table();
	// return no error
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END







