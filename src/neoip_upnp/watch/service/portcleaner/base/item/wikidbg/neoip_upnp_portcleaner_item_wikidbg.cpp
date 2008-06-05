/*! \file
    \brief Declaration of the upnp_portcleaner_item_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_upnp_portcleaner_item_wikidbg.hpp"
#include "neoip_upnp_portcleaner_item.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref upnp_portcleaner_item_http_t defines the wikidbg stuff for \ref upnp_portcleaner_item_t
 */
class upnp_portcleaner_item_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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

/** \brief define all the handled keyword and their attached portcleaner_itembacks
 */
void	upnp_portcleaner_item_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, upnp_portcleaner_item_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", upnp_portcleaner_item_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", upnp_portcleaner_item_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, upnp_portcleaner_item_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, upnp_portcleaner_item_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, upnp_portcleaner_item_wikidbg_t::tablerow);

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string upnp_portcleaner_item_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "portcleaner_item";

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
std::string upnp_portcleaner_item_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	upnp_portcleaner_item_t *portcleaner_item	= (upnp_portcleaner_item_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", portcleaner_item), wikidbg_html("page_title_attr", portcleaner_item));
	oss << wikidbg_html("oneword", portcleaner_item);
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
std::string upnp_portcleaner_item_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t upnp_portcleaner_item_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	upnp_portcleaner_item_t *portcleaner_item= (upnp_portcleaner_item_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("upnp_portcleaner_item_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "portdesc"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << portcleaner_item->portdesc()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "creation_date"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << portcleaner_item->creation_date()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "expire_date"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << portcleaner_item->expire_date()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
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
std::string upnp_portcleaner_item_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "ipport_pview"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "ipport_lview"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "sockfam"			<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "portcleaner_lease"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "portcleaner_nonce"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "expire_date"		<< h.e_b() << h.e_th();
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
std::string upnp_portcleaner_item_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	upnp_portcleaner_item_t *portcleaner_item= (upnp_portcleaner_item_t *)object_ptr;
	upnp_portdesc_t &	portdesc	= portcleaner_item->portdesc();
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << h.s_link(wikidbg_url("page", portcleaner_item), wikidbg_html("page_title_attr", portcleaner_item))
				<< portdesc.ipport_pview() << h.e_link()	<< h.e_td();
	oss << h.s_td() << portdesc.ipport_lview()				<< h.e_td();
	oss << h.s_td() << portdesc.sockfam()					<< h.e_td();
	oss << h.s_td() << portdesc.portcleaner_lease()				<< h.e_td();
	oss << h.s_td() << portdesc.portcleaner_nonce()				<< h.e_td();
	oss << h.s_td() << portcleaner_item->expire_date()			<< h.e_td();
	
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







