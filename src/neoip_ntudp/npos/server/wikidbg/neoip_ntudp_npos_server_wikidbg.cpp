/*! \file
    \brief Declaration of the ntudp_npos_server_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_server_wikidbg.hpp"
#include "neoip_ntudp_npos_server.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ntudp_npos_server_wikidbg_t defines the wikidbg stuff for \ref ntudp_npos_server_t
 */
class ntudp_npos_server_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	ntudp_npos_server_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, ntudp_npos_server_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("oneword_pageurl"	, ntudp_npos_server_wikidbg_t::wikidbg_oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, ntudp_npos_server_wikidbg_t::wikidbg_page_title_attr);	
	keyword_db.insert_page("page"			, ntudp_npos_server_wikidbg_t::wikidbg_page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string ntudp_npos_server_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "ntudp_npos_server";

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
std::string ntudp_npos_server_wikidbg_t::wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_npos_server_t *	npos_server	= (ntudp_npos_server_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", npos_server), wikidbg_html("page_title_attr", npos_server));
	oss << wikidbg_html("oneword", npos_server);
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
std::string ntudp_npos_server_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t ntudp_npos_server_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	ntudp_npos_server_t *	npos_server	= (ntudp_npos_server_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("ntudp_npos_server_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "udp_vresp"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", npos_server->udp_vresp)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	// display the table for probe_db_db
	oss << h.s_sub1title() << "probe_db: " << npos_server->probe_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole ntudp_ntudp_npos_server_t::probe_db
	ntudp_npos_server_t::probe_db_t &		probe_db = npos_server->probe_db;
	ntudp_npos_server_t::probe_db_t::iterator	iter;
	for(iter = probe_db.begin(); iter != probe_db.end(); iter++){
		ntudp_npos_server_t::probe_cb_t &	probe_cb	= iter->second;
		// if it is the first element, add the tableheader
		if( iter == probe_db.begin() ){
			oss << h.s_tr();
			oss << h.s_th() << h.s_b() << "nonce"		<< h.e_b() << h.e_th();
			oss << h.s_th() << h.s_b() << "callback"	<< h.e_b() << h.e_th();
			oss << h.s_th() << h.s_b() << "userptr"		<< h.e_b() << h.e_th();
			oss << h.e_tr();
		}
		// display this row
		oss << h.s_tr();
		oss << h.s_td() << probe_cb.nonce				<< h.e_td();
		oss << h.s_td() << wikidbg_html_callback(probe_cb.callback)	<< h.e_td();
		oss << h.s_td() << probe_cb.userptr				<< h.e_td();
		oss << h.e_tr();
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}



NEOIP_NAMESPACE_END







