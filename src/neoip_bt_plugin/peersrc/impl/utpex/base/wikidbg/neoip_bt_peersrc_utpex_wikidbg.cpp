/*! \file
    \brief Declaration of the bt_peersrc_utpex_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_peersrc_utpex_wikidbg.hpp"
#include "neoip_bt_peersrc_utpex.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_peersrc_utpex_t
 */
class bt_peersrc_utpex_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_peersrc_utpex_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_peersrc_utpex_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_peersrc_utpex_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_peersrc_utpex_wikidbg_t::page_title_attr);
	keyword_db.insert_html("menu_item"	, bt_peersrc_utpex_wikidbg_t::menu_item);
	keyword_db.insert_page("page"		, bt_peersrc_utpex_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_peersrc_utpex_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "utpex tracker";

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
std::string bt_peersrc_utpex_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_peersrc_utpex_t *	peersrc_utpex	= (bt_peersrc_utpex_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", peersrc_utpex), wikidbg_html("page_title_attr", peersrc_utpex));
	oss << wikidbg_html("oneword", peersrc_utpex);
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
std::string bt_peersrc_utpex_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "utpex tracker";
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
std::string bt_peersrc_utpex_wikidbg_t::menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_peersrc_utpex_t *	peersrc_utpex	= (bt_peersrc_utpex_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("page", peersrc_utpex), wikidbg_html("page_title_attr", peersrc_utpex));
	oss << wikidbg_html("oneword", peersrc_utpex);
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
httpd_err_t bt_peersrc_utpex_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_peersrc_utpex_t *	peersrc_utpex	= (bt_peersrc_utpex_t *)object_ptr;	
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", peersrc_utpex->bt_swarm);
	// put the title
	oss << h.pagetitle("bt_peersrc_utpex_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "utmsg_type"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << peersrc_utpex->utmsgtype()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "utmsg_str"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << peersrc_utpex->utmsgstr()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_seeder"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << peersrc_utpex->nb_seeder()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_leecher"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << peersrc_utpex->nb_leecher()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "next_seqnb"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << peersrc_utpex->next_seqnb
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// display the table of all the current bt_swarm_t
	oss << h.s_sub1title() << "List of all current bt_peersrc_utpex_cnx_t: "
					<< peersrc_utpex->cnx_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_session_t::swarm_db
	std::list<bt_peersrc_utpex_cnx_t *> &		cnx_db = peersrc_utpex->cnx_db;
	std::list<bt_peersrc_utpex_cnx_t *>::iterator	iter;
	for(iter = cnx_db.begin(); iter != cnx_db.end(); iter++ ){
		bt_peersrc_utpex_cnx_t *	utpex_cnx	= *iter;
		// if it is the first element, add the tableheader
		if( iter == cnx_db.begin() )	oss << wikidbg_html("tableheader", utpex_cnx);	
		// display this row
		oss << wikidbg_html("tablerow", utpex_cnx);
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







