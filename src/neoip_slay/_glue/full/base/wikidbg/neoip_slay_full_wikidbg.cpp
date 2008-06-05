/*! \file
    \brief Declaration of the slay_full_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_slay_full_wikidbg.hpp"
#include "neoip_slay_full.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref slay_full_wikidbg_t defines the wikidbg stuff for \ref slay_full_t
 */
class slay_full_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	slay_full_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("subpage"	, slay_full_wikidbg_t::subpage);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "page" keyword
 */
std::string slay_full_wikidbg_t::subpage(const std::string &keyword
							, void *object_ptr)	throw()
{
	slay_full_t *		full_dom	= (slay_full_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// put the title
	oss << h.sub1title("Subpage for slay_full_t:");
	// display the socket_full_vapi_t subpage
	oss << wikidbg_html("subpage", full_dom->full_vapi());

	// return the built string
	return oss.str();
}



NEOIP_NAMESPACE_END







