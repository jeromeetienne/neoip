/*! \file
    \brief Declaration of the casto_swarm_httpo_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_casto_swarm_httpo_wikidbg.hpp"
#include "neoip_casto_swarm_httpo.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_herr_http_t defines the wikidbg stuff for \ref casto_swarm_httpo_t
 */
class casto_swarm_httpo_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	casto_swarm_httpo_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, casto_swarm_httpo_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", casto_swarm_httpo_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", casto_swarm_httpo_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, casto_swarm_httpo_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string casto_swarm_httpo_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "casto_swarm_httpo_t";

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
std::string casto_swarm_httpo_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	casto_swarm_httpo_t *	swarm_httpo	= (casto_swarm_httpo_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", swarm_httpo), wikidbg_html("page_title_attr", swarm_httpo));
	oss << wikidbg_html("oneword", swarm_httpo);
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
std::string casto_swarm_httpo_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t casto_swarm_httpo_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	casto_swarm_httpo_t *	swarm_httpo	= (casto_swarm_httpo_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("casto_swarm_httpo_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "casto_swarm"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", swarm_httpo->casto_swarm())
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
		
	// display the table of all the current bt_oload_swarm_t
	oss << h.s_sub1title() << "List of all current bt_httpo_full_t: "
					<< swarm_httpo->m_httpo_full_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_oload_t::httpo_full_db
	std::list<bt_httpo_full_t *> &		httpo_full_db = swarm_httpo->m_httpo_full_db;
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

NEOIP_NAMESPACE_END







