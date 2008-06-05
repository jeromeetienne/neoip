/*! \file
    \brief Declaration of the bt_httpo_listener_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_httpo_listener_wikidbg.hpp"
#include "neoip_bt_httpo_listener.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_httpo_http_t defines the wikidbg stuff for \ref bt_httpo_listener_t
 */
class bt_httpo_listener_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_httpo_listener_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_httpo_listener_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_httpo_listener_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_httpo_listener_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_httpo_listener_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_httpo_listener_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bt_httpo_listener_t";

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
std::string bt_httpo_listener_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_httpo_listener_t *	httpo_listener	= (bt_httpo_listener_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", httpo_listener), wikidbg_html("page_title_attr", httpo_listener));
	oss << wikidbg_html("oneword", httpo_listener);
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
std::string bt_httpo_listener_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_httpo_listener_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_httpo_listener_t *	httpo_listener	= (bt_httpo_listener_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_httpo_listener_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_listener"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", httpo_listener->http_listener)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

{
	// display the table of all the current httpo_listener_full_t
	oss << h.s_sub1title() << "List of all current httpo_listener_full_t: "
					<< httpo_listener->httpo_full_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_httpo_listener_t::httpo_full_db
	std::list<bt_httpo_full_t *> &		httpo_full_db = httpo_listener->httpo_full_db;
	std::list<bt_httpo_full_t *>::iterator	iter;
	for( iter = httpo_full_db.begin(); iter != httpo_full_db.end(); iter++ ){
		bt_httpo_full_t *	httpo_full	= *iter;
		// if it is the first element, add the tableheader
		if( iter == httpo_full_db.begin() )	oss << wikidbg_html("tableheader", httpo_full);	
		// display this row
		oss << wikidbg_html("tablerow", httpo_full);
	}
	oss << h.e_table();
}

{
	// display the table of all the current bt_httpo_resp_t
	oss << h.s_sub1title() << "List of all current bt_httpo_resp_t: "
					<< httpo_listener->httpo_resp_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_httpo_listener_t::httpo_resp_db
	std::list<bt_httpo_resp_t *> &		httpo_resp_db = httpo_listener->httpo_resp_db;
	std::list<bt_httpo_resp_t *>::iterator	iter;
	for( iter = httpo_resp_db.begin(); iter != httpo_resp_db.end(); iter++ ){
		bt_httpo_resp_t *	httpo_resp	= *iter;
		// if it is the first element, add the tableheader
		if( iter == httpo_resp_db.begin() )	oss << wikidbg_html("tableheader", httpo_resp);	
		// display this row
		oss << wikidbg_html("tablerow", httpo_resp);
	}
	oss << h.e_table();
}		
	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







