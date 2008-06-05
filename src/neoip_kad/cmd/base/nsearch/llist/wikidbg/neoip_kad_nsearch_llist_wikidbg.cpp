/*! \file
    \brief Declaration of the kad_nsearch_llist_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_kad_nsearch_llist_wikidbg.hpp"
#include "neoip_kad_nsearch_llist.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref kad_nsearch_llist_wikidbg_t defines the wikidbg stuff for \ref kad_nsearch_llist_t
 */
class kad_nsearch_llist_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	wikidbg_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	wikidbg_tableheader(const std::string &keyword, void *object_ptr)throw();
	static std::string	wikidbg_tablerow(const std::string &keyword, void *object_ptr)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	kad_nsearch_llist_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, kad_nsearch_llist_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("oneword_pageurl", kad_nsearch_llist_wikidbg_t::wikidbg_oneword_pageurl);
	keyword_db.insert_html("page_title_attr", kad_nsearch_llist_wikidbg_t::wikidbg_page_title_attr);	
	keyword_db.insert_page("page"		, kad_nsearch_llist_wikidbg_t::wikidbg_page);

	keyword_db.insert_html("tableheader"	, kad_nsearch_llist_wikidbg_t::wikidbg_tableheader);
	keyword_db.insert_html("tablerow"	, kad_nsearch_llist_wikidbg_t::wikidbg_tablerow);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string kad_nsearch_llist_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "kad_nsearch_llist";

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
std::string kad_nsearch_llist_wikidbg_t::wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	kad_nsearch_llist_t *	nsearch_llist	= (kad_nsearch_llist_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", nsearch_llist), wikidbg_html("page_title_attr", nsearch_llist));
	oss << wikidbg_html("oneword", nsearch_llist);
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
std::string kad_nsearch_llist_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t kad_nsearch_llist_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	kad_nsearch_llist_t *	nsearch_llist	= (kad_nsearch_llist_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("kad_nsearch_llist_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_nsearch"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", nsearch_llist->kad_nsearch)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_succ_useless_query"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nsearch_llist->nb_succ_useless_query	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "findsome_max_nb_rec"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nsearch_llist->findsome_max_nb_rec	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "caching_candidate"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nsearch_llist->caching_candidate	<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current kad_peer_t
	oss << h.s_sub1title() << "List of all current kad_nsearch_lnode_t: "
					<< nsearch_llist->lnode_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole nsearch_llist_t::lnode_db
	kad_nsearch_llist_t::lnode_db_t &		lnode_db = nsearch_llist->lnode_db;
	kad_nsearch_llist_t::lnode_db_t::iterator	iter;
	for( iter = lnode_db.begin(); iter != lnode_db.end(); iter++ ){
		kad_nsearch_lnode_t *	nsearch_lnode	= iter->second;
		// if it is the first element, add the tableheader
		if( iter == lnode_db.begin() )	oss << wikidbg_html("tableheader", nsearch_lnode);	
		// display this row
		oss << wikidbg_html("tablerow", nsearch_lnode);
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
std::string kad_nsearch_llist_wikidbg_t::wikidbg_tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "kad_nsearch_llist"		<< h.e_b() << h.e_th();
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
std::string kad_nsearch_llist_wikidbg_t::wikidbg_tablerow(const std::string &keyword, void *object_ptr) throw()
{
	kad_nsearch_llist_t *	nsearch_llist	= (kad_nsearch_llist_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", nsearch_llist)	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END







