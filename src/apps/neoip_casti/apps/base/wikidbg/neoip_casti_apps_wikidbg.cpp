/*! \file
    \brief Declaration of the casti_apps_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_casti_apps_wikidbg.hpp"
#include "neoip_casti_apps.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref casti_apps_http_t defines the wikidbg stuff for \ref casti_apps_t
 */
class casti_apps_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	casti_apps_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw()
{
	// add the keywords
	keyword_db.insert_html("oneword"	, casti_apps_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", casti_apps_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", casti_apps_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, casti_apps_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string casti_apps_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	casti_apps_t *		casti_apps	= (casti_apps_t *)object_ptr;
	std::ostringstream	oss;

	oss << casti_apps->bt_ezsession()->bt_session()->local_peerid();

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
std::string casti_apps_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	casti_apps_t *		casti_apps	= (casti_apps_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", casti_apps), wikidbg_html("page_title_attr", casti_apps));
	oss << wikidbg_html("oneword", casti_apps);
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
std::string casti_apps_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t casti_apps_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	casti_apps_t *		casti_apps	= (casti_apps_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("casti_apps_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_ezsession"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_apps->bt_ezsession())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "io_pfile_dirpath"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_apps->io_pfile_dirpath()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "dfl_http_peersrc_uri"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_apps->dfl_http_peersrc_uri()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "dfl_mdata_srv_uri"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_apps->dfl_mdata_srv_uri()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_listener"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_apps->http_listener())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "rtmp_cam_listener"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_apps->rtmp_cam_listener())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "inetreach_httpd"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_apps->m_inetreach_httpd)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "casti_ctrl_cline"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_apps->m_ctrl_cline)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "casti_ctrl_wpage"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_apps->m_ctrl_wpage)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "mdata_server"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_apps->m_mdata_server)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "apps_httpdetect"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_apps->m_apps_httpdetect)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// display the table of all the current casti_swarm_t
	oss << h.s_sub1title() << "List of all current casti_swarm_t: "
					<< casti_apps->swarm_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole casti_apps_t::swarm_db
	std::list<casti_swarm_t *> &		swarm_db = casti_apps->swarm_db;
	std::list<casti_swarm_t *>::iterator	iter;
	for( iter = swarm_db.begin(); iter != swarm_db.end(); iter++ ){
		casti_swarm_t *	casti_swarm	= *iter;
		// if it is the first element, add the tableheader
		if( iter == swarm_db.begin() )	oss << wikidbg_html("tableheader", casti_swarm);
		// display this row
		oss << wikidbg_html("tablerow", casti_swarm);
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







