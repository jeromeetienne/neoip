/*! \file
    \brief Declaration of the http_client_pool_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_http_client_pool_wikidbg.hpp"
#include "neoip_http_client_pool.hpp"
#include "neoip_http_client_pool_stat_helper.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref http_client_pool_http_t defines the wikidbg stuff for \ref http_client_pool_t
 */
class http_client_pool_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	http_client_pool_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, http_client_pool_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", http_client_pool_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", http_client_pool_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, http_client_pool_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string http_client_pool_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "http_client_pool";

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
std::string http_client_pool_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	http_client_pool_t *	client_pool	= (http_client_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", client_pool), wikidbg_html("page_title_attr", client_pool));
	oss << wikidbg_html("oneword", client_pool);
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
std::string http_client_pool_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	http_client_pool_t *	client_pool	= (http_client_pool_t *)object_ptr;	
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "http_client_pool_t of " << client_pool->size() << " connections";
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
httpd_err_t http_client_pool_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	http_client_pool_t *	client_pool	= (http_client_pool_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	
	// put the title
	oss << h.pagetitle("http_client_pool_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cnx_timeout_delay"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << client_pool->get_profile().cnx_timeout_delay()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// display the http_client_pool_stat_t
	oss << h.sub1title("http_client_pool_stat_t");
	oss << http_client_pool_stat_helper_t::to_html(client_pool->pool_stat);
	
	// display the table of all the current http_client_pool_cnx_t
	std::list<http_client_pool_cnx_t *> &		cnx_db = client_pool->cnx_db;
	std::list<http_client_pool_cnx_t *>::iterator	iter;
	oss << h.s_sub1title() << "List of all pending http_client_pool_cnx_t: "
				<< cnx_db.size()
				<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole http_client_pool_t::cnx_db
	for( iter = cnx_db.begin(); iter != cnx_db.end(); iter++ ){
		http_client_pool_cnx_t *	pool_cnx	= *iter;
		// if it is the first element, add the tableheader
		if( iter == cnx_db.begin() )	oss << wikidbg_html("tableheader", pool_cnx);	
		// display this row
		oss << wikidbg_html("tablerow", pool_cnx);
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







