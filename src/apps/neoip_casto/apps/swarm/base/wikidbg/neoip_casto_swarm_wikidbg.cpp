/*! \file
    \brief Declaration of the casto_swarm_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_casto_swarm_wikidbg.hpp"
#include "neoip_casto_swarm.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_herr_http_t defines the wikidbg stuff for \ref casto_swarm_t
 */
class casto_swarm_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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

/** \brief define all the handled keyword and their attached callbacks
 */
void	casto_swarm_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, casto_swarm_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", casto_swarm_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", casto_swarm_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, casto_swarm_wikidbg_t::page);


	keyword_db.insert_html("tableheader"	, casto_swarm_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, casto_swarm_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string casto_swarm_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "casto_swarm_t";

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
std::string casto_swarm_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	casto_swarm_t *	casto_swarm	= (casto_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", casto_swarm), wikidbg_html("page_title_attr", casto_swarm));
	oss << wikidbg_html("oneword", casto_swarm);
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
std::string casto_swarm_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t casto_swarm_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	casto_swarm_t *		casto_swarm	= (casto_swarm_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("casto_swarm_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "casto_apps"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casto_swarm->m_casto_apps)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "mdata_srv_uri"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casto_swarm->mdata_srv_uri()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cast_name"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casto_swarm->cast_name()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cast_privhash"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casto_swarm->cast_privhash()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "swarm_httpo"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casto_swarm->swarm_httpo())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "swarm_udata"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casto_swarm->swarm_udata())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_ezswarm"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casto_swarm->bt_ezswarm)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cast_mdata"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casto_swarm->cast_mdata()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "casti_dtime"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casto_swarm->casti_dtime()	<< h.e_td();
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
std::string casto_swarm_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "infohash"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "cast_name"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "mdata_srv_uri"		<< h.e_b() << h.e_th();
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
std::string casto_swarm_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	casto_swarm_t *		casto_swarm	= (casto_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", casto_swarm)		<< h.e_td();
	oss << h.s_td() << casto_swarm->cast_name()				<< h.e_td();
	oss << h.s_td() << casto_swarm->mdata_srv_uri()				<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







