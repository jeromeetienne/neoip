/*! \file
    \brief Declaration of the bt_session_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_session_wikidbg.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_session_http_t defines the wikidbg stuff for \ref bt_session_t
 */
class bt_session_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_session_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_session_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_session_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_session_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_session_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_session_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_session_t *		bt_session	= (bt_session_t *)object_ptr;
	std::ostringstream	oss;

	oss << bt_session->local_peerid();

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
std::string bt_session_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_session_t *		bt_session	= (bt_session_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", bt_session), wikidbg_html("page_title_attr", bt_session));
	oss << wikidbg_html("oneword", bt_session);
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
std::string bt_session_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_session_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_session_t *		bt_session	= (bt_session_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_session_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "peerid"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_session->local_peerid()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "local program"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_session->local_peerid().peerid_progfull();
			if( bt_session->local_peerid().peerid_progfull().substr(0,7) == "unknown" )
				oss << h.s_pre() << bt_session->local_peerid().to_datum() << h.e_pre();		
			oss << h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "socket_resp"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_session->socket_resp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "listen_ipport_lview"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_session->listen_lview()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "listen_ipport_pview"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_session->listen_pview()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "is_tcp_inetreach"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_session->is_tcp_inetreach
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	// display the table of all the current bt_swarm_t
	oss << h.s_sub1title() << "List of all current bt_swarm_t: "
					<< bt_session->swarm_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_session_t::swarm_db
	std::list<bt_swarm_t *> &		swarm_db = bt_session->swarm_db;
	std::list<bt_swarm_t *>::iterator	iter;
	for( iter = swarm_db.begin(); iter != swarm_db.end(); iter++ ){
		bt_swarm_t *	bt_swarm	= *iter;
		// if it is the first element, add the tableheader
		if( iter == swarm_db.begin() )	oss << wikidbg_html("tableheader", bt_swarm);	
		// display this row
		oss << wikidbg_html("tablerow", bt_swarm);
	}
	oss << h.e_table();
		
	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







