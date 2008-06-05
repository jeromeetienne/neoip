/*! \file
    \brief Declaration of the bt_oload0_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_oload0_wikidbg.hpp"
#include "neoip_bt_oload0.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_oload0_http_t defines the wikidbg stuff for \ref bt_oload0_t
 */
class bt_oload0_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	menu(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

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
void	bt_oload0_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("menu"		, bt_oload0_wikidbg_t::menu);	
	keyword_db.insert_html("oneword"	, bt_oload0_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_oload0_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_oload0_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_oload0_wikidbg_t::page);


	keyword_db.insert_html("swarm_oneword"		, bt_oload0_wikidbg_t::swarm_oneword);
	keyword_db.insert_html("swarm_page_title_attr"	, bt_oload0_wikidbg_t::swarm_page_title_attr);
	keyword_db.insert_html("swarm_menu_item"	, bt_oload0_wikidbg_t::swarm_menu_item);		
	keyword_db.insert_page("swarm_page"		, bt_oload0_wikidbg_t::swarm_page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu" keyword
 */
std::string bt_oload0_wikidbg_t::menu(const std::string &keyword, void *object_ptr) throw()
{
	bt_oload0_t *		bt_oload0	= (bt_oload0_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// build displayed html itself
	oss << h.s_link(wikidbg_url("page", bt_oload0), "title=\"Main bt_oload0_t page\"")
					<< h.b("bt_oload0_t:") << h.e_link();
	oss << " ";
	oss << wikidbg_html("swarm_menu_item", bt_oload0);

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
std::string bt_oload0_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_oload0_t *		bt_oload0	= (bt_oload0_t *)object_ptr;
	std::ostringstream	oss;

	oss << bt_oload0->get_bt_session()->local_peerid();

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
std::string bt_oload0_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_oload0_t *		bt_oload0	= (bt_oload0_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", bt_oload0), wikidbg_html("page_title_attr", bt_oload0));
	oss << wikidbg_html("oneword", bt_oload0);
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
std::string bt_oload0_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_oload0_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_oload0_t *		bt_oload0	= (bt_oload0_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", bt_oload0);

	// put the title
	oss << h.pagetitle("bt_oload0_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_oload0_herr_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_oload0->oload0_herr)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "udp_vresp"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_oload0->udp_vresp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_listener"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_oload0->kad_listener)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_peer"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_oload0->kad_peer)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_session"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_oload0->bt_session)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
		
	// return no error
	return httpd_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             bt_oload0_swarm_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "swarm_oneword" keyword
 */
std::string bt_oload0_wikidbg_t::swarm_oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_oload0_t *		bt_oload0	= (bt_oload0_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "(" << bt_oload0->oload0_swarm_db.size() << ")";

	// return the built string
	return oss.str();
}


/** \brief Handle the "swarm_page_title_attr" keyword
 */
std::string bt_oload0_wikidbg_t::swarm_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	bt_oload0_t *		bt_oload0	= (bt_oload0_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << bt_oload0->oload0_swarm_db.size() << " responder";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "swarm_menu_item" keyword
 */
std::string bt_oload0_wikidbg_t::swarm_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_oload0_t *		bt_oload0	= (bt_oload0_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("swarm_page", bt_oload0), wikidbg_html("swarm_page_title_attr", bt_oload0));
	oss << "swarm";
	oss << wikidbg_html("swarm_oneword", bt_oload0);
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "swarm_page" keyword
 */
httpd_err_t bt_oload0_wikidbg_t::swarm_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_oload0_t *		bt_oload0	= (bt_oload0_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", bt_oload0);

	// put the title
	oss << h.pagetitle("bt_oload0_swarm_t Page");
	
	// display the table of all the current bt_oload0_swarm_t
	oss << h.s_sub1title() << "List of all current bt_oload0_swarm_t: "
					<< bt_oload0->oload0_swarm_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_oload0_t::oload0_swarm_db
	std::list<bt_oload0_swarm_t *> &		oload0_swarm_db = bt_oload0->oload0_swarm_db;
	std::list<bt_oload0_swarm_t *>::iterator	iter;
	for( iter = oload0_swarm_db.begin(); iter != oload0_swarm_db.end(); iter++ ){
		bt_oload0_swarm_t *	oload0_swarm	= *iter;
		// if it is the first element, add the tableheader
		if( iter == oload0_swarm_db.begin() )	oss << wikidbg_html("tableheader", oload0_swarm);	
		// display this row
		oss << wikidbg_html("tablerow", oload0_swarm);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







