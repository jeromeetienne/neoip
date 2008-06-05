/*! \file
    \brief Declaration of the bt_swarm_sched_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_sched_wikidbg.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_swarm_sched_t
 */
class bt_swarm_sched_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_swarm_sched_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_swarm_sched_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_swarm_sched_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_swarm_sched_wikidbg_t::page_title_attr);
	keyword_db.insert_html("menu_item"	, bt_swarm_sched_wikidbg_t::menu_item);
	keyword_db.insert_page("page"		, bt_swarm_sched_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_swarm_sched_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_sched_t *	swarm_sched	= (bt_swarm_sched_t *)object_ptr;
	std::ostringstream	oss;
	size_t			nb_piece	= swarm_sched->piece_do_needreq_db.size() 
						+ swarm_sched->piece_no_needreq_db.size();

	oss << "sched(" << nb_piece << ")";

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
std::string bt_swarm_sched_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_sched_t *	swarm_sched	= (bt_swarm_sched_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", swarm_sched), wikidbg_html("page_title_attr", swarm_sched));
	oss << wikidbg_html("oneword", swarm_sched);
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
std::string bt_swarm_sched_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << "sched";
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
std::string bt_swarm_sched_wikidbg_t::menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_sched_t *	swarm_sched	= (bt_swarm_sched_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("page", swarm_sched), wikidbg_html("page_title_attr", swarm_sched));
	oss << wikidbg_html("oneword", swarm_sched);
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
httpd_err_t bt_swarm_sched_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_sched_t *	swarm_sched	= (bt_swarm_sched_t *)object_ptr;
	bt_swarm_t *		bt_swarm	= swarm_sched->bt_swarm();
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", bt_swarm);
	
	// put the title
	oss << h.pagetitle("bt_swarm_sched_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_swarm)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "in_endgame"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << (swarm_sched->in_endgame() ? "YES" : "no")
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// display the table of all the current bt_swarm_sched_piece_t
	std::map<size_t, bt_swarm_sched_piece_t *> &		piece_do_needreq_db = swarm_sched->piece_do_needreq_db;
	std::map<size_t, bt_swarm_sched_piece_t *>::iterator	iter;
	oss << h.s_sub1title() << "List of all bt_swarm_sched_piece_t DO_NEEDREQ: "
				<< piece_do_needreq_db.size()	<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_sched_t::piece_db
	for( iter = piece_do_needreq_db.begin(); iter != piece_do_needreq_db.end(); iter++ ){
		bt_swarm_sched_piece_t *	sched_piece	= iter->second;
		// if it is the first element, add the tableheader
		if( iter == piece_do_needreq_db.begin() )	oss << wikidbg_html("tableheader", sched_piece);	
		// display this row
		oss << wikidbg_html("tablerow", sched_piece);
	}
	oss << h.e_table();
		
	// display the table of all the current bt_swarm_sched_piece_t
	std::map<size_t, bt_swarm_sched_piece_t *> &		piece_no_needreq_db = swarm_sched->piece_no_needreq_db;
	oss << h.s_sub1title() << "List of all bt_swarm_sched_piece_t NO_NEEDREQ: "
				<< piece_no_needreq_db.size()	<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_sched_t::piece_db
	for( iter = piece_no_needreq_db.begin(); iter != piece_no_needreq_db.end(); iter++ ){
		bt_swarm_sched_piece_t *	sched_piece	= iter->second;
		// if it is the first element, add the tableheader
		if( iter == piece_no_needreq_db.begin() )	oss << wikidbg_html("tableheader", sched_piece);	
		// display this row
		oss << wikidbg_html("tablerow", sched_piece);
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







