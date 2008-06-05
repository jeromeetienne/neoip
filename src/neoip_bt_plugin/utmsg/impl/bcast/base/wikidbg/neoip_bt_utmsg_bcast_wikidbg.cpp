/*! \file
    \brief Declaration of the bt_utmsg_bcast_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_utmsg_bcast_wikidbg.hpp"
#include "neoip_bt_utmsg_bcast.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_utmsg_bcast_t
 */
class bt_utmsg_bcast_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_utmsg_bcast_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_utmsg_bcast_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_utmsg_bcast_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_utmsg_bcast_wikidbg_t::page_title_attr);
	keyword_db.insert_html("menu_item"	, bt_utmsg_bcast_wikidbg_t::menu_item);
	keyword_db.insert_page("page"		, bt_utmsg_bcast_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_utmsg_bcast_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bcast extension";

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
std::string bt_utmsg_bcast_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_utmsg_bcast_t *	utmsg_bcast	= (bt_utmsg_bcast_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", utmsg_bcast), wikidbg_html("page_title_attr", utmsg_bcast));
	oss << wikidbg_html("oneword", utmsg_bcast);
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
std::string bt_utmsg_bcast_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "bcast tracker";
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
std::string bt_utmsg_bcast_wikidbg_t::menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_utmsg_bcast_t *	utmsg_bcast	= (bt_utmsg_bcast_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("page", utmsg_bcast), wikidbg_html("page_title_attr", utmsg_bcast));
	oss << wikidbg_html("oneword", utmsg_bcast);
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
httpd_err_t bt_utmsg_bcast_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_utmsg_bcast_t *	utmsg_bcast	= (bt_utmsg_bcast_t *)object_ptr;	
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", utmsg_bcast->bt_swarm);
	// put the title
	oss << h.pagetitle("bt_utmsg_bcast_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "utmsg_type"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << utmsg_bcast->utmsgtype()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "utmsg_str"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << utmsg_bcast->utmsgstr()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// display the table of all the current bt_swarm_t
	oss << h.s_sub1title() << "List of all current bt_utmsg_bcast_cnx_t: "
					<< utmsg_bcast->cnx_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_session_t::swarm_db
	std::list<bt_utmsg_bcast_cnx_t *> &		cnx_db = utmsg_bcast->cnx_db;
	std::list<bt_utmsg_bcast_cnx_t *>::iterator	iter;
	for(iter = cnx_db.begin(); iter != cnx_db.end(); iter++ ){
		bt_utmsg_bcast_cnx_t *	bcast_cnx	= *iter;
		// if it is the first element, add the tableheader
		if( iter == cnx_db.begin() )	oss << wikidbg_html("tableheader", bcast_cnx);	
		// display this row
		oss << wikidbg_html("tablerow", bcast_cnx);
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







