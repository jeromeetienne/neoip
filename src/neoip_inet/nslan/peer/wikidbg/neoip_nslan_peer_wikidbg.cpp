/*! \file
    \brief Declaration of the nslan_peer_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_nslan_peer_wikidbg.hpp"
#include "neoip_nslan_peer.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref nslan_peer_wikidbg_t defines the wikidbg stuff for \ref nslan_peer_t
 */
class nslan_peer_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	menu(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	tableheader(const std::string &keyword, void *object_ptr)	throw();
	static std::string	tablerow(const std::string &keyword, void *object_ptr)		throw();

	static std::string	publish_oneword(const std::string &keyword, void *object_ptr)throw();
	static std::string	publish_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	publish_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	publish_menu_item(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	publish_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	query_oneword(const std::string &keyword, void *object_ptr)throw();
	static std::string	query_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	query_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	query_menu_item(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	query_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	nslan_peer_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("menu"			, nslan_peer_wikidbg_t::menu);
	keyword_db.insert_html("oneword"		, nslan_peer_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl"	, nslan_peer_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, nslan_peer_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"			, nslan_peer_wikidbg_t::page);

	keyword_db.insert_html("tableheader"		, nslan_peer_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"		, nslan_peer_wikidbg_t::tablerow);

	keyword_db.insert_html("publish_oneword"	, nslan_peer_wikidbg_t::publish_oneword);
	keyword_db.insert_html("publish_oneword_pageurl", nslan_peer_wikidbg_t::publish_oneword_pageurl);
	keyword_db.insert_html("publish_page_title_attr", nslan_peer_wikidbg_t::publish_page_title_attr);
	keyword_db.insert_html("publish_menu_item"	, nslan_peer_wikidbg_t::publish_menu_item);
	keyword_db.insert_page("publish_page"		, nslan_peer_wikidbg_t::publish_page);

	keyword_db.insert_html("query_oneword"		, nslan_peer_wikidbg_t::query_oneword);
	keyword_db.insert_html("query_oneword_pageurl"	, nslan_peer_wikidbg_t::query_oneword_pageurl);
	keyword_db.insert_html("query_page_title_attr"	, nslan_peer_wikidbg_t::query_page_title_attr);
	keyword_db.insert_html("query_menu_item"	, nslan_peer_wikidbg_t::query_menu_item);
	keyword_db.insert_page("query_page"		, nslan_peer_wikidbg_t::query_page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu" keyword
 */
std::string nslan_peer_wikidbg_t::menu(const std::string &keyword, void *object_ptr) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// build displayed html itself
	oss << h.s_link(wikidbg_url("page", nslan_peer), "title=\"Main nslan_peer_t page\"");
	oss << h.b("nslan_peer_t:");
	oss << h.e_link();
	oss << " ";
	oss << wikidbg_html("publish_menu_item", nslan_peer);
	oss << " / ";
	oss << wikidbg_html("query_menu_item", nslan_peer);

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
std::string nslan_peer_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	oss;

	oss << nslan_peer->get_realmid();

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
std::string nslan_peer_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", nslan_peer), wikidbg_html("page_title_attr", nslan_peer));
	oss << wikidbg_html("oneword", nslan_peer);
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
std::string nslan_peer_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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

/** \brief Handle the "tableheader" keyword
 */
std::string nslan_peer_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "RealmID"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "Publish"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "Query"		<< h.e_b() << h.e_th();
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
std::string nslan_peer_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	nslan_peer_t*		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << h.s_link(wikidbg_url("page", nslan_peer), wikidbg_html("page_title_attr", nslan_peer))
				<< nslan_peer->get_realmid()
				<< h.e_link();
	oss << h.s_td() << wikidbg_html("publish_oneword_pageurl", nslan_peer)	<< h.e_td();
	oss << h.s_td() << wikidbg_html("query_oneword_pageurl", nslan_peer)	<< h.e_td();
	oss << h.e_tr();

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
httpd_err_t nslan_peer_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", nslan_peer);

	// put the title
	oss << h.pagetitle("nslan_peer_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "RealmID"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nslan_peer->get_realmid()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nslan Listener"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", nslan_peer->nslan_listener)
						<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             nslan_publish_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "publish_oneword" keyword
 */
std::string nslan_peer_wikidbg_t::publish_oneword(const std::string &keyword, void *object_ptr) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << nslan_peer->publish_db.size() << " record";

	// return the built string
	return oss.str();
}


/** \brief Handle the "publish_page_title_attr" keyword
 */
std::string nslan_peer_wikidbg_t::publish_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << "Click for more information on all the record publish on this nslan_peer_t";
	oss << "\"";

	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "publish_oneword_pageurl" keyword
 */
std::string nslan_peer_wikidbg_t::publish_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("publish_page", nslan_peer), wikidbg_html("publish_page_title_attr", nslan_peer));
	oss << wikidbg_html("publish_oneword", nslan_peer);
	oss << h.e_link();
	
	// return the built string
	return oss.str();
}

/** \brief Handle the "publish_menu_item" keyword
 */
std::string nslan_peer_wikidbg_t::publish_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("publish_page", nslan_peer), wikidbg_html("publish_page_title_attr", nslan_peer));
	oss << "publish";
	oss << "(" << wikidbg_html("publish_oneword", nslan_peer) << ")";
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "publish_page" keyword
 */
httpd_err_t nslan_peer_wikidbg_t::publish_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", nslan_peer);

	// put the title
	oss << h.pagetitle("nslan_publish_t Page");
	
	// display the table of all the current nslan_publish_t
	oss << h.s_sub1title() << "List of all current nslan_publish_t: "
					<< nslan_peer->publish_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole nslan_peer_t::publish_db
	std::multimap<nslan_keyid_t, nslan_publish_t *> &		publish_db = nslan_peer->publish_db;
	std::multimap<nslan_keyid_t, nslan_publish_t *>::iterator	iter;
	for( iter = publish_db.begin(); iter != publish_db.end(); iter++ ){
		nslan_publish_t *	nslan_publish	= iter->second;
		// if it is the first element, add the tableheader
		if( iter == publish_db.begin() )	oss << wikidbg_html("tableheader", nslan_publish);	
		// display this row
		oss << wikidbg_html("tablerow", nslan_publish);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             nslan_query_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "query_oneword" keyword
 */
std::string nslan_peer_wikidbg_t::query_oneword(const std::string &keyword, void *object_ptr) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << nslan_peer->query_db.size() << " record";

	// return the built string
	return oss.str();
}


/** \brief Handle the "query_page_title_attr" keyword
 */
std::string nslan_peer_wikidbg_t::query_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << "Click for more information on all the pending query on this nslan_peer_t";
	oss << "\"";

	// return the built string
	return oss.str();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "query_oneword_pageurl" keyword
 */
std::string nslan_peer_wikidbg_t::query_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("query_page", nslan_peer), wikidbg_html("query_page_title_attr", nslan_peer));
	oss << wikidbg_html("query_oneword", nslan_peer);
	oss << h.e_link();
	
	// return the built string
	return oss.str();
}
/** \brief Handle the "query_menu_item" keyword
 */
std::string nslan_peer_wikidbg_t::query_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("query_page", nslan_peer), wikidbg_html("query_page_title_attr", nslan_peer));
	oss << "query";
	oss << "(" << wikidbg_html("query_oneword", nslan_peer) << ")";
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "query_page" keyword
 */
httpd_err_t nslan_peer_wikidbg_t::query_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	nslan_peer_t *		nslan_peer	= (nslan_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", nslan_peer);

	// put the title
	oss << h.pagetitle("nslan_query_t Page");
	
	// display the table of all the current nslan_query_t
	oss << h.s_sub1title() << "List of all current nslan_query_t: "
					<< nslan_peer->query_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole nslan_peer_t::query_db
	std::multimap<nslan_keyid_t, nslan_query_t *> &		query_db = nslan_peer->query_db;
	std::multimap<nslan_keyid_t, nslan_query_t *>::iterator	iter;
	for( iter = query_db.begin(); iter != query_db.end(); iter++ ){
		nslan_query_t *	nslan_query	= iter->second;
		// if it is the first element, add the tableheader
		if( iter == query_db.begin() )	oss << wikidbg_html("tableheader", nslan_query);	
		// display this row
		oss << wikidbg_html("tablerow", nslan_query);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







