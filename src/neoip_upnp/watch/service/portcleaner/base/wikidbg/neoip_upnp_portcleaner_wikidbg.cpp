/*! \file
    \brief Declaration of the upnp_portcleaner_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_upnp_portcleaner_wikidbg.hpp"
#include "neoip_upnp_portcleaner.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref upnp_portcleaner_http_t defines the wikidbg stuff for \ref upnp_portcleaner_t
 */
class upnp_portcleaner_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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

/** \brief define all the handled keyword and their attached portcleanerbacks
 */
void	upnp_portcleaner_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, upnp_portcleaner_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", upnp_portcleaner_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", upnp_portcleaner_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, upnp_portcleaner_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string upnp_portcleaner_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "upnp portcleaner";

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
std::string upnp_portcleaner_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	upnp_portcleaner_t *	upnp_portcleaner	= (upnp_portcleaner_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", upnp_portcleaner), wikidbg_html("page_title_attr", upnp_portcleaner));
	oss << wikidbg_html("oneword", upnp_portcleaner);
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
std::string upnp_portcleaner_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t upnp_portcleaner_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	upnp_portcleaner_t *	upnp_portcleaner= (upnp_portcleaner_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("upnp_portcleaner_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "upnp_watch"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", upnp_portcleaner->upnp_watch)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "probe_timeout"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &upnp_portcleaner->probe_timeout)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "upnp_scanallport"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", upnp_portcleaner->upnp_scanallport)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// display the table of all the current upnp_portcleaner_item_arr_t
	oss << h.s_sub1title() << "List of all current upnp_portcleaner_item_t: "
					<< upnp_portcleaner->cur_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole upnp_portcleaner_item_arr_t
	upnp_portcleaner_item_arr_t &	cur_db	= upnp_portcleaner->cur_db;
	for(size_t i = 0; i < cur_db.size(); i++ ){
		const upnp_portcleaner_item_t &	item	= cur_db[i];
		// if it is the first element, add the tableheader
		if( i == 0 )	oss << wikidbg_html("tableheader", &item);	
		// display this row
		oss << wikidbg_html("tablerow", &item);
	}
	oss << h.e_table();


	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







