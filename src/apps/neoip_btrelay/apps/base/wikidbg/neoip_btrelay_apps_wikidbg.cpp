/*! \file
    \brief Declaration of the btrelay_apps_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_btrelay_apps_wikidbg.hpp"
#include "neoip_btrelay_apps.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref btrelay_apps_http_t defines the wikidbg stuff for \ref btrelay_apps_t
 */
class btrelay_apps_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	menu(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	itor_oneword(const std::string &keyword, void *object_ptr)	throw();
	static std::string	itor_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	itor_menu_item(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	itor_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	swarm_oneword(const std::string &keyword, void *object_ptr)	throw();
	static std::string	swarm_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	swarm_menu_item(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	swarm_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	btrelay_apps_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("menu"		, btrelay_apps_wikidbg_t::menu);	
	keyword_db.insert_html("oneword"	, btrelay_apps_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", btrelay_apps_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", btrelay_apps_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, btrelay_apps_wikidbg_t::page);

	keyword_db.insert_html("itor_oneword"		, btrelay_apps_wikidbg_t::itor_oneword);
	keyword_db.insert_html("itor_page_title_attr"	, btrelay_apps_wikidbg_t::itor_page_title_attr);
	keyword_db.insert_html("itor_menu_item"		, btrelay_apps_wikidbg_t::itor_menu_item);		
	keyword_db.insert_page("itor_page"		, btrelay_apps_wikidbg_t::itor_page);

	keyword_db.insert_html("swarm_oneword"		, btrelay_apps_wikidbg_t::swarm_oneword);
	keyword_db.insert_html("swarm_page_title_attr"	, btrelay_apps_wikidbg_t::swarm_page_title_attr);
	keyword_db.insert_html("swarm_menu_item"		, btrelay_apps_wikidbg_t::swarm_menu_item);		
	keyword_db.insert_page("swarm_page"		, btrelay_apps_wikidbg_t::swarm_page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu" keyword
 */
std::string btrelay_apps_wikidbg_t::menu(const std::string &keyword, void *object_ptr) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// build displayed html itself
	oss << h.s_link(wikidbg_url("page", btrelay_apps), "title=\"Main btrelay_apps_t page\"")
					<< h.b("btrelay_apps_t:") << h.e_link();
	oss << " ";
	oss << wikidbg_html("itor_menu_item", btrelay_apps);
	oss << " / ";
	oss << wikidbg_html("swarm_menu_item", btrelay_apps);

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
std::string btrelay_apps_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	oss;

	oss << btrelay_apps->bt_ezsession()->bt_session()->local_peerid();

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
std::string btrelay_apps_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", btrelay_apps), wikidbg_html("page_title_attr", btrelay_apps));
	oss << wikidbg_html("oneword", btrelay_apps);
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
std::string btrelay_apps_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t btrelay_apps_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", btrelay_apps);

	// put the title
	oss << h.pagetitle("btrelay_apps_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_ezsession"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", btrelay_apps->bt_ezsession())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_relay"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", btrelay_apps->bt_relay())
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
		
	// return no error
	return httpd_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             btrelay_itor_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "itor_oneword" keyword
 */
std::string btrelay_apps_wikidbg_t::itor_oneword(const std::string &keyword, void *object_ptr) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "(" << btrelay_apps->itor_db.size() << ")";

	// return the built string
	return oss.str();
}


/** \brief Handle the "itor_page_title_attr" keyword
 */
std::string btrelay_apps_wikidbg_t::itor_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << btrelay_apps->itor_db.size() << " itor";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "itor_menu_item" keyword
 */
std::string btrelay_apps_wikidbg_t::itor_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("itor_page", btrelay_apps), wikidbg_html("itor_page_title_attr", btrelay_apps));
	oss << "itor";
	oss << wikidbg_html("itor_oneword", btrelay_apps);
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "itor_page" keyword
 */
httpd_err_t btrelay_apps_wikidbg_t::itor_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", btrelay_apps);

	// put the title
	oss << h.pagetitle("btrelay_itor_t Page");
	
	// display the table of all the current btrelay_itor_t
	oss << h.s_sub1title() << "List of all current btrelay_itor_t: "
					<< btrelay_apps->itor_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole btrelay_apps_t::itor_db
	std::list<btrelay_itor_t *> &		itor_db = btrelay_apps->itor_db;
	std::list<btrelay_itor_t *>::iterator	iter;
	for( iter = itor_db.begin(); iter != itor_db.end(); iter++ ){
		btrelay_itor_t *	btrelay_itor	= *iter;
		// if it is the first element, add the tableheader
		if( iter == itor_db.begin() )	oss << wikidbg_html("tableheader", btrelay_itor);	
		// display this row
		oss << wikidbg_html("tablerow", btrelay_itor);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             btrelay_swarm_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "swarm_oneword" keyword
 */
std::string btrelay_apps_wikidbg_t::swarm_oneword(const std::string &keyword, void *object_ptr) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "(" << btrelay_apps->swarm_db.size() << ")";

	// return the built string
	return oss.str();
}


/** \brief Handle the "swarm_page_title_attr" keyword
 */
std::string btrelay_apps_wikidbg_t::swarm_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << btrelay_apps->swarm_db.size() << " swarm";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "swarm_menu_item" keyword
 */
std::string btrelay_apps_wikidbg_t::swarm_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("swarm_page", btrelay_apps), wikidbg_html("swarm_page_title_attr", btrelay_apps));
	oss << "swarm";
	oss << wikidbg_html("swarm_oneword", btrelay_apps);
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "swarm_page" keyword
 */
httpd_err_t btrelay_apps_wikidbg_t::swarm_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	btrelay_apps_t *		btrelay_apps	= (btrelay_apps_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", btrelay_apps);

	// put the title
	oss << h.pagetitle("btrelay_swarm_t Page");
	
	// display the table of all the current btrelay_swarm_t
	oss << h.s_sub1title() << "List of all current btrelay_swarm_t: "
					<< btrelay_apps->swarm_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole btrelay_apps_t::swarm_db
	std::list<btrelay_swarm_t *> &		swarm_db = btrelay_apps->swarm_db;
	std::list<btrelay_swarm_t *>::iterator	iter;
	for( iter = swarm_db.begin(); iter != swarm_db.end(); iter++ ){
		btrelay_swarm_t *	btrelay_swarm	= *iter;
		// if it is the first element, add the tableheader
		if( iter == swarm_db.begin() )	oss << wikidbg_html("tableheader", btrelay_swarm);	
		// display this row
		oss << wikidbg_html("tablerow", btrelay_swarm);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







