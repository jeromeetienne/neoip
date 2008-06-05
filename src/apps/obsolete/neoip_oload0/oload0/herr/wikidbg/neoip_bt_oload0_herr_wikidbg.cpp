/*! \file
    \brief Declaration of the bt_oload0_herr_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_oload0_herr_wikidbg.hpp"
#include "neoip_bt_oload0_herr.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_herr_http_t defines the wikidbg stuff for \ref bt_oload0_herr_t
 */
class bt_oload0_herr_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_oload0_herr_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_oload0_herr_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_oload0_herr_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_oload0_herr_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_oload0_herr_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_oload0_herr_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bt_oload0_herr_t";

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
std::string bt_oload0_herr_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_oload0_herr_t *	oload0_herr	= (bt_oload0_herr_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", oload0_herr), wikidbg_html("page_title_attr", oload0_herr));
	oss << wikidbg_html("oneword", oload0_herr);
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
std::string bt_oload0_herr_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_oload0_herr_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_oload0_herr_t *	oload0_herr	= (bt_oload0_herr_t *)object_ptr;
	bt_oload0_t *		bt_oload0	= oload0_herr->bt_oload0;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	
	// add the menu
	oss << wikidbg_html("menu", bt_oload0);
	
	// put the title
	oss << h.pagetitle("bt_oload0_herr_t Page");

	
	// display the table of all the current bt_oload0_herr_t
	oss << h.s_sub1title() << "List of all current bt_httpo_full_t: "
					<< oload0_herr->httpo_full_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_oload0_t::httpo_full_db
	std::list<bt_httpo_full_t *> &		httpo_full_db = oload0_herr->httpo_full_db;
	std::list<bt_httpo_full_t *>::iterator	iter;
	for(iter = httpo_full_db.begin(); iter != httpo_full_db.end(); iter++){
		bt_httpo_full_t *	httpo_full	= *iter;
		// if it is the first element, add the tableheader
		if( iter == httpo_full_db.begin() )	oss << wikidbg_html("tableheader", httpo_full);	
		// display this row
		oss << wikidbg_html("tablerow", httpo_full);
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







