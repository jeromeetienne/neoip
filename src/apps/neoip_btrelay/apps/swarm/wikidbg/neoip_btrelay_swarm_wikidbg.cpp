/*! \file
    \brief Declaration of the btrelay_swarm_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_btrelay_swarm_wikidbg.hpp"
#include "neoip_btrelay_swarm.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_relay.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_herr_http_t defines the wikidbg stuff for \ref btrelay_swarm_t
 */
class btrelay_swarm_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	btrelay_swarm_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, btrelay_swarm_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", btrelay_swarm_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", btrelay_swarm_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, btrelay_swarm_wikidbg_t::page);


	keyword_db.insert_html("tableheader"	, btrelay_swarm_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, btrelay_swarm_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string btrelay_swarm_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "btrelay_swarm_t";

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
std::string btrelay_swarm_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	btrelay_swarm_t *	btrelay_swarm	= (btrelay_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", btrelay_swarm), wikidbg_html("page_title_attr", btrelay_swarm));
	oss << wikidbg_html("oneword", btrelay_swarm);
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
std::string btrelay_swarm_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t btrelay_swarm_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	btrelay_swarm_t *	btrelay_swarm	= (btrelay_swarm_t *)object_ptr;
	btrelay_apps_t *	btrelay_apps	= btrelay_swarm->btrelay_apps;
	bt_ezswarm_t *		bt_ezswarm	= btrelay_swarm->bt_ezswarm;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", btrelay_apps);

	// put the title
	oss << h.pagetitle("btrelay_swarm_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "btrelay_apps"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", btrelay_swarm->btrelay_apps)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_ezswarm"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", btrelay_swarm->bt_ezswarm)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cast_mdata"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << btrelay_swarm->cast_mdata()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

if( bt_ezswarm->in_share() ){
	bt_swarm_t *		bt_swarm	= bt_ezswarm->share()->bt_swarm();	 
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	// display the table of all the current bt_pselect_slide_curs_t::pieceprec_arr()
	oss << h.s_sub1title() << "List of status for each pieceidx: "		<< h.e_sub1title();
	oss << h.br();
	oss << "ra= for remote_pavail" 				<< h.br();
	oss << "rw= for remote_pwish"				<< h.br();
	oss << "pl= for piecelen" 				<< h.br();
	oss << "pal= for pieceavaillen" 			<< h.br();
	oss << "ge= for giveexpect"				<< h.br();
	oss << "tc= for takecost"				<< h.br();
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "index"			<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "piece len stuff"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "remote stuff"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "expectation stuff"	<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	// go thru the whole bt_session_t::swarm_db
	for( size_t i = 0; i < bt_mfile.nb_piece(); i ++){
		size_t	pieceidx	= i;
		// produce the html
		oss << h.s_tr();
		oss << h.s_td() << pieceidx	<< h.e_td();

		oss << h.s_td();
			oss << "(pl=" << bt_relay_t::piecelen(bt_swarm, pieceidx);
			oss << ")-(";
			oss << "pal=" << bt_relay_t::pieceavaillen(bt_swarm, pieceidx);
			oss << ")=(tc=" << bt_relay_t::takecost(bt_swarm, pieceidx);
			oss << ")";
			oss << h.e_td();
						
		oss << h.s_td();
			oss << "(ra=" << bt_relay_t::remote_pavail(bt_swarm, pieceidx);
			oss << ")/(";
			oss << "rw=" << bt_relay_t::remote_pwish(bt_swarm, pieceidx);
			oss << ")=" << (double)bt_relay_t::remote_pwish(bt_swarm, pieceidx)
					/ bt_relay_t::remote_pavail(bt_swarm, pieceidx);
			oss << h.e_td();
						
		oss << h.s_td();
			oss << "(ge=" << bt_relay_t::giveexpect(bt_swarm, pieceidx);
			oss << ")-(";
			oss << "(tc=" << bt_relay_t::takecost(bt_swarm, pieceidx);
			oss << ")=" << (double)bt_relay_t::gainexpect(bt_swarm, pieceidx);
			oss << h.e_td();

		oss << h.e_tr();
	}
	oss << h.e_table();
}
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
std::string btrelay_swarm_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "infohash"		<< h.e_b() << h.e_th();
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
std::string btrelay_swarm_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	btrelay_swarm_t *		btrelay_swarm	= (btrelay_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", btrelay_swarm)		<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







