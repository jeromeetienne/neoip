/*! \file
    \brief Declaration of the upnp_bindport_wikidbg_t

\par Possible Improvement
- get the data from the http_sclient_t to determine the info on this upnp_bindport_t ?
  - the other alternative is to store the data in upnp_bindport_t itself
    - just to be queriable ?
    - dunno yet

*/

/* system include */
/* local include */
#include "neoip_upnp_bindport_wikidbg.hpp"
#include "neoip_upnp_bindport.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref upnp_bindport_http_t defines the wikidbg stuff for \ref upnp_bindport_t
 */
class upnp_bindport_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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

/** \brief define all the handled keyword and their attached bindportbacks
 */
void	upnp_bindport_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, upnp_bindport_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", upnp_bindport_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", upnp_bindport_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, upnp_bindport_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string upnp_bindport_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	upnp_bindport_t *	upnp_bindport	= (upnp_bindport_t *)object_ptr;
	std::ostringstream	oss;

	oss << "upnp bindport for " << upnp_bindport->ipport_lview();

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
std::string upnp_bindport_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	upnp_bindport_t *	upnp_bindport	= (upnp_bindport_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", upnp_bindport), wikidbg_html("page_title_attr", upnp_bindport));
	oss << wikidbg_html("oneword", upnp_bindport);
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
std::string upnp_bindport_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t upnp_bindport_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	upnp_bindport_t *	upnp_bindport	= (upnp_bindport_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("upnp_bindport_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "upnp_watch"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", upnp_bindport->upnp_watch)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ipport_lview"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << upnp_bindport->ipport_lview()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ipport_pview"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << upnp_bindport->ipport_pview()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "upnp_sockfam"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << upnp_bindport->upnp_sockfam()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "getport_nbretry"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << upnp_bindport->getport_nbretry
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "description_str"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << upnp_bindport->description_str
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "itor_expire_timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &upnp_bindport->itor_expire_timeout)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "upnp_addport"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", upnp_bindport->call_addport)
						<< " (next scheduled in "
						<< wikidbg_html("oneword_pageurl", &upnp_bindport->addport_timeout)	
						<< ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "is_bound"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << upnp_bindport->is_bound()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(upnp_bindport->callback)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







