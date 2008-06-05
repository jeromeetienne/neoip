/*! \file
    \brief Declaration of the ntudp_resp_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_resp_wikidbg.hpp"
#include "neoip_ntudp_resp.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ntudp_resp_wikidbg_t defines the wikidbg stuff for \ref ntudp_resp_t
 */
class ntudp_resp_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_tableheader(const std::string &keyword, void *object_ptr)throw();
	static std::string	wikidbg_tablerow(const std::string &keyword, void *object_ptr)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void ntudp_resp_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw()
{
	// add the keywords
	keyword_db.insert_html("tableheader"	, ntudp_resp_wikidbg_t::wikidbg_tableheader);
	keyword_db.insert_html("tablerow"	, ntudp_resp_wikidbg_t::wikidbg_tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "tableheader" keyword
 */
std::string ntudp_resp_wikidbg_t::wikidbg_tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "Listen PortID"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb direct subresp"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb estarelay subresp"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb reverse subresp"	<< h.e_b() << h.e_th();
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
std::string ntudp_resp_wikidbg_t::wikidbg_tablerow(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_resp_t*		ntudp_resp	= (ntudp_resp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << ntudp_resp->listen_addr().portid()	<< h.e_td();
	oss << h.s_td() << ntudp_resp->direct_db.size()		<< h.e_td();
	oss << h.s_td() << ntudp_resp->estarelay_db.size()	<< h.e_td();
	oss << h.s_td() << ntudp_resp->reverse_db.size()	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END







