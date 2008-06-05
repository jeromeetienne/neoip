/*! \file
    \brief Declaration of the ntudp_npos_inetreach_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_inetreach_wikidbg.hpp"
#include "neoip_ntudp_npos_inetreach.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ntudp_npos_inetreach_wikidbg_t defines the wikidbg stuff for \ref ntudp_npos_inetreach_t
 */
class ntudp_npos_inetreach_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	wikidbg_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	wikidbg_tableheader(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_tablerow(const std::string &keyword, void *object_ptr)		throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	ntudp_npos_inetreach_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, ntudp_npos_inetreach_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("oneword_pageurl", ntudp_npos_inetreach_wikidbg_t::wikidbg_oneword_pageurl);
	keyword_db.insert_html("page_title_attr", ntudp_npos_inetreach_wikidbg_t::wikidbg_page_title_attr);	
	keyword_db.insert_page("page"		, ntudp_npos_inetreach_wikidbg_t::wikidbg_page);

	keyword_db.insert_html("tableheader"	, ntudp_npos_inetreach_wikidbg_t::wikidbg_tableheader);
	keyword_db.insert_html("tablerow"	, ntudp_npos_inetreach_wikidbg_t::wikidbg_tablerow);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string ntudp_npos_inetreach_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "npos_inetreach";

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
std::string ntudp_npos_inetreach_wikidbg_t::wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_npos_inetreach_t *npos_inetreach	= (ntudp_npos_inetreach_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", npos_inetreach), wikidbg_html("page_title_attr", npos_inetreach));
	oss << wikidbg_html("oneword", npos_inetreach);
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
std::string ntudp_npos_inetreach_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t ntudp_npos_inetreach_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	ntudp_npos_inetreach_t *npos_inetreach	= (ntudp_npos_inetreach_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("ntudp_npos_inetreach_t Page");
	
	oss << "This object uses a remote ntudp_pserver_t to test if the local peer is inetreach. ";
	oss << "The protocol is :";
	oss << h.br();	
	oss << "(i) local peer register a nonce to the local npos_server ";
	oss << h.br();
	oss << "(ii) local peer send a request to remote ntudp_pserver_t with the public "
					<< "view of the local listen addr.";
	oss << h.br();
	oss << "(iii) remote ntudp_pserver_t send back a reply to the local peer via the request cnx "
					<< " and send a probe to the public view of the listen addr "
					<< " containing the request_nonce";
	oss << h.br();
	oss << "(iv) it is considered inetreach when the npos_server receive the probe ";
	oss << " or considered not inetreach when nb_receved_reply exceed a given amount.";
	
	
	oss << h.br();
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "request_nonce"			<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << npos_inetreach->request_nonce	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "listen_addr_pview"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << npos_inetreach->listen_addr_pview	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_recved_reply"			<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << npos_inetreach->nb_recved_reply	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "npos_server"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", npos_inetreach->npos_server)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "pserver_pool"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", npos_inetreach->pserver_pool)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "pserver_addr"			<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << npos_inetreach->get_pserver_addr()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "udp_client"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", npos_inetreach->udp_client)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "rxmit_timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &npos_inetreach->rxmit_timeout)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(npos_inetreach->callback)
					<< h.e_td();
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
std::string ntudp_npos_inetreach_wikidbg_t::wikidbg_tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "npos_inetreach"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "pserver_addr"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "request_nonce"		<< h.e_b() << h.e_th();
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
std::string ntudp_npos_inetreach_wikidbg_t::wikidbg_tablerow(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_npos_inetreach_t*	npos_inetreach	= (ntudp_npos_inetreach_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", npos_inetreach)	<< h.e_td();
	oss << h.s_td() << npos_inetreach->get_pserver_addr()			<< h.e_td();
	oss << h.s_td() << npos_inetreach->request_nonce			<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END







