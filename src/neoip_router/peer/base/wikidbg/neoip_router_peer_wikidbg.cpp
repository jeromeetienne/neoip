/*! \file
    \brief Declaration of the router_peer_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_router_peer_wikidbg.hpp"
#include "neoip_router_peer.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref router_peer_http_t defines the wikidbg stuff for \ref router_peer_t
 */
class router_peer_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	menu(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	full_oneword(const std::string &keyword, void *object_ptr)throw();
	static std::string	full_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	full_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	full_menu_item(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	full_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	itor_oneword(const std::string &keyword, void *object_ptr)throw();
	static std::string	itor_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	itor_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	itor_menu_item(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	itor_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	negcache_oneword(const std::string &keyword, void *object_ptr)throw();
	static std::string	negcache_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	negcache_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	negcache_menu_item(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	negcache_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	router_peer_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("menu"		, router_peer_wikidbg_t::menu);
	keyword_db.insert_html("oneword"	, router_peer_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", router_peer_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", router_peer_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, router_peer_wikidbg_t::page);

	keyword_db.insert_html("full_oneword"		, router_peer_wikidbg_t::full_oneword);
	keyword_db.insert_html("full_oneword_pageurl"	, router_peer_wikidbg_t::full_oneword_pageurl);
	keyword_db.insert_html("full_page_title_attr"	, router_peer_wikidbg_t::full_page_title_attr);
	keyword_db.insert_html("full_menu_item"		, router_peer_wikidbg_t::full_menu_item);
	keyword_db.insert_page("full_page"		, router_peer_wikidbg_t::full_page);

	keyword_db.insert_html("itor_oneword"		, router_peer_wikidbg_t::itor_oneword);
	keyword_db.insert_html("itor_oneword_pageurl"	, router_peer_wikidbg_t::itor_oneword_pageurl);
	keyword_db.insert_html("itor_page_title_attr"	, router_peer_wikidbg_t::itor_page_title_attr);
	keyword_db.insert_html("itor_menu_item"		, router_peer_wikidbg_t::itor_menu_item);
	keyword_db.insert_page("itor_page"		, router_peer_wikidbg_t::itor_page);

	keyword_db.insert_html("negcache_oneword"	, router_peer_wikidbg_t::negcache_oneword);
	keyword_db.insert_html("negcache_oneword_pageurl",router_peer_wikidbg_t::negcache_oneword_pageurl);
	keyword_db.insert_html("negcache_page_title_attr",router_peer_wikidbg_t::negcache_page_title_attr);
	keyword_db.insert_html("negcache_menu_item"	, router_peer_wikidbg_t::negcache_menu_item);
	keyword_db.insert_page("negcache_page"		, router_peer_wikidbg_t::negcache_page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu" keyword
 */
std::string router_peer_wikidbg_t::menu(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// build displayed html itself
	oss << h.s_link(wikidbg_url("page", router_peer), "title=\"Main router_peer_t page\"");
	oss << h.b("router_peer_t:");
	oss << h.e_link();
	oss << " ";
	oss << wikidbg_html("full_menu_item", router_peer);
	oss << " / ";
	oss << wikidbg_html("itor_menu_item", router_peer);
	oss << " / ";
	oss << wikidbg_html("negcache_menu_item", router_peer);
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
std::string router_peer_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;

	oss << router_peer->lident().peerid();

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
std::string router_peer_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", router_peer), wikidbg_html("page_title_attr", router_peer));
	oss << wikidbg_html("oneword", router_peer);
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
std::string router_peer_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t router_peer_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", router_peer);
	
	// put the title
	oss << h.pagetitle("router_peer_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ip_netaddr_arr"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << router_peer->ip_netaddr_arr
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "catchall addr"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << router_peer->catchall_iaddr
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "catchall_netif"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", router_peer->catchall_netif)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "local identity"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &router_peer->lident())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "router_acache"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &router_peer->acache())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "router_acl"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &router_peer->acl())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "router_resp"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", router_peer->router_resp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ntudp peer"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", router_peer->ntudp_peer)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_publish"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", router_peer->kad_publish)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "dnsgrab"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", router_peer->dnsgrab)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current router_rident_t
	oss << h.s_sub1title() << "List of all router_rident: "
					<< router_peer->rident_arr().size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole router_rident_t
	const router_rident_arr_t &	rident_arr	= router_peer->rident_arr();
	for(size_t i = 0; i < rident_arr.size(); i++){
		const router_rident_t &	router_rident = rident_arr[i];
		// if it is the first element, add the tableheader
		if( i == 0 )	oss << wikidbg_html("tableheader", &router_rident);	
		// display this row
		oss << wikidbg_html("tablerow", &router_rident);
	}
	oss << h.e_table();

	// display the table of all the current router_rootca_t
	oss << h.s_sub1title() << "List of all router_rootca: "
					<< router_peer->rootca_arr().size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole router_rootca_t
	const router_rootca_arr_t &	rootca_arr	= router_peer->rootca_arr();
	for(size_t i = 0; i < rootca_arr.size(); i++){
		const router_rootca_t &	router_rootca = rootca_arr[i];
		// if it is the first element, add the tableheader
		if( i == 0 )	oss << wikidbg_html("tableheader", &router_rootca);	
		// display this row
		oss << wikidbg_html("tablerow", &router_rootca);
	}
	oss << h.e_table();

	
	// return no error
	return httpd_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             router_full_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "full_oneword" keyword
 */
std::string router_peer_wikidbg_t::full_oneword(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << router_peer->full_db.size();

	// return the built string
	return oss.str();
}


/** \brief Handle the "full_page_title_attr" keyword
 */
std::string router_peer_wikidbg_t::full_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << "Click for more information on all the router_full_t on this router_peer_t";
	oss << "\"";

	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "full_oneword_pageurl" keyword
 */
std::string router_peer_wikidbg_t::full_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("full_page", router_peer), wikidbg_html("full_page_title_attr", router_peer));
	oss << wikidbg_html("full_oneword", router_peer);
	oss << h.e_link();
	
	// return the built string
	return oss.str();
}

/** \brief Handle the "full_menu_item" keyword
 */
std::string router_peer_wikidbg_t::full_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("full_page", router_peer), wikidbg_html("full_page_title_attr", router_peer));
	oss << "full";
	oss << "(" << wikidbg_html("full_oneword", router_peer) << ")";
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "full_page" keyword
 */
httpd_err_t router_peer_wikidbg_t::full_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", router_peer);

	// put the title
	oss << h.pagetitle("router_full_t Page");
	
	// display the table of all the current router_full_t
	oss << h.s_sub1title() << "List of all current router_full_t: "
					<< router_peer->full_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole router_peer_t::full_db
	std::list<router_full_t *> &		full_db = router_peer->full_db;
	std::list<router_full_t *>::iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		router_full_t *	router_full	= *iter;
		// if it is the first element, add the tableheader
		if( iter == full_db.begin() )	oss << wikidbg_html("tableheader", router_full);	
		// display this row
		oss << wikidbg_html("tablerow", router_full);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             router_itor_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "itor_oneword" keyword
 */
std::string router_peer_wikidbg_t::itor_oneword(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << router_peer->itor_db.size();

	// return the built string
	return oss.str();
}


/** \brief Handle the "itor_page_title_attr" keyword
 */
std::string router_peer_wikidbg_t::itor_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << "Click for more information on all the router_itor_t on this router_peer_t";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "itor_oneword_pageurl" keyword
 */
std::string router_peer_wikidbg_t::itor_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("itor_page", router_peer), wikidbg_html("itor_page_title_attr", router_peer));
	oss << wikidbg_html("itor_oneword", router_peer);
	oss << h.e_link();
	
	// return the built string
	return oss.str();
}

