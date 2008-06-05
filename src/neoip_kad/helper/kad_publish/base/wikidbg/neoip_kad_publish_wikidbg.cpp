/*! \file
    \brief Declaration of the kad_publish_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_kad_publish_wikidbg.hpp"
#include "neoip_kad_publish.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref kad_publish_wikidbg_t defines the wikidbg stuff for \ref kad_publish_t
 */
class kad_publish_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	kad_publish_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, kad_publish_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("oneword_pageurl", kad_publish_wikidbg_t::wikidbg_oneword_pageurl);
	keyword_db.insert_html("page_title_attr", kad_publish_wikidbg_t::wikidbg_page_title_attr);	
	keyword_db.insert_page("page"		, kad_publish_wikidbg_t::wikidbg_page);

	keyword_db.insert_html("tableheader"	, kad_publish_wikidbg_t::wikidbg_tableheader);
	keyword_db.insert_html("tablerow"	, kad_publish_wikidbg_t::wikidbg_tablerow);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string kad_publish_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "kad_publish";

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
std::string kad_publish_wikidbg_t::wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	kad_publish_t *		kad_publish	= (kad_publish_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", kad_publish), wikidbg_html("page_title_attr", kad_publish));
	oss << wikidbg_html("oneword", kad_publish);
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
std::string kad_publish_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t kad_publish_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	kad_publish_t *		kad_publish	= (kad_publish_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("kad_publish_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_peer"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", kad_publish->kad_peer)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_keyid"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_publish->kad_keyid	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_store"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", kad_publish->kad_store)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(kad_publish->callback)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table for recid_db_db
	oss << h.s_sub1title() << "recid_db: " << kad_publish->recid_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole ntudp_ntudp_kad_publish_t::recid_db
	std::list<kad_recid_t> &		recid_db = kad_publish->recid_db;
	std::list<kad_recid_t>::iterator	iter;
	for(iter = recid_db.begin(); iter != recid_db.end(); iter++){
		kad_recid_t &	kad_recid	= *iter;
		// if it is the first element, add the tableheader
		if( iter == recid_db.begin() ){
			oss << h.s_tr();
			oss << h.s_th() << h.s_b() << "recid_db"	<< h.e_b() << h.e_th();
			oss << h.e_tr();
		}
		// display this row
		oss << h.s_tr();
		oss << h.s_td() << kad_recid				<< h.e_td();
		oss << h.e_tr();
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
std::string kad_publish_wikidbg_t::wikidbg_tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "kad_publish"		<< h.e_b() << h.e_th();
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
std::string kad_publish_wikidbg_t::wikidbg_tablerow(const std::string &keyword, void *object_ptr) throw()
{
	kad_publish_t *		kad_publish	= (kad_publish_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", kad_publish)	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END







