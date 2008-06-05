/*! \file
    \brief Declaration of the bt_swarm_peersrc_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_peersrc_wikidbg.hpp"
#include "neoip_bt_swarm_peersrc.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_swarm_peersrc_t
 */
class bt_swarm_peersrc_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static std::string	menu_item(const std::string &keyword, void *object_ptr)		throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	itor_oneword(const std::string &keyword, void *object_ptr)	throw();
	static std::string	itor_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	itor_menu_item(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	itor_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_swarm_peersrc_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_swarm_peersrc_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_swarm_peersrc_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_swarm_peersrc_wikidbg_t::page_title_attr);
	keyword_db.insert_html("menu_item"	, bt_swarm_peersrc_wikidbg_t::menu_item);
	keyword_db.insert_page("page"		, bt_swarm_peersrc_wikidbg_t::page);

	keyword_db.insert_html("itor_oneword"		, bt_swarm_peersrc_wikidbg_t::itor_oneword);
	keyword_db.insert_html("itor_page_title_attr"	, bt_swarm_peersrc_wikidbg_t::itor_page_title_attr);
	keyword_db.insert_html("itor_menu_item"		, bt_swarm_peersrc_wikidbg_t::itor_menu_item);		
	keyword_db.insert_page("itor_page"		, bt_swarm_peersrc_wikidbg_t::itor_page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_swarm_peersrc_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "peersrc";

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
std::string bt_swarm_peersrc_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_peersrc_t *	swarm_peersrc	= (bt_swarm_peersrc_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", swarm_peersrc), wikidbg_html("page_title_attr", swarm_peersrc));
	oss << wikidbg_html("oneword", swarm_peersrc);
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
std::string bt_swarm_peersrc_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "tracker";
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
std::string bt_swarm_peersrc_wikidbg_t::menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_peersrc_t *	swarm_peersrc	= (bt_swarm_peersrc_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("page", swarm_peersrc), wikidbg_html("page_title_attr", swarm_peersrc));
	oss << wikidbg_html("oneword", swarm_peersrc);
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
httpd_err_t bt_swarm_peersrc_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_peersrc_t *	swarm_peersrc	= (bt_swarm_peersrc_t *)object_ptr;
	bt_swarm_t *		bt_swarm	= swarm_peersrc->bt_swarm;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", swarm_peersrc->bt_swarm);

	// put the title
	oss << h.pagetitle("bt_swarm_peersrc_t Page");
	
	/*************** display global info about bt_swarm_peersrc_t	*******/
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_swarm)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_seeder"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_peersrc->nb_seeder()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_leecher"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_peersrc->nb_leecher()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();
	
	/*************** display all the bt_peersrc_vapi_t	***************/
	oss << h.pagetitle("bt_peersrc_vapi_t");
	oss << h.s_table_packed_noborder();
	// go thru the whole bt_swarm_peersrc_t::peersrc_db
	std::list<bt_peersrc_vapi_t *> &		peersrc_db = swarm_peersrc->peersrc_db;
	std::list<bt_peersrc_vapi_t *>::iterator	iter;
	size_t						src_idx;
	for(iter = peersrc_db.begin(), src_idx = 0; iter != peersrc_db.end(); iter++, src_idx++){
		bt_peersrc_vapi_t *	peersrc_vapi	= *iter;
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "peersrc " << src_idx
					<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", peersrc_vapi)
					<< h.e_td();
			oss << h.e_tr();
	}	
	oss << h.e_table();
	oss << h.br();

	/*************** display all the bt_peersrc_peer_t	***************/
	oss << h.pagetitle("bt_peersrc_peer_t");
	oss << h.s_table_packed_noborder();
	// go thru the whole bt_swarm_peersrc_t::peersrc_db
	std::list<bt_peersrc_peer_t> &		peersrc_peer_db = swarm_peersrc->peersrc_peer_db;
	std::list<bt_peersrc_peer_t>::iterator	peer_iter;
	for(peer_iter = peersrc_peer_db.begin(), src_idx = 0; peer_iter != peersrc_peer_db.end(); peer_iter++, src_idx++){
		bt_peersrc_peer_t &	peersrc_peer	= *peer_iter;
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "peersrc_peer " << src_idx
					<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << peersrc_peer	<< h.e_td();
			oss << h.e_tr();
	}	
	oss << h.e_table();
	oss << h.br();
	
	/*************** display itor_blacklist	*******************************/
	oss << h.pagetitle("itor_blacklist");
	oss << h.s_table_packed_noborder();
	// go thru the whole bt_swarm_peersrc_t::peersrc_db
	bt_swarm_peersrc_t::itor_blacklist_t &	itor_blacklist	= *swarm_peersrc->itor_blacklist;
	for(size_t i = 0; i < itor_blacklist.size(); i++){
		const bt_peersrc_peer_t&peersrc_peer	= itor_blacklist[i];
		delay_t			expire_delay	= itor_blacklist.expire_delay_at(i);
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "black_listed " << i
					<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << peersrc_peer	
					<< " expiring in "<< expire_delay
					<< h.e_td();
			oss << h.e_tr();
	}
	oss << h.e_table();
	oss << h.br();
	
	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             bt_swarm_itor_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "itor_oneword" keyword
 */
std::string bt_swarm_peersrc_wikidbg_t::itor_oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_peersrc_t *	swarm_peersrc	= (bt_swarm_peersrc_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "(" << swarm_peersrc->itor_db.size() << ")";

	// return the built string
	return oss.str();
}


/** \brief Handle the "itor_page_title_attr" keyword
 */
std::string bt_swarm_peersrc_wikidbg_t::itor_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_peersrc_t *	swarm_peersrc	= (bt_swarm_peersrc_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << swarm_peersrc->itor_db.size() << " initiators";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "itor_menu_item" keyword
 */
std::string bt_swarm_peersrc_wikidbg_t::itor_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_peersrc_t *	swarm_peersrc	= (bt_swarm_peersrc_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("itor_page", swarm_peersrc), wikidbg_html("itor_page_title_attr", swarm_peersrc));
	oss << "itor";
	oss << wikidbg_html("itor_oneword", swarm_peersrc);
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "itor_page" keyword
 */
httpd_err_t bt_swarm_peersrc_wikidbg_t::itor_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_peersrc_t *	swarm_peersrc	= (bt_swarm_peersrc_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", swarm_peersrc->bt_swarm);

	// put the title
	oss << h.pagetitle("bt_swarm_itor_t Page");
	
	// display the table of all the current bt_swarm_itor_t
	oss << h.s_sub1title() << "List of all current bt_swarm_itor_t: "
					<< swarm_peersrc->itor_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_t::itor_db
	std::list<bt_swarm_itor_t *> &		itor_db = swarm_peersrc->itor_db;
	std::list<bt_swarm_itor_t *>::iterator	iter;
	for( iter = itor_db.begin(); iter != itor_db.end(); iter++ ){
		bt_swarm_itor_t *	swarm_itor	= *iter;
		// if it is the first element, add the tableheader
		if( iter == itor_db.begin() )	oss << wikidbg_html("tableheader", swarm_itor);	
		// display this row
		oss << wikidbg_html("tablerow", swarm_itor);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







