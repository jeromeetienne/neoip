/*! \file
    \brief Declaration of the ntudp_pserver_tunnel_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_pserver_tunnel_wikidbg.hpp"
#include "neoip_ntudp_pserver_tunnel.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ntudp_pserver_tunnel_wikidbg_t defines the wikidbg stuff for \ref ntudp_pserver_tunnel_t
 */
class ntudp_pserver_tunnel_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void ntudp_pserver_tunnel_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw()
{
	// add the keywords
	keyword_db.insert_html("tableheader"	, ntudp_pserver_tunnel_wikidbg_t::wikidbg_tableheader);
	keyword_db.insert_html("tablerow"	, ntudp_pserver_tunnel_wikidbg_t::wikidbg_tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "tableheader" keyword
 */
std::string ntudp_pserver_tunnel_wikidbg_t::wikidbg_tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "Client PeerID"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "Client Nonce"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "idle_timeout"	<< h.e_b() << h.e_th();
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
std::string ntudp_pserver_tunnel_wikidbg_t::wikidbg_tablerow(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_pserver_tunnel_t*	pserver_tunnel	= (ntudp_pserver_tunnel_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << pserver_tunnel->client_peerid			<< h.e_td();
	oss << h.s_td() << pserver_tunnel->client_nonce				<< h.e_td();
	oss << h.s_td() << pserver_tunnel->idle_timeout.get_expire_delay()	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END







