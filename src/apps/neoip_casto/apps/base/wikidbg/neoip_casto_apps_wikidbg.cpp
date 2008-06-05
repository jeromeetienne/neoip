/*! \file
    \brief Declaration of the casto_apps_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_casto_apps_wikidbg.hpp"
#include "neoip_casto_apps.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref casto_apps_http_t defines the wikidbg stuff for \ref casto_apps_t
 */
class casto_apps_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	casto_apps_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, casto_apps_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", casto_apps_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", casto_apps_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, casto_apps_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string casto_apps_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	casto_apps_t *		casto_apps	= (casto_apps_t *)object_ptr;
	std::ostringstream	oss;

	oss << casto_apps->bt_ezsession()->bt_session()->local_peerid();

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
std::string casto_apps_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	casto_apps_t *		casto_apps	= (casto_apps_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", casto_apps), wikidbg_html("page_title_attr", casto_apps));
	oss << wikidbg_html("oneword", casto_apps);
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
std::string casto_apps_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t casto_apps_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	casto_apps_t *		casto_apps	= (casto_apps_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("casto_apps_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_ezsession"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casto_apps->bt_ezsession())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "io_pfile_dirpath"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casto_apps->io_pfile_dirpath()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "dfl_mdata_srv_uri"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casto_apps->dfl_mdata_srv_uri()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_listener"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casto_apps->http_listener())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_httpo_listener_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casto_apps->httpo_listener())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "apps_httpdetect"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casto_apps->m_apps_httpdetect)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_cast_prefetch_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casto_apps->m_cast_prefetch)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current casto_swarm_t
	oss << h.s_sub1title() << "List of all current casto_swarm_t: "
					<< casto_apps->swarm_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole casto_apps_t::swarm_db
	std::list<casto_swarm_t *> &		swarm_db = casto_apps->swarm_db;
	std::list<casto_swarm_t *>::iterator	iter_swarm;
	for( iter_swarm = swarm_db.begin(); iter_swarm != swarm_db.end(); iter_swarm++ ){
		casto_swarm_t *	casto_swarm	= *iter_swarm;
		// if it is the first element, add the tableheader
		if( iter_swarm == swarm_db.begin() )	oss << wikidbg_html("tableheader", casto_swarm);	
		// display this row
		oss << wikidbg_html("tablerow", casto_swarm);
	}
	oss << h.e_table();


	// display the table of all the current casto_itor_t
	oss << h.s_sub1title() << "List of all current casto_itor_t: "
					<< casto_apps->itor_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole casto_apps_t::itor_db
	std::list<casto_itor_t *> &		itor_db = casto_apps->itor_db;
	std::list<casto_itor_t *>::iterator	iter_itor;
	for( iter_itor = itor_db.begin(); iter_itor != itor_db.end(); iter_itor++ ){
		casto_itor_t *	casto_itor	= *iter_itor;
		// if it is the first element, add the tableheader
		if( iter_itor == itor_db.begin() )	oss << wikidbg_html("tableheader", casto_itor);	
		// display this row
		oss << wikidbg_html("tablerow", casto_itor);
	}
	oss << h.e_table();
	// return no error
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END







