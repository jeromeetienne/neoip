/*! \file
    \brief Declaration of the bt_relay_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_relay_wikidbg.hpp"
#include "neoip_bt_relay.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_relay_http_t defines the wikidbg stuff for \ref bt_relay_t
 */
class bt_relay_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_relay_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_relay_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_relay_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_relay_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_relay_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_relay_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bt_relay";

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
std::string bt_relay_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_relay_t *		bt_relay	= (bt_relay_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", bt_relay), wikidbg_html("page_title_attr", bt_relay));
	oss << wikidbg_html("oneword", bt_relay);
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
std::string bt_relay_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_relay_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_relay_t *		bt_relay	= (bt_relay_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_relay_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "eval_timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &bt_relay->eval_timeout)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current bt_swarm_t
	oss << h.s_sub1title() << "List of all current bt_swarm_t: "
					<< bt_relay->swarm_db().size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_session_t::swarm_db
	const bt_relay_t::swarm_db_t &		swarm_db = bt_relay->swarm_db();
	bt_relay_t::swarm_db_t::const_iterator	iter;
	for( iter = swarm_db.begin(); iter != swarm_db.end(); iter++ ){
		const bt_swarm_t *	bt_swarm	= *iter;
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







