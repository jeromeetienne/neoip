/*! \file
    \brief Declaration of the router_itor_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_router_itor_wikidbg.hpp"
#include "neoip_router_itor.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref router_itor_http_t defines the wikidbg stuff for \ref router_itor_t
 */
class router_itor_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	tableheader(const std::string &keyword, void *object_ptr)	throw();
	static std::string	tablerow(const std::string &keyword, void *object_ptr)		throw();
	
	static std::string	dnsgrab_list(const std::string &keyword, void *object_ptr)	throw();
	static std::string	cnx_list(const std::string &keyword, void *object_ptr)		throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	router_itor_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, router_itor_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", router_itor_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", router_itor_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, router_itor_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, router_itor_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, router_itor_wikidbg_t::tablerow);

	keyword_db.insert_html("dnsgrab_list"	, router_itor_wikidbg_t::dnsgrab_list);
	keyword_db.insert_html("cnx_list"	, router_itor_wikidbg_t::cnx_list);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string router_itor_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	router_itor_t *		router_itor	= (router_itor_t *)object_ptr;
	std::ostringstream	oss;

	oss << router_itor->remote_dnsname();

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
std::string router_itor_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	router_itor_t *		router_itor	= (router_itor_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", router_itor), wikidbg_html("page_title_attr", router_itor));
	oss << wikidbg_html("oneword", router_itor);
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
std::string router_itor_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t router_itor_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	router_itor_t *		router_itor	= (router_itor_t *)object_ptr;
	router_peer_t *		router_peer	= router_itor->router_peer;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", router_peer);
	
	// put the title
	oss << h.pagetitle("router_itor_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "router_peer"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", router_peer)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote_peerid"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << router_itor->remote_peerid()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote_dnsname"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << router_itor->remote_dnsname()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_query"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", router_itor->kad_query)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	oss << wikidbg_html("dnsgrab_list", router_itor);

	oss << wikidbg_html("cnx_list", router_itor);

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
std::string router_itor_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "remote_peerid"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "remote_dnsname"		<< h.e_b() << h.e_th();
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
std::string router_itor_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	router_itor_t*		router_itor	= (router_itor_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", router_itor)		<< h.e_td();
	oss << h.s_td() << router_itor->remote_peerid()				<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "dnsgrab_list" keyword
 */
std::string router_itor_wikidbg_t::dnsgrab_list(const std::string &keyword, void *object_ptr) throw()
{
	router_itor_t *		router_itor	= (router_itor_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// display the table of all the current dnsgrab_request_t
	oss << h.s_sub1title() << "List of all current dnsgrab_request_t: "
					<< router_itor->dnsgrab_req_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole router_peer_t::dnsgrab_req_db
	std::list<dnsgrab_request_t> &		dnsgrab_req_db = router_itor->dnsgrab_req_db;
	std::list<dnsgrab_request_t>::iterator	iter;
	for( iter = dnsgrab_req_db.begin(); iter != dnsgrab_req_db.end(); iter++ ){
		dnsgrab_request_t *	dnsgrab_request	= &(*iter);
		// if it is the first element, add the tableheader
		if(iter == dnsgrab_req_db.begin())	oss << wikidbg_html("tableheader", dnsgrab_request);	
		// display this row
		oss << wikidbg_html("tablerow", dnsgrab_request);
	}
	oss << h.e_table();
	// return the built string
	return oss.str();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "cnx_list" keyword
 */
std::string router_itor_wikidbg_t::cnx_list(const std::string &keyword, void *object_ptr) throw()
{
	router_itor_t *		router_itor	= (router_itor_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// display the table of all the current router_itor_cnx_t
	oss << h.s_sub1title() << "List of all current router_itor_cnx_t: "
					<< router_itor->cnx_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole router_peer_t::cnx_db
	std::list<router_itor_cnx_t *> &		cnx_db = router_itor->cnx_db;
	std::list<router_itor_cnx_t *>::iterator	iter;
	for( iter = cnx_db.begin(); iter != cnx_db.end(); iter++ ){
		router_itor_cnx_t *	resp_cnx	= *iter;
		// if it is the first element, add the tableheader
		if( iter == cnx_db.begin() )	oss << wikidbg_html("tableheader", resp_cnx);	
		// display this row
		oss << wikidbg_html("tablerow", resp_cnx);
	}
	oss << h.e_table();
	// return the built string
	return oss.str();
}



NEOIP_NAMESPACE_END







