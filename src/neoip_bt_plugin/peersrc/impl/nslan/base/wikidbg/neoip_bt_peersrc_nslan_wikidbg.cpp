/*! \file
    \brief Declaration of the bt_peersrc_nslan_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_peersrc_nslan_wikidbg.hpp"
#include "neoip_bt_peersrc_nslan.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_peersrc_nslan_t
 */
class bt_peersrc_nslan_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static std::string	menu_item(const std::string &keyword, void *object_ptr)		throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_peersrc_nslan_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_peersrc_nslan_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_peersrc_nslan_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_peersrc_nslan_wikidbg_t::page_title_attr);
	keyword_db.insert_html("menu_item"	, bt_peersrc_nslan_wikidbg_t::menu_item);
	keyword_db.insert_page("page"		, bt_peersrc_nslan_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_peersrc_nslan_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "nslan peersrc";

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
std::string bt_peersrc_nslan_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_peersrc_nslan_t*peersrc_nslan	= (bt_peersrc_nslan_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", peersrc_nslan), wikidbg_html("page_title_attr", peersrc_nslan));
	oss << wikidbg_html("oneword", peersrc_nslan);
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
std::string bt_peersrc_nslan_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "nslan peersrc";
	oss << "\"";
	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu_item" keyword
 */
std::string bt_peersrc_nslan_wikidbg_t::menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_peersrc_nslan_t*peersrc_nslan	= (bt_peersrc_nslan_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("page", peersrc_nslan), wikidbg_html("page_title_attr", peersrc_nslan));
	oss << wikidbg_html("oneword", peersrc_nslan);
	oss << h.e_link();

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
httpd_err_t bt_peersrc_nslan_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_peersrc_nslan_t*	peersrc_nslan	= (bt_peersrc_nslan_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", peersrc_nslan->bt_swarm);
	// put the title
	oss << h.pagetitle("bt_peersrc_nslan_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_seeder"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << peersrc_nslan->nb_seeder()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_leecher"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << peersrc_nslan->nb_leecher()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nslan_peer"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", peersrc_nslan->nslan_peer)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nslan_publish"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", peersrc_nslan->nslan_publish)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();
		
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







