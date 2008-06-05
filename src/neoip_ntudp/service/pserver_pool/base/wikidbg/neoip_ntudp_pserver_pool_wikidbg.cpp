/*! \file
    \brief Declaration of the ntudp_pserver_pool_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_pserver_pool_wikidbg.hpp"
#include "neoip_ntudp_pserver_pool.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief \ref ntudp_pserver_pool_wikidbg_t defines the wikidbg stuff for \ref ntudp_pserver_pool_t
 */
class ntudp_pserver_pool_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_menu_item(const std::string &keyword, void *object_ptr)		throw();
	static std::string	wikidbg_oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	wikidbg_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	wikidbg_reach_db(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_unreach_db(const std::string &keyword, void *object_ptr)throw();
	static std::string	wikidbg_unknown_db(const std::string &keyword, void *object_ptr)throw();
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	ntudp_pserver_pool_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("menu_item"	, ntudp_pserver_pool_wikidbg_t::wikidbg_menu_item);
	keyword_db.insert_html("oneword"	, ntudp_pserver_pool_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("oneword_pageurl", ntudp_pserver_pool_wikidbg_t::wikidbg_oneword_pageurl);
	keyword_db.insert_html("page_title_attr", ntudp_pserver_pool_wikidbg_t::wikidbg_page_title_attr);
	keyword_db.insert_page("page"		, ntudp_pserver_pool_wikidbg_t::wikidbg_page);

	keyword_db.insert_html("reach_db"	, ntudp_pserver_pool_wikidbg_t::wikidbg_reach_db);
	keyword_db.insert_html("unreach_db"	, ntudp_pserver_pool_wikidbg_t::wikidbg_unreach_db);
	keyword_db.insert_html("unknown_db"	, ntudp_pserver_pool_wikidbg_t::wikidbg_unknown_db);
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu_item" keyword
 */
std::string ntudp_pserver_pool_wikidbg_t::wikidbg_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_pserver_pool_t *	pserver_pool	= (ntudp_pserver_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", pserver_pool), wikidbg_html("page_title_attr", pserver_pool));
	oss << wikidbg_html("oneword", pserver_pool);
	oss << h.e_link();
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
std::string ntudp_pserver_pool_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_pserver_pool_t *	pserver_pool	= (ntudp_pserver_pool_t *)object_ptr;
	std::ostringstream	oss;

	oss << "pserver_pool(";
	oss << pserver_pool->reach_size()	<< "/";
	oss << pserver_pool->unreach_size()	<< "/";
	oss << pserver_pool->unknown_size();
	oss << ")";

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
std::string ntudp_pserver_pool_wikidbg_t::wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_pserver_pool_t *	pserver_pool	= (ntudp_pserver_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", pserver_pool), wikidbg_html("page_title_attr", pserver_pool));
	oss << wikidbg_html("oneword", pserver_pool);
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
std::string ntudp_pserver_pool_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_pserver_pool_t *	pserver_pool	= (ntudp_pserver_pool_t *)object_ptr;
	std::ostringstream	oss;

	// build the title attribute
	oss << "title=\"";
	oss << "Addresses State: ";
	oss << pserver_pool->reach_size()	<< " reachable";
	oss << " / ";
	oss << pserver_pool->unreach_size()	<< " unreachable";
	oss << " / ";
	oss << pserver_pool->unknown_size()	<< " unknown";
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
httpd_err_t ntudp_pserver_pool_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request)	throw()
{
	ntudp_pserver_pool_t *	pserver_pool	= (ntudp_pserver_pool_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the ntudp_peer_t menu
	oss << wikidbg_html("menu", pserver_pool->ntudp_peer);

	// put the title
	oss << h.pagetitle("ntudp_pserver_pool_t Page");
	
	oss << "This object handles a pool of the public servers addresses. ";
	oss << "It allows to query new addresses and maintain the state of the current ones.";

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_query"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", pserver_pool->kad_query)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "timeout_noquery"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &pserver_pool->timeout_noquery)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	oss << h.hr();
	oss << h.s_table("width=\"100\%\"");
	oss << h.s_tr();
	oss << h.s_td("valign=\"top\" width=\"33%\"") 
			<< wikidbg_html("reach_db"	, pserver_pool) << h.e_td();
	oss << h.s_td("valign=\"top\" width=\"33%\"")
			<< wikidbg_html("unreach_db"	, pserver_pool) << h.e_td();
	oss << h.s_td("valign=\"top\" width=\"33%\"")
			<< wikidbg_html("unknown_db"	, pserver_pool) << h.e_td();
	oss << h.e_tr();
	oss << h.e_table();


	// return no error
	return httpd_err_t::OK;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "reach_db" keyword
 */
std::string ntudp_pserver_pool_wikidbg_t::wikidbg_reach_db(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_pserver_pool_t *	pserver_pool	= (ntudp_pserver_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// display all the reachable addresses
	oss << h.s_table();
	oss << h.s_caption() << h.s_b() << "Reachable Address: "<< pserver_pool->reach_db.size() 
						<< h.e_b() << h.e_caption();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "IP:port" << h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "Expire Delay (default=" 
				<< pserver_pool->reach_db.get_default_ttl() << ")"
				<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	for(size_t i = 0; i < pserver_pool->reach_size(); i++ ){
		oss << h.s_tr();
		oss << h.s_td() << pserver_pool->reach_at(i)			<< h.e_td();
		oss << h.s_td() << pserver_pool->reach_expire_delay_at(i)	<< h.e_td();
		oss << h.e_tr();
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

/** \brief Handle the "unreach_db" keyword
 */
std::string ntudp_pserver_pool_wikidbg_t::wikidbg_unreach_db(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_pserver_pool_t *	pserver_pool	= (ntudp_pserver_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// display all the unreachable addresses
	oss << h.s_table();
	oss << h.s_caption() << h.s_b() << "Unreachable Address: "<< pserver_pool->unreach_db.size() 
						<< h.e_b() << h.e_caption();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "IP:port" << h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "Expire Delay (default=" 
				<< pserver_pool->unreach_db.get_default_ttl() << ")"
				<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	for(size_t i = 0; i < pserver_pool->unreach_size(); i++ ){
		oss << h.s_tr();
		oss << h.s_td() << pserver_pool->unreach_at(i)			<< h.e_td();
		oss << h.s_td() << pserver_pool->unreach_expire_delay_at(i)	<< h.e_td();
		oss << h.e_tr();
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

/** \brief Handle the "unknown_db" keyword
 */
std::string ntudp_pserver_pool_wikidbg_t::wikidbg_unknown_db(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_pserver_pool_t *	pserver_pool	= (ntudp_pserver_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// display all the unknownable addresses
	oss << h.s_table();
	oss << h.s_caption() << h.s_b() << "Unknown Address: "<< pserver_pool->unknown_db.size() 
						<< h.e_b() << h.e_caption();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "IP:port" << h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "Expire Delay (default=" 
				<< pserver_pool->unknown_db.get_default_ttl() << ")"
				<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	for(size_t i = 0; i < pserver_pool->unknown_size(); i++ ){
		oss << h.s_tr();
		oss << h.s_td() << pserver_pool->unknown_at(i)			<< h.e_td();
		oss << h.s_td() << pserver_pool->unknown_expire_delay_at(i)	<< h.e_td();
		oss << h.e_tr();
	}	
	oss << h.e_table();

	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END







