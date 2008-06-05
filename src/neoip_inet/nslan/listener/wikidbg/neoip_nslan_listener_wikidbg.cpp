/*! \file
    \brief Declaration of the nslan_listener_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_nslan_listener_wikidbg.hpp"
#include "neoip_nslan_listener.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref nslan_listener_http_t defines the wikidbg stuff for \ref nslan_listener_t
 */
class nslan_listener_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	nslan_listener_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, nslan_listener_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl"	, nslan_listener_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, nslan_listener_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"			, nslan_listener_wikidbg_t::page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string nslan_listener_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	nslan_listener_t *	nslan_listener	= (nslan_listener_t *)object_ptr;
	std::ostringstream	oss;

	oss << nslan_listener->get_listen_addr();

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
std::string nslan_listener_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	nslan_listener_t *	nslan_listener	= (nslan_listener_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", nslan_listener), wikidbg_html("page_title_attr", nslan_listener));
	oss << wikidbg_html("oneword", nslan_listener);
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
std::string nslan_listener_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	nslan_listener_t *	nslan_listener	= (nslan_listener_t *)object_ptr;
	std::ostringstream	oss;

	oss << "title=\"";
	oss << nslan_listener->peer_db.size()	<< " nslan_peer_t attached.";
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
httpd_err_t nslan_listener_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	nslan_listener_t *	nslan_listener	= (nslan_listener_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("nslan_listener_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Listen addr"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nslan_listener->get_listen_addr()<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nudp"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", nslan_listener->nudp)<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current nslan_peer_t
	oss << h.s_sub1title() << "List of all current nslan_peer_t: "
					<< nslan_listener->peer_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole nslan_listener_t::peer_db
	std::multimap<nslan_realmid_t, nslan_peer_t *> &	peer_db = nslan_listener->peer_db;
	std::multimap<nslan_realmid_t, nslan_peer_t *>::iterator	iter;
	for( iter = peer_db.begin(); iter != peer_db.end(); iter++ ){
		nslan_peer_t *	nslan_peer	= iter->second;
		// if it is the first element, add the tableheader
		if( iter == peer_db.begin() )	oss << wikidbg_html("tableheader", nslan_peer);	
		// display this row
		oss << wikidbg_html("tablerow", nslan_peer);
	}
	oss << h.e_table();
	

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







