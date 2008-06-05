/*! \file
    \brief Declaration of the socket_itor_stcp_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_socket_itor_stcp_wikidbg.hpp"
#include "neoip_socket_itor_stcp.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref socket_itor_stcp_wikidbg_t defines the wikidbg stuff for \ref socket_itor_stcp_t
 */
class socket_itor_stcp_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	subpage(const std::string &keyword, void *object_ptr)	throw();
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	socket_itor_stcp_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("subpage"	, socket_itor_stcp_wikidbg_t::subpage);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "page" keyword
 */
std::string socket_itor_stcp_wikidbg_t::subpage(const std::string &keyword
							, void *object_ptr)	throw()
{
	socket_itor_stcp_t *	itor_dom	= (socket_itor_stcp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// put the title
	oss << h.sub1title("Subpage for socket_itor_stcp_t:");
	oss << wikidbg_html("subpage", itor_dom->m_tcp_client);
	oss << wikidbg_html("subpage", itor_dom->m_slay_itor);
	// return the built string
	return oss.str();
}



NEOIP_NAMESPACE_END







