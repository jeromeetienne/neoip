/*! \file
    \brief Declaration of the ntudp_npos_eval_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_eval_wikidbg.hpp"
#include "neoip_ntudp_npos_eval.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ntudp_npos_eval_wikidbg_t defines the wikidbg stuff for \ref ntudp_npos_eval_t
 */
class ntudp_npos_eval_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	ntudp_npos_eval_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, ntudp_npos_eval_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("oneword_pageurl"	, ntudp_npos_eval_wikidbg_t::wikidbg_oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, ntudp_npos_eval_wikidbg_t::wikidbg_page_title_attr);	
	keyword_db.insert_page("page"			, ntudp_npos_eval_wikidbg_t::wikidbg_page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string ntudp_npos_eval_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "npos_eval";

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
std::string ntudp_npos_eval_wikidbg_t::wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_npos_eval_t *	npos_eval	= (ntudp_npos_eval_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", npos_eval), wikidbg_html("page_title_attr", npos_eval));
	oss << wikidbg_html("oneword", npos_eval);
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
std::string ntudp_npos_eval_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t ntudp_npos_eval_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	ntudp_npos_eval_t *	npos_eval	= (ntudp_npos_eval_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("ntudp_npos_eval_t Page");
	

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "pserver_pool"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", npos_eval->pserver_pool)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "npos_server"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", npos_eval->npos_server)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "current_res"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &npos_eval->current_res)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "expire_timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &npos_eval->expire_timeout)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(npos_eval->callback)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table for inetreach_db
	oss << h.s_sub1title() << "inetreach_db: " << npos_eval->inetreach_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole ntudp_npos_eval_t::inetreach_db
	std::set<ntudp_npos_inetreach_t *> &		inetreach_db = npos_eval->inetreach_db;
	std::set<ntudp_npos_inetreach_t *>::iterator	inetreach_iter;
	for(inetreach_iter = inetreach_db.begin(); inetreach_iter != inetreach_db.end();inetreach_iter++){
		ntudp_npos_inetreach_t * npos_inetreach	= *inetreach_iter;
		// if it is the first element, add the tableheader
		if(inetreach_iter == inetreach_db.begin() )
			oss << wikidbg_html("tableheader", npos_inetreach);	
		// display this row
		oss << wikidbg_html("tablerow", npos_inetreach);
	}
	oss << h.e_table();

	// display the table for natted_db
	oss << h.s_sub1title() << "natted_db: "	<< npos_eval->natted_db.size() << h.e_sub1title();
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "natted_aview"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << npos_eval->natted_aview	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "natted_pserver_addr"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << npos_eval->natted_pserver_addr	<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	oss << h.s_table();
	// go thru the whole ntudp_npos_eval_t::natted_db
	std::set<ntudp_npos_saddrecho_t *> &		natted_db = npos_eval->natted_db;
	std::set<ntudp_npos_saddrecho_t *>::iterator	natted_iter;
	for(natted_iter = natted_db.begin(); natted_iter != natted_db.end();natted_iter++){
		ntudp_npos_saddrecho_t * npos_saddrecho	= *natted_iter;
		// if it is the first element, add the tableheader
		if(natted_iter == natted_db.begin() )
			oss << wikidbg_html("tableheader", npos_saddrecho);	
		// display this row
		oss << wikidbg_html("tablerow", npos_saddrecho);
	}
	oss << h.e_table();

	// display the table for natsym_db
	oss << h.s_sub1title() << "natsym_db: " << npos_eval->natsym_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole ntudp_npos_eval_t::natsym_db
	std::set<ntudp_npos_saddrecho_t *> &		natsym_db = npos_eval->natsym_db;
	std::set<ntudp_npos_saddrecho_t *>::iterator	natsym_iter;
	for(natsym_iter = natsym_db.begin(); natsym_iter != natsym_db.end();natsym_iter++){
		ntudp_npos_saddrecho_t * npos_saddrecho	= *natsym_iter;
		// if it is the first element, add the tableheader
		if(natsym_iter == natsym_db.begin() )
			oss << wikidbg_html("tableheader", npos_saddrecho);	
		// display this row
		oss << wikidbg_html("tablerow", npos_saddrecho);
	}
	oss << h.e_table();

	// display the table for natlback_db
	oss << h.s_sub1title() << "natlback_db: " << npos_eval->natlback_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole ntudp_npos_eval_t::natlback_db
	std::set<ntudp_npos_natlback_t *> &		natlback_db = npos_eval->natlback_db;
	std::set<ntudp_npos_natlback_t *>::iterator	natlback_iter;
	for(natlback_iter = natlback_db.begin(); natlback_iter != natlback_db.end();natlback_iter++){
		ntudp_npos_natlback_t * npos_natlback	= *natlback_iter;
		// if it is the first element, add the tableheader
		if(natlback_iter == natlback_db.begin() )
			oss << wikidbg_html("tableheader", npos_natlback);	
		// display this row
		oss << wikidbg_html("tablerow", npos_natlback);
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END







