/*! \file
    \brief Declaration of the bt_http_ecnx_pool_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_http_ecnx_pool_wikidbg.hpp"
#include "neoip_bt_http_ecnx_pool.hpp"
#include "neoip_bt_http_ecnx_cnx.hpp"
#include "neoip_bt_http_ecnx_cnx_stats_helper.hpp"
#include "neoip_bt_http_ecnx_herr.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_http_ecnx_pool_t
 */
class bt_http_ecnx_pool_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
private:
	static double			cpu_total_recv_rate(bt_http_ecnx_pool_t *ecnx_pool)	throw();
	static bt_http_ecnx_cnx_stats_t	cpu_total_cnx_stats(bt_http_ecnx_pool_t *ecnx_pool)	throw();
public:
	static std::string	menu(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	cnx_oneword(const std::string &keyword, void *object_ptr)	throw();
	static std::string	cnx_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	cnx_menu_item(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	cnx_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
	
	static std::string	herr_oneword(const std::string &keyword, void *object_ptr)	throw();
	static std::string	herr_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	herr_menu_item(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	herr_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_http_ecnx_pool_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("menu"		, bt_http_ecnx_pool_wikidbg_t::menu);	
	keyword_db.insert_html("oneword"	, bt_http_ecnx_pool_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_http_ecnx_pool_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_http_ecnx_pool_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_http_ecnx_pool_wikidbg_t::page);
	
	keyword_db.insert_html("cnx_oneword"		, bt_http_ecnx_pool_wikidbg_t::cnx_oneword);
	keyword_db.insert_html("cnx_page_title_attr"	, bt_http_ecnx_pool_wikidbg_t::cnx_page_title_attr);
	keyword_db.insert_html("cnx_menu_item"		, bt_http_ecnx_pool_wikidbg_t::cnx_menu_item);		
	keyword_db.insert_page("cnx_page"		, bt_http_ecnx_pool_wikidbg_t::cnx_page);

	keyword_db.insert_html("herr_oneword"		, bt_http_ecnx_pool_wikidbg_t::herr_oneword);
	keyword_db.insert_html("herr_page_title_attr"	, bt_http_ecnx_pool_wikidbg_t::herr_page_title_attr);
	keyword_db.insert_html("herr_menu_item"		, bt_http_ecnx_pool_wikidbg_t::herr_menu_item);		
	keyword_db.insert_page("herr_page"		, bt_http_ecnx_pool_wikidbg_t::herr_page);}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  private function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Compute the total_recv_rate for all the bt_http_ecnx_cnx_t
 */
double	bt_http_ecnx_pool_wikidbg_t::cpu_total_recv_rate(bt_http_ecnx_pool_t *ecnx_pool)	throw()
{
	double	total_recv_rate	= 0.0;
	// compute the total_recv_rate
	std::list<bt_http_ecnx_cnx_t *> &		ecnx_cnx_db = ecnx_pool->ecnx_cnx_db;
	std::list<bt_http_ecnx_cnx_t *>::iterator	iter;
	// go thru the whole bt_http_ecnx_pool_t::cnx_db
	for( iter = ecnx_cnx_db.begin(); iter != ecnx_cnx_db.end(); iter++ ){
		bt_http_ecnx_cnx_t *	ecnx_cnx	= *iter;
		total_recv_rate	+= ecnx_cnx->recv_rate_avg();
	}
	// return the just computed value
	return total_recv_rate;
}

/** \brief Compute the total_recv_rate for all the bt_http_ecnx_cnx_t
 */
bt_http_ecnx_cnx_stats_t	bt_http_ecnx_pool_wikidbg_t::cpu_total_cnx_stats(bt_http_ecnx_pool_t *ecnx_pool)	throw()
{
	bt_http_ecnx_cnx_stats_t	total_cnx_stats;
	// compute the total_recv_rate
	std::list<bt_http_ecnx_cnx_t *> &		ecnx_cnx_db = ecnx_pool->ecnx_cnx_db;
	std::list<bt_http_ecnx_cnx_t *>::iterator	iter;
	// go thru the whole bt_http_ecnx_pool_t::cnx_db
	for( iter = ecnx_cnx_db.begin(); iter != ecnx_cnx_db.end(); iter++ ){
		bt_http_ecnx_cnx_t *	ecnx_cnx	= *iter;
		total_cnx_stats	+= ecnx_cnx->cnx_stats();
	}
	// return the just computed value
	return total_cnx_stats;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu" keyword
 */
std::string bt_http_ecnx_pool_wikidbg_t::menu(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_pool_t *	ecnx_pool	= (bt_http_ecnx_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// build displayed html itself
	oss << h.s_link(wikidbg_url("page", ecnx_pool), "title=\"Main bt_http_ecnx_pool_t page\"")
					<< h.b("http_ecnx_pool:") << h.e_link();
	oss << " ";
	oss << wikidbg_html("cnx_menu_item", ecnx_pool);
	oss << " / ";
	oss << wikidbg_html("herr_menu_item", ecnx_pool);

	oss << h.br();

	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_http_ecnx_pool_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "HTTP ecnx_pool";

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
std::string bt_http_ecnx_pool_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_pool_t *	ecnx_pool	= (bt_http_ecnx_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", ecnx_pool), wikidbg_html("page_title_attr", ecnx_pool));
	oss << wikidbg_html("oneword", ecnx_pool);
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
std::string bt_http_ecnx_pool_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "the external connection pool for http";
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
httpd_err_t bt_http_ecnx_pool_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_http_ecnx_pool_t *	ecnx_pool	= (bt_http_ecnx_pool_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	double			total_recv_rate	= cpu_total_recv_rate(ecnx_pool);

	// add the menu
	oss << wikidbg_html("menu", ecnx_pool);
	// put the title
	oss << h.pagetitle("bt_http_ecnx_pool_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ecnx_pool->get_swarm())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_client_pool"<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ecnx_pool->http_client_pool)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "total_recv_rate"<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string((uint64_t)total_recv_rate)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();
	
	// display the bt_http_ecnx_cnx_stats_t
	bt_http_ecnx_cnx_stats_t	total_cnx_stats	= cpu_total_cnx_stats(ecnx_pool);
	oss << h.sub1title("total bt_http_ecnx_cnx_stats_t :");
	oss << bt_http_ecnx_cnx_stats_helper_t::to_html(total_cnx_stats);	

	// return no error
	return httpd_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             bt_http_ecnx_cnx_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "cnx_oneword" keyword
 */
std::string bt_http_ecnx_pool_wikidbg_t::cnx_oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_pool_t *	ecnx_pool	= (bt_http_ecnx_pool_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "(" << ecnx_pool->ecnx_cnx_db.size() << ")";

	// return the built string
	return oss.str();
}


/** \brief Handle the "cnx_page_title_attr" keyword
 */
std::string bt_http_ecnx_pool_wikidbg_t::cnx_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_pool_t *	ecnx_pool	= (bt_http_ecnx_pool_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << ecnx_pool->ecnx_cnx_db.size() << " initiators";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "cnx_menu_item" keyword
 */
std::string bt_http_ecnx_pool_wikidbg_t::cnx_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_pool_t *	ecnx_pool	= (bt_http_ecnx_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("cnx_page", ecnx_pool), wikidbg_html("cnx_page_title_attr", ecnx_pool));
	oss << "cnx";
	oss << wikidbg_html("cnx_oneword", ecnx_pool);
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "cnx_page" keyword
 */
httpd_err_t bt_http_ecnx_pool_wikidbg_t::cnx_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_http_ecnx_pool_t *	ecnx_pool	= (bt_http_ecnx_pool_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", ecnx_pool);
	// put the title
	oss << h.pagetitle("bt_http_ecnx_cnx_t Page");

	// display the table of all the current bt_http_ecnx_cnx_t
	std::list<bt_http_ecnx_cnx_t *> &		ecnx_cnx_db = ecnx_pool->ecnx_cnx_db;
	std::list<bt_http_ecnx_cnx_t *>::iterator	iter;
	oss << h.s_sub1title() << "List of all pending bt_http_ecnx_cnx_t: "
				<< ecnx_cnx_db.size()
				<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_http_ecnx_pool_t::cnx_db
	for( iter = ecnx_cnx_db.begin(); iter != ecnx_cnx_db.end(); iter++ ){
		bt_http_ecnx_cnx_t *	ecnx_cnx	= *iter;
		// if it is the first element, add the tableheader
		if( iter == ecnx_cnx_db.begin() )	oss << wikidbg_html("tableheader", ecnx_cnx);	
		// display this row
		oss << wikidbg_html("tablerow", ecnx_cnx);
	}
	oss << h.e_table();

	
	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             bt_http_ecnx_herr_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "herr_oneword" keyword
 */
std::string bt_http_ecnx_pool_wikidbg_t::herr_oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_pool_t *	ecnx_pool	= (bt_http_ecnx_pool_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "(" << ecnx_pool->ecnx_herr_db.size() << ")";

	// return the built string
	return oss.str();
}


/** \brief Handle the "herr_page_title_attr" keyword
 */
std::string bt_http_ecnx_pool_wikidbg_t::herr_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_pool_t *	ecnx_pool	= (bt_http_ecnx_pool_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << ecnx_pool->ecnx_herr_db.size() << " error handler";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "herr_menu_item" keyword
 */
std::string bt_http_ecnx_pool_wikidbg_t::herr_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_pool_t *	ecnx_pool	= (bt_http_ecnx_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("herr_page", ecnx_pool), wikidbg_html("herr_page_title_attr", ecnx_pool));
	oss << "herr";
	oss << wikidbg_html("herr_oneword", ecnx_pool);
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "herr_page" keyword
 */
httpd_err_t bt_http_ecnx_pool_wikidbg_t::herr_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_http_ecnx_pool_t *	ecnx_pool	= (bt_http_ecnx_pool_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", ecnx_pool);
	// put the title
	oss << h.pagetitle("bt_http_ecnx_herr_t List Page");

	// display the table of all the current bt_http_ecnx_herr_t
	std::map<std::string, bt_http_ecnx_herr_t *> &		ecnx_herr_db = ecnx_pool->ecnx_herr_db;
	std::map<std::string, bt_http_ecnx_herr_t *>::iterator	herr_iter;
	oss << h.s_sub1title() << "List of all pending bt_http_ecnx_herr_t: "
				<< ecnx_herr_db.size()
				<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_http_ecnx_pool_t::herr_db
	for( herr_iter = ecnx_herr_db.begin(); herr_iter != ecnx_herr_db.end(); herr_iter++ ){
		bt_http_ecnx_herr_t *	ecnx_herr	= herr_iter->second;
		// if it is the first element, add the tableheader
		if( herr_iter == ecnx_herr_db.begin() )	oss << wikidbg_html("tableheader", ecnx_herr);	
		// display this row
		oss << wikidbg_html("tablerow", ecnx_herr);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