/** \brief Handle the "itor_menu_item" keyword
 */
std::string router_peer_wikidbg_t::itor_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("itor_page", router_peer), wikidbg_html("itor_page_title_attr", router_peer));
	oss << "itor";
	oss << "(" << wikidbg_html("itor_oneword", router_peer) << ")";
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "itor_page" keyword
 */
httpd_err_t router_peer_wikidbg_t::itor_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", router_peer);

	// put the title
	oss << h.pagetitle("router_itor_t Page");
	
	// display the table of all the current router_itor_t
	oss << h.s_sub1title() << "List of all current router_itor_t: "
					<< router_peer->itor_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole router_peer_t::itor_db
	std::list<router_itor_t *> &		itor_db = router_peer->itor_db;
	std::list<router_itor_t *>::iterator	iter;
	for( iter = itor_db.begin(); iter != itor_db.end(); iter++ ){
		router_itor_t *	router_itor	= *iter;
		// if it is the first element, add the tableheader
		if( iter == itor_db.begin() )	oss << wikidbg_html("tableheader", router_itor);	
		// display this row
		oss << wikidbg_html("tablerow", router_itor);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             itor_negcache stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "negcache_oneword" keyword
 */
std::string router_peer_wikidbg_t::negcache_oneword(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << router_peer->itor_negcache.size();

	// return the built string
	return oss.str();
}


/** \brief Handle the "itor_page_title_attr" keyword
 */
std::string router_peer_wikidbg_t::negcache_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << "Click for more information on all the peerid in negcache";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "itor_oneword_pageurl" keyword
 */
std::string router_peer_wikidbg_t::negcache_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("negcache_page", router_peer), wikidbg_html("negcache_page_title_attr", router_peer));
	oss << wikidbg_html("negcache_oneword", router_peer);
	oss << h.e_link();
	
	// return the built string
	return oss.str();
}

/** \brief Handle the "negcache_menu_item" keyword
 */
std::string router_peer_wikidbg_t::negcache_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("negcache_page", router_peer), wikidbg_html("negcache_page_title_attr", router_peer));
	oss << "itor negcache";
	oss << "(" << wikidbg_html("negcache_oneword", router_peer) << ")";
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "negcache_page" keyword
 */
httpd_err_t router_peer_wikidbg_t::negcache_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	router_peer_t *		router_peer	= (router_peer_t *)object_ptr;
	const router_rident_arr_t & rident_arr	= router_peer->rident_arr();
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", router_peer);

	// put the title
	oss << h.pagetitle("itor_negcache Page");
	
	// display the table of all the current router_itor_t
	oss << h.s_sub1title() << "List of all current peerid in the negcache: "
					<< router_peer->itor_negcache.size() << h.e_sub1title();
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "remote_peerid"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "rident"			<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "expire_delay"		<< h.e_b() << h.e_th();
	oss << h.e_tr();
	// go thru the whole router_peer_t::negcache_negcache_db
	const router_peer_t::itor_negcache_t &	itor_negcache = router_peer->itor_negcache;
	for(size_t i = 0; i < itor_negcache.size(); i++){
		const router_peerid_t &	remote_peerid	= itor_negcache[i];
		oss << h.s_tr();
		oss << h.s_td() << remote_peerid				<< h.e_td();
		if( rident_arr.find_by_peerid(remote_peerid) ){
			const router_rident_t &	router_rident	= rident_arr.get_by_peerid(remote_peerid);
			oss << h.s_td() << wikidbg_html("oneword_pageurl", &router_rident)<< h.e_td();
		}else{
			oss << h.s_td() << "unknown"				<< h.e_td();
		}
		oss << h.s_td() << itor_negcache.expire_delay_at(i)		<< h.e_td();
		oss << h.e_tr();
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







