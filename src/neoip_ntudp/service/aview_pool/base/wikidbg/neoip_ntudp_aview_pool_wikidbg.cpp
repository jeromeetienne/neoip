/*! \file
    \brief Declaration of the ntudp_aview_pool_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_aview_pool_wikidbg.hpp"
#include "neoip_ntudp_aview_pool.hpp"
#include "neoip_ntudp_npos_saddrecho.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ntudp_aview_pool_wikidbg_t defines the wikidbg stuff for \ref ntudp_aview_pool_t
 */
class ntudp_aview_pool_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_oneword(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_menu_item(const std::string &keyword, void *object_ptr)		throw();
	static httpd_err_t	wikidbg_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void ntudp_aview_pool_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw()
{
	// add the keywords
	keyword_db.insert_html("oneword"	, ntudp_aview_pool_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("page_title_attr", ntudp_aview_pool_wikidbg_t::wikidbg_page_title_attr);
	keyword_db.insert_html("menu_item"	, ntudp_aview_pool_wikidbg_t::wikidbg_menu_item);
	keyword_db.insert_page("page"		, ntudp_aview_pool_wikidbg_t::wikidbg_page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string ntudp_aview_pool_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_aview_pool_t *	aview_pool	= (ntudp_aview_pool_t *)object_ptr;
	std::ostringstream	oss;

	oss << aview_pool->size();
	oss << "/";
	oss << aview_pool->saddrecho_db.size() - aview_pool->size();
	
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
std::string ntudp_aview_pool_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_aview_pool_t *	aview_pool	= (ntudp_aview_pool_t *)object_ptr;
	std::ostringstream	oss;

	// build the title attribute
	oss << "title=\"";
	oss << "Address View: ";
	oss << aview_pool->size() 					<< " available";
	oss << " / ";
	oss << aview_pool->saddrecho_db.size() - aview_pool->size()	<< " inestablishement ";	
	oss << "\"";
	// return the built string
	return oss.str();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu_item" keyword
 */
std::string ntudp_aview_pool_wikidbg_t::wikidbg_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_aview_pool_t *	aview_pool	= (ntudp_aview_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", aview_pool), wikidbg_html("page_title_attr", aview_pool));
	oss << "aview_pool(";
	oss << wikidbg_html("oneword", aview_pool);
	oss << ")";
	oss << h.e_link();
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
httpd_err_t ntudp_aview_pool_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
					, httpd_request_t &request) throw()
{
	ntudp_aview_pool_t *	aview_pool	= (ntudp_aview_pool_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", aview_pool->ntudp_peer);

	// put the title
	oss << h.pagetitle("ntudp_aview_pool_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_needed_aview"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << aview_pool->nb_needed_aview	<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// display all the saddrecho of the pool
	oss << h.s_table();
	std::set<ntudp_npos_saddrecho_t *>::iterator	iter;
	for( iter = aview_pool->saddrecho_db.begin(); iter != aview_pool->saddrecho_db.end(); iter++ ){
		ntudp_npos_saddrecho_t *	npos_saddrecho	= *iter;
		// if it is the first element, add the tableheader
		if( iter == aview_pool->saddrecho_db.begin() )
			oss << wikidbg_html("tableheader", npos_saddrecho);
		// display this row
		oss << wikidbg_html("tablerow", npos_saddrecho);
	}
	oss << h.e_table();
	oss << h.br();

	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







