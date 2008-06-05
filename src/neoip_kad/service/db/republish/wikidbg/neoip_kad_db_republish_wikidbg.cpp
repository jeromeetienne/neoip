/*! \file
    \brief Declaration of the kad_db_republish_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_kad_db_republish_wikidbg.hpp"
#include "neoip_kad_db_republish.hpp"
#include "neoip_kad_db.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref kad_db_republish_http_t defines the wikidbg stuff for \ref kad_db_republish_t
 */
class kad_db_republish_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	kad_db_republish_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, kad_db_republish_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", kad_db_republish_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", kad_db_republish_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, kad_db_republish_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string kad_db_republish_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	kad_db_republish_t *	kad_db_republish	= (kad_db_republish_t *)object_ptr;
	std::ostringstream	oss;

	oss << kad_db_republish->kad_store_db.size() << " kad_store_t inprogress";

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
std::string kad_db_republish_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	kad_db_republish_t *		kad_db_republish	= (kad_db_republish_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", kad_db_republish), wikidbg_html("page_title_attr", kad_db_republish));
	oss << wikidbg_html("oneword", kad_db_republish);
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
std::string kad_db_republish_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "Click for more info.";
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
httpd_err_t kad_db_republish_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	kad_db_republish_t *	kad_db_republish= (kad_db_republish_t *)object_ptr;
	kad_db_t *		kad_db		= kad_db_republish->kad_db;
	std::ostringstream &	oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", kad_db->kad_peer);

	// put the title
	if( kad_db->is_local_db() )	oss << h.pagetitle("Local kad_db_republish_t Page");
	else				oss << h.pagetitle("Remote kad_db_republish_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_db"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", kad_db);
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	// display the table of all the current nslan_publish_t
	oss << h.s_sub1title() << "List of all current kad_store_t: "
					<< kad_db_republish->kad_store_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole database
	std::list<kad_store_t *> &		kad_store_db = kad_db_republish->kad_store_db;
	std::list<kad_store_t *>::iterator	iter;
	for( iter = kad_store_db.begin(); iter != kad_store_db.end(); iter++ ){
		kad_store_t *	kad_store	= *iter;
		// if it is the first element, add the tableheader
		if( iter == kad_store_db.begin() )	oss << wikidbg_html("tableheader", kad_store);
		// display this row
		oss << wikidbg_html("tablerow", kad_store);
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







