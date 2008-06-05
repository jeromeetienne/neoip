/*! \file
    \brief Declaration of the kad_listener_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_kad_listener_wikidbg.hpp"
#include "neoip_kad_listener.hpp"
#include "neoip_ndiag_watch.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref kad_listener_http_t defines the wikidbg stuff for \ref kad_listener_t
 */
class kad_listener_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	kad_listener_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, kad_listener_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl"	, kad_listener_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, kad_listener_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"			, kad_listener_wikidbg_t::page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string kad_listener_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	kad_listener_t *	kad_listener	= (kad_listener_t *)object_ptr;
	std::ostringstream	oss;

	oss << kad_listener->listen_pview();

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
std::string kad_listener_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	kad_listener_t *	kad_listener	= (kad_listener_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", kad_listener), wikidbg_html("page_title_attr", kad_listener));
	oss << wikidbg_html("oneword", kad_listener);
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
std::string kad_listener_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	kad_listener_t *	kad_listener	= (kad_listener_t *)object_ptr;
	std::ostringstream	oss;

	oss << "title=\"";
	if( kad_listener->listen_pview().is_null() ){
		oss << "not inetreachable";
	}else{
		oss << "inetreachable via " << kad_listener->listen_pview();
	}
	oss << " with ";
	oss << kad_listener->peer_db.size()	<< " kad_peer_t attached.";
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
httpd_err_t kad_listener_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	kad_listener_t *	kad_listener	= (kad_listener_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("kad_listener_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Listen outter addr local view"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_listener->listen_lview()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Listen outter addr public view"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_listener->listen_pview()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nslan listener"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", kad_listener->nslan_listener())<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nslan peer"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", kad_listener->nslan_peer())<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ndiag_watch"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ndiag_watch_get())<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current kad_peer_t
	oss << h.s_sub1title() << "List of all current kad_peer_t: "
					<< kad_listener->peer_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole kad_listener_t::peer_db
	std::list<kad_peer_t *> &		peer_db = kad_listener->peer_db;
	std::list<kad_peer_t *>::iterator	iter;
	for( iter = peer_db.begin(); iter != peer_db.end(); iter++ ){
		kad_peer_t *	kad_peer	= *iter;
		// if it is the first element, add the tableheader
		if( iter == peer_db.begin() )	oss << wikidbg_html("tableheader", kad_peer);	
		// display this row
		oss << wikidbg_html("tablerow", kad_peer);
	}
	oss << h.e_table();
	

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







