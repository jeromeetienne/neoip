/*! \file
    \brief Declaration of the ntudp_npos_res_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_res_wikidbg.hpp"
#include "neoip_ntudp_npos_res.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ntudp_npos_res_wikidbg_t defines the wikidbg stuff for \ref ntudp_npos_res_t
 */
class ntudp_npos_res_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	wikidbg_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	ntudp_npos_res_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, ntudp_npos_res_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("oneword_pageurl"	, ntudp_npos_res_wikidbg_t::wikidbg_oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, ntudp_npos_res_wikidbg_t::wikidbg_page_title_attr);	
	keyword_db.insert_page("page"			, ntudp_npos_res_wikidbg_t::wikidbg_page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string ntudp_npos_res_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "npos_res";

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
std::string ntudp_npos_res_wikidbg_t::wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_npos_res_t *	npos_res	= (ntudp_npos_res_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", npos_res), wikidbg_html("page_title_attr", npos_res));
	oss << wikidbg_html("oneword", npos_res);
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
std::string ntudp_npos_res_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t ntudp_npos_res_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	ntudp_npos_res_t *	npos_res	= (ntudp_npos_res_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("ntudp_npos_res_t Page");
	
	oss << h.br();
if( npos_res->is_null() ){
	oss << "is_null";
}else{
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "natted"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": ";
			if( !npos_res->natted_present() )	oss << "not present";
			else					oss << (npos_res->natted() ? "yes" : "no");
			oss << h.e_td() << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "natlback"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": ";
			if( !npos_res->natlback_present() )	oss << "not present";
			else					oss << (npos_res->natlback() ? "yes" : "no");
			oss << h.e_td() << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "natsym"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": ";
			if( !npos_res->natsym_present() )	oss << "not present";
			else					oss << (npos_res->natsym() ? "yes" : "no");
			oss << h.e_td() << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "inetreach"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": ";
			if( !npos_res->inetreach_present() )	oss << "not present";
			else					oss << (npos_res->inetreach() ? "yes" : "no");
			oss << h.e_td() << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "local_ipaddr_lview"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": ";
			if( !npos_res->local_ipaddr_lview_present() )	oss << "not present";
			else						oss << npos_res->local_ipaddr_lview();
			oss << h.e_td() << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "local_ipaddr_pview"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": ";
			if( !npos_res->local_ipaddr_pview_present() )	oss << "not present";
			else						oss << npos_res->local_ipaddr_pview();
			oss << h.e_td() << h.e_tr();
	oss << h.e_table();	
}
	oss << h.br();

	// return no error
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END







