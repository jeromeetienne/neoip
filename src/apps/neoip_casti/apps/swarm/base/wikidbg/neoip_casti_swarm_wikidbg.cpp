/*! \file
    \brief Declaration of the casti_swarm_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_casti_swarm_wikidbg.hpp"
#include "neoip_casti_swarm.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_bt_cast_udata.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_herr_http_t defines the wikidbg stuff for \ref casti_swarm_t
 */
class casti_swarm_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	casti_swarm_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, casti_swarm_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", casti_swarm_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", casti_swarm_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, casti_swarm_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, casti_swarm_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, casti_swarm_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string casti_swarm_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "casti_swarm_t";

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
std::string casti_swarm_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	casti_swarm_t *	casti_swarm	= (casti_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", casti_swarm), wikidbg_html("page_title_attr", casti_swarm));
	oss << wikidbg_html("oneword", casti_swarm);
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
std::string casti_swarm_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t casti_swarm_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	casti_swarm_t *		casti_swarm	= (casti_swarm_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("casti_swarm_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cast_id"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->cast_id()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cast_name"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->cast_name()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cast_privtext"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->cast_privtext()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "mdata_srv_uri"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->mdata_srv_uri()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "scasti_uri"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->scasti_uri()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_scasti_mod_type_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->scasti_mod()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_peersrc_uri"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->http_peersrc_uri()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "casti_apps"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_swarm->casti_apps)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_ezswarm"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_swarm->bt_ezswarm())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "casti_swarm_spos_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " <<wikidbg_html("oneword_pageurl", casti_swarm->swarm_spos())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "casti_swarm_udata_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " <<wikidbg_html("oneword_pageurl", casti_swarm->swarm_udata())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "casti_swarm_scasti_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_swarm->swarm_scasti())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_cast_mdata_dopublish_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_swarm->m_mdata_dopublish)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_cast_mdata_unpublish_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", casti_swarm->m_mdata_unpublish)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "pieceq_beg"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->pieceq_beg	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "pieceq_end"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->pieceq_end	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "mdata_nonce"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->mdata_nonce	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "current_mdata"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->current_mdata()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "current_udata"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << casti_swarm->current_udata()	<< h.e_td();
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
std::string casti_swarm_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "key"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "cast_name"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "mdata_srv_uri"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "bt_ezswarm_t"	<< h.e_b() << h.e_th();
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
std::string casti_swarm_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	casti_swarm_t *		casti_swarm	= (casti_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", casti_swarm)			<< h.e_td();
	oss << h.s_td() << casti_swarm->cast_name()					<< h.e_td();
	oss << h.s_td() << casti_swarm->mdata_srv_uri()					<< h.e_td();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", casti_swarm->bt_ezswarm())	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







