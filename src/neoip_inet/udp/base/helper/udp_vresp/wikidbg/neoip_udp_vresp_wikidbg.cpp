/*! \file
    \brief Declaration of the udp_vresp_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_udp_vresp_wikidbg.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref udp_vresp_wikidbg_t defines the wikidbg stuff for \ref udp_vresp_t
 */
class udp_vresp_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	udp_vresp_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, udp_vresp_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("oneword_pageurl"	, udp_vresp_wikidbg_t::wikidbg_oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, udp_vresp_wikidbg_t::wikidbg_page_title_attr);	
	keyword_db.insert_page("page"			, udp_vresp_wikidbg_t::wikidbg_page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string udp_vresp_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	udp_vresp_t *		udp_vresp	= (udp_vresp_t *)object_ptr;
	std::ostringstream	oss;

	oss << "udp_vresp " << udp_vresp->listen_addr();

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
std::string udp_vresp_wikidbg_t::wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	udp_vresp_t *		udp_vresp	= (udp_vresp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", udp_vresp), wikidbg_html("page_title_attr", udp_vresp));
	oss << wikidbg_html("oneword", udp_vresp);
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
std::string udp_vresp_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	udp_vresp_t *		udp_vresp	= (udp_vresp_t *)object_ptr;
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "Listen on " << udp_vresp->listen_addr();
	oss << " with " << udp_vresp->reg_cb_db.size() << " pkttype registered.";
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
httpd_err_t udp_vresp_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	udp_vresp_t *		udp_vresp	= (udp_vresp_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("udp_vresp_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "udp_resp"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", udp_vresp->udp_resp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "listen_addr"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << udp_vresp->listen_addr()	<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	// display the table for reg_cb_db_db
	oss << h.s_sub1title() << "reg_cb_db: " << udp_vresp->reg_cb_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole ntudp_udp_vresp_t::reg_cb_db
	udp_vresp_t::reg_cb_db_t &		reg_cb_db = udp_vresp->reg_cb_db;
	udp_vresp_t::reg_cb_db_t::iterator	iter;
	for(iter = reg_cb_db.begin(); iter != reg_cb_db.end(); iter++){
		udp_vresp_t::reg_cb_t &	reg_cb	= iter->second;
		// if it is the first element, add the tableheader
		if( iter == reg_cb_db.begin() ){
			oss << h.s_tr();
			oss << h.s_th() << h.s_b() << "pkttype"		<< h.e_b() << h.e_th();
			oss << h.s_th() << h.s_b() << "callback"	<< h.e_b() << h.e_th();
			oss << h.s_th() << h.s_b() << "userptr"		<< h.e_b() << h.e_th();
			oss << h.e_tr();
		}
		// display this row
		oss << h.s_tr();
		oss << h.s_td() << (int)reg_cb.pkttype				<< h.e_td();
		oss << h.s_td() << wikidbg_html_callback(reg_cb.callback)	<< h.e_td();
		oss << h.s_td() << reg_cb.userptr				<< h.e_td();
		oss << h.e_tr();
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END







