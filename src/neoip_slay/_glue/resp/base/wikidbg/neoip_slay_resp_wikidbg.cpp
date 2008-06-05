/*! \file
    \brief Declaration of the slay_resp_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_slay_resp_wikidbg.hpp"
#include "neoip_slay_resp.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref slay_resp_wikidbg_t defines the wikidbg stuff for \ref slay_resp_t
 */
class slay_resp_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	slay_resp_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("subpage"	, slay_resp_wikidbg_t::subpage);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "page" keyword
 */
std::string slay_resp_wikidbg_t::subpage(const std::string &keyword
							, void *object_ptr)	throw()
{
	slay_resp_t *		resp_dom	= (slay_resp_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// put the title
	oss << h.sub1title("Subpage for slay_resp_t:");
	// display the socket_resp_vapi_t subpage
	oss << wikidbg_html("subpage", resp_dom->resp_vapi());

	// return the built string
	return oss.str();
}



NEOIP_NAMESPACE_END







