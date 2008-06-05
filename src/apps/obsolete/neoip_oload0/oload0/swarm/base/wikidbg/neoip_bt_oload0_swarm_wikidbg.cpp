/*! \file
    \brief Declaration of the bt_oload0_swarm_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_oload0_swarm_wikidbg.hpp"
#include "neoip_bt_oload0_swarm.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_oload0_swarm_t
 */
class bt_oload0_swarm_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_oload0_swarm_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_oload0_swarm_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_oload0_swarm_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_oload0_swarm_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_oload0_swarm_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_oload0_swarm_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_oload0_swarm_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_oload0_swarm_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_oload0_swarm_t *	oload0_swarm	= (bt_oload0_swarm_t *)object_ptr;	
	const bt_mfile_t &	bt_mfile	= oload0_swarm->bt_swarm->get_mfile();
	std::ostringstream	oss;

	oss << bt_mfile.infohash();

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
std::string bt_oload0_swarm_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_oload0_swarm_t *	oload0_swarm	= (bt_oload0_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", oload0_swarm), wikidbg_html("page_title_attr", oload0_swarm));
	oss << wikidbg_html("oneword", oload0_swarm);
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
std::string bt_oload0_swarm_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	bt_oload0_swarm_t *	oload0_swarm	= (bt_oload0_swarm_t *)object_ptr;	
	const bt_mfile_t &	bt_mfile	= oload0_swarm->bt_swarm->get_mfile();
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "bt_oload0_swarm_t on " << bt_mfile.infohash();
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
httpd_err_t bt_oload0_swarm_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_oload0_swarm_t *	oload0_swarm	= (bt_oload0_swarm_t *)object_ptr;
	bt_oload0_t *		bt_oload0	= oload0_swarm->bt_oload0;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	
	// add the menu
	oss << wikidbg_html("menu", bt_oload0);
	
	// put the title
	oss << h.pagetitle("bt_oload0_swarm_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_oload0"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", oload0_swarm->bt_oload0)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nested_uri"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << oload0_swarm->nested_uri
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_ecnx_pool"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", oload0_swarm->http_ecnx_pool)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", oload0_swarm->bt_swarm)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	
	// display the table of all the current bt_oload0_swarm_t
	oss << h.s_sub1title() << "List of all current bt_httpo_full_t: "
					<< oload0_swarm->httpo_full_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_oload0_t::httpo_full_db
	std::list<bt_httpo_full_t *> &		httpo_full_db = oload0_swarm->httpo_full_db;
	std::list<bt_httpo_full_t *>::iterator	iter;
	for(iter = httpo_full_db.begin(); iter != httpo_full_db.end(); iter++){
		bt_httpo_full_t *	httpo_full	= *iter;
		// if it is the first element, add the tableheader
		if( iter == httpo_full_db.begin() )	oss << wikidbg_html("tableheader", httpo_full);	
		// display this row
		oss << wikidbg_html("tablerow", httpo_full);
	}
	oss << h.e_table();

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
std::string bt_oload0_swarm_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "infohash"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb_httpo_full"		<< h.e_b() << h.e_th();
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
std::string bt_oload0_swarm_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_oload0_swarm_t *	oload0_swarm	= (bt_oload0_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", oload0_swarm)		<< h.e_td();
	oss << h.s_td() << oload0_swarm->httpo_full_db.size()			<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}
NEOIP_NAMESPACE_END







