/*! \file
    \brief Declaration of the bt_swarm_full_sched_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_full_sched_wikidbg.hpp"
#include "neoip_bt_swarm_full_sched.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_swarm_full_sched_t
 */
class bt_swarm_full_sched_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr
							, httpd_request_t &request)	throw();
	static std::string	page_info(const std::string &keyword, void *object_ptr)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_swarm_full_sched_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_swarm_full_sched_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_swarm_full_sched_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_swarm_full_sched_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_swarm_full_sched_wikidbg_t::page);

	keyword_db.insert_html("page_info"	, bt_swarm_full_sched_wikidbg_t::page_info);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_swarm_full_sched_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_full_sched_t *	full_sched	= (bt_swarm_full_sched_t *)object_ptr;
	std::ostringstream	oss;

	oss << "full_sched ("<< full_sched->request_db.size() <<")";
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
std::string bt_swarm_full_sched_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_full_sched_t *	full_sched	= (bt_swarm_full_sched_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", full_sched), wikidbg_html("page_title_attr", full_sched));
	oss << wikidbg_html("oneword", full_sched);
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
std::string bt_swarm_full_sched_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "full_sched established connection";
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
httpd_err_t bt_swarm_full_sched_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_full_sched_t *	full_sched	= (bt_swarm_full_sched_t *)object_ptr;
	bt_swarm_t *		bt_swarm	= full_sched->swarm_full->get_swarm();
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", bt_swarm);
	// put the title
	oss << h.pagetitle("bt_swarm_full_sched_t Page");
	
	// put the page_info 
	oss << wikidbg_html("page_info", full_sched);

	// return no error
	return httpd_err_t::OK;
}

/** \brief Handle the "page_info" keyword
 */
std::string bt_swarm_full_sched_wikidbg_t::page_info(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_full_sched_t *	full_sched	= (bt_swarm_full_sched_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm_full_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", full_sched->swarm_full)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "reqauth_type"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << full_sched->reqauth_type()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "reqauth_timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &full_sched->reqauth_timeout)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();


	// display the table of all the current bt_swarm_sched_request_t
	bt_swarm_full_sched_t::request_db_t &		request_db = full_sched->request_db;
	bt_swarm_full_sched_t::request_db_t::iterator	iter;
	oss << h.s_sub1title() << "List of all pending bt_swarm_sched_request_t: "
				<< request_db.size()
				<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_sched_t::request_db
	for( iter = request_db.begin(); iter != request_db.end(); iter++ ){
		bt_swarm_sched_request_t *	sched_request	= *iter;
		// if it is the first element, add the tableheader
		if( iter == request_db.begin() )	oss << wikidbg_html("tableheader", sched_request);	
		// display this row
		oss << wikidbg_html("tablerow", sched_request);
	}
	oss << h.e_table();
	
	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







