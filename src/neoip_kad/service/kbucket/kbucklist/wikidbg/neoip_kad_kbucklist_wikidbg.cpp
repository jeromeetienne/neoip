/*! \file
    \brief Declaration of the kad_kbucklist_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_kad_kbucklist_wikidbg.hpp"
#include "neoip_kad_kbucklist.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref kad_kbucklist_http_t defines the wikidbg stuff for \ref kad_kbucket_t::bucklist_t
 */
class kad_kbucklist_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	tableheader(const std::string &keyword, void *object_ptr)	throw();
	static std::string	tablerow(const std::string &keyword, void *object_ptr)		throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	kad_kbucklist_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, kad_kbucklist_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", kad_kbucklist_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", kad_kbucklist_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, kad_kbucklist_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, kad_kbucklist_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, kad_kbucklist_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string kad_kbucklist_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	kad_kbucket_t::bucklist_t *	kad_kbucklist	= (kad_kbucket_t::bucklist_t *)object_ptr;
	std::ostringstream		oss;

	oss << kad_kbucklist->bucknode_lru.size() << " nodes";

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
std::string kad_kbucklist_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	kad_kbucket_t::bucklist_t *		kad_kbucklist	= (kad_kbucket_t::bucklist_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", kad_kbucklist), wikidbg_html("page_title_attr", kad_kbucklist));
	oss << wikidbg_html("oneword", kad_kbucklist);
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
std::string kad_kbucklist_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t kad_kbucklist_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	kad_kbucket_t::bucklist_t *	kad_kbucklist	= (kad_kbucket_t::bucklist_t *)object_ptr;
	std::ostringstream &		oss 		= request.get_reply();
	html_builder_t			h;

	// add the menu
	oss << wikidbg_html("menu", kad_kbucklist->kad_kbucket->kad_peer);
	
	// put the title
	oss << h.pagetitle("kad_kbucket_t::bucklist_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Refresh timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_kbucklist->refresh_timeout	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "refreshing kad_closestnode"<< h.e_b() << h.e_td();
			oss << h.s_td() << ": ";
			if( !kad_kbucklist->kad_closestnode )	oss << "not inprogress";
			else	wikidbg_html("oneword_pageurl", kad_kbucklist->kad_closestnode);
			oss << h.e_td() << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current kad_peer_t
	oss << h.s_sub1title() << "List of all kad_kbucket_t::bucknode_t : "
					<< kad_kbucklist->bucknode_lru.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole kad_kbucket_t::bucklist_db
	std::list<kad_kbucket_t::bucknode_t *> &	bucknode_lru = kad_kbucklist->bucknode_lru;
	std::list<kad_kbucket_t::bucknode_t *>::iterator	iter;
	for( iter = bucknode_lru.begin(); iter != bucknode_lru.end(); iter++ ){
		kad_kbucket_t::bucknode_t *	bucknode	= *iter;
		// if it is the first element, add the tableheader
		if( iter == bucknode_lru.begin() )	oss << wikidbg_html("tableheader", bucknode);	
		// display this row
		oss << wikidbg_html("tablerow", bucknode);
	}
	oss << h.e_table();
	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "tableheader" keyword
 */
std::string kad_kbucklist_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "Index"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb nodes"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "next refresh"	<< h.e_b() << h.e_th();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "tablerow" keyword
 */
std::string kad_kbucklist_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	kad_kbucket_t::bucklist_t *	kad_kbucklist	= (kad_kbucket_t::bucklist_t *)object_ptr;
	kad_kbucket_t *			kad_kbucket	= kad_kbucklist->kad_kbucket;
	std::ostringstream		oss;
	html_builder_t			h;

	oss << h.s_tr();
	oss << h.s_td() << h.s_link(wikidbg_url("page", kad_kbucklist), wikidbg_html("page_title_attr", kad_kbucklist))
				<< kad_kbucket->get_bucket_idx(kad_kbucklist)
				<< h.e_link() << h.e_td();
	oss << h.s_td() << kad_kbucklist->bucknode_lru.size()			<< h.e_td();
	oss << h.s_td() << kad_kbucklist->refresh_timeout			<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







