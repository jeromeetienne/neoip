/*! \file
    \brief Declaration of the kad_kbucket_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_kad_kbucket_wikidbg.hpp"
#include "neoip_kad_kbucket.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref kad_kbucket_http_t defines the wikidbg stuff for \ref kad_kbucket_t
 */
class kad_kbucket_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	menu_item(const std::string &keyword, void *object_ptr)		throw();
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
void	kad_kbucket_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("menu_item"		, kad_kbucket_wikidbg_t::menu_item);
	keyword_db.insert_html("oneword"		, kad_kbucket_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl"	, kad_kbucket_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, kad_kbucket_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"			, kad_kbucket_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu_item" keyword
 */
std::string kad_kbucket_wikidbg_t::menu_item(const std::string &keyword, void *object_ptr) throw()
{
	kad_kbucket_t *		kad_kbucket	= (kad_kbucket_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("page", kad_kbucket), wikidbg_html("page_title_attr", kad_kbucket));
	oss << "kbucket";
	oss << "(" << wikidbg_html("oneword", kad_kbucket) << ")";
	oss << h.e_link();

	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string kad_kbucket_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	kad_kbucket_t *		kad_kbucket	= (kad_kbucket_t *)object_ptr;
	std::ostringstream	oss;

	oss << kad_kbucket->bucknode_db.size() << " node";

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
std::string kad_kbucket_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	kad_kbucket_t *	kad_kbucket	= (kad_kbucket_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", kad_kbucket), wikidbg_html("page_title_attr", kad_kbucket));
	oss << wikidbg_html("oneword", kad_kbucket);
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
std::string kad_kbucket_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "Click for more information.";
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
httpd_err_t kad_kbucket_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	kad_kbucket_t *		kad_kbucket	= (kad_kbucket_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;


	// add the menu
	oss << wikidbg_html("menu", kad_kbucket->kad_peer);
	
	// put the title
	oss << h.pagetitle("kad_kbucket_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "KBucket Width"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_kbucket->kbucket_width	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb kbucket:"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_kbucket->bucklist_db.size()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Nb Peer"			<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_kbucket->bucknode_db.size()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "k (replication parameter)"<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_kbucket->kbucket_height	<< h.e_td();
			oss << h.e_tr();			
	oss << h.e_table();
	oss << h.br();
	oss << h.br();

	// display the table of all the current kad_peer_t
	oss << h.s_sub1title() << "List of all kad_kbucket_t::bucklist_t : "
					<< kad_kbucket->bucklist_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole kad_kbucket_t::bucklist_db
	std::vector<kad_kbucket_t::bucklist_t *> &		bucklist_db = kad_kbucket->bucklist_db;
	std::vector<kad_kbucket_t::bucklist_t *>::reverse_iterator	iter;
	for( iter = bucklist_db.rbegin(); iter != bucklist_db.rend(); iter++ ){
		kad_kbucket_t::bucklist_t *	bucklist	= *iter;
		// if it is the first element, add the tableheader
		if( iter == bucklist_db.rbegin() )	oss << wikidbg_html("tableheader", bucklist);	
		// display this row
		oss << wikidbg_html("tablerow", bucklist);
	}
	oss << h.e_table();
	

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







