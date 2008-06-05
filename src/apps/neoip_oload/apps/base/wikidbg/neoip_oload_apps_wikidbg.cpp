/*! \file
    \brief Declaration of the oload_apps_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_oload_apps_wikidbg.hpp"
#include "neoip_oload_apps.hpp"
#include "neoip_oload_mod_vapi.hpp"
#include "neoip_oload_mod_type.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref oload_apps_http_t defines the wikidbg stuff for \ref oload_apps_t
 */
class oload_apps_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	oload_apps_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, oload_apps_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", oload_apps_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", oload_apps_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, oload_apps_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string oload_apps_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	oload_apps_t *		oload_apps	= (oload_apps_t *)object_ptr;
	std::ostringstream	oss;

	oss << oload_apps->bt_ezsession()->bt_session()->local_peerid();

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
std::string oload_apps_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	oload_apps_t *		oload_apps	= (oload_apps_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", oload_apps), wikidbg_html("page_title_attr", oload_apps));
	oss << wikidbg_html("oneword", oload_apps);
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
std::string oload_apps_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t oload_apps_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	oload_apps_t *		oload_apps	= (oload_apps_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("oload_apps_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_listener"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", oload_apps->http_listener())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_httpo_listener_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", oload_apps->httpo_listener())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_ezsession"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", oload_apps->bt_ezsession())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "apps_httpdetect"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", oload_apps->m_apps_httpdetect)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_cast_prefetch_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", oload_apps->m_cast_prefetch)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "oload_httpo_ctrl_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", oload_apps->m_httpo_ctrl)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "oload_flash_xdom_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", oload_apps->m_flash_xdom)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the oload_mod_vapi_t
	oss << h.s_table_packed_noborder();
	oss << h.s_tr();
	oss << h.s_td() << h.s_b() << "oload_mod_vapi_t list"	<< h.e_b() << h.e_td();
	oss << h.s_td() << ": ";
	// go thru the whole oload_apps_t::mod_db
	std::list<oload_mod_vapi_t *> &		mod_db = oload_apps->mod_db;
	std::list<oload_mod_vapi_t *>::iterator	iter;
	for( iter = mod_db.begin(); iter != mod_db.end(); iter++ ){
		oload_mod_vapi_t *	mod_vapi	= *iter;
		if( iter != mod_db.begin() )	oss << "&nbsp;/&nbsp;";	
		oss << mod_vapi->type();
	}
	oss << h.e_td();
	oss << h.e_tr();
	oss << h.e_table();	

	
	// display the table of all the current oload_swarm_t
	oss << h.s_sub1title() << "List of all current oload_swarm_t: "
					<< oload_apps->swarm_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole oload_apps_t::swarm_db
	std::list<oload_swarm_t *> &		swarm_db = oload_apps->swarm_db;
	std::list<oload_swarm_t *>::iterator	iter_swarm;
	for( iter_swarm = swarm_db.begin(); iter_swarm != swarm_db.end(); iter_swarm++ ){
		oload_swarm_t *	oload_swarm	= *iter_swarm;
		// if it is the first element, add the tableheader
		if( iter_swarm == swarm_db.begin() )	oss << wikidbg_html("tableheader", oload_swarm);	
		// display this row
		oss << wikidbg_html("tablerow", oload_swarm);
	}
	oss << h.e_table();


	// display the table of all the current oload_itor_t
	oss << h.s_sub1title() << "List of all current oload_itor_t: "
					<< oload_apps->itor_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole oload_apps_t::itor_db
	std::list<oload_itor_t *> &		itor_db = oload_apps->itor_db;
	std::list<oload_itor_t *>::iterator	iter_itor;
	for( iter_itor = itor_db.begin(); iter_itor != itor_db.end(); iter_itor++ ){
		oload_itor_t *	oload_itor	= *iter_itor;
		// if it is the first element, add the tableheader
		if( iter_itor == itor_db.begin() )	oss << wikidbg_html("tableheader", oload_itor);	
		// display this row
		oss << wikidbg_html("tablerow", oload_itor);
	}
	oss << h.e_table();
	// return no error
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END







