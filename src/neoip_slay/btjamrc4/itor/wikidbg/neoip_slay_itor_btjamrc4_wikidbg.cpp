/*! \file
    \brief Declaration of the slay_itor_btjamrc4_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_slay_itor_btjamrc4_wikidbg.hpp"
#include "neoip_slay_itor_btjamrc4.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref slay_itor_btjamrc4_wikidbg_t defines the wikidbg stuff for \ref slay_itor_btjamrc4_t
 */
class slay_itor_btjamrc4_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	slay_itor_btjamrc4_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("subpage"	, slay_itor_btjamrc4_wikidbg_t::subpage);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "page" keyword
 */
std::string slay_itor_btjamrc4_wikidbg_t::subpage(const std::string &keyword
							, void *object_ptr)	throw()
{
	slay_itor_btjamrc4_t *	itor_dom	= (slay_itor_btjamrc4_t *)object_ptr;
	std::ostringstream	oss;

	// put the title
	oss << wikidbg_html("subpage", &itor_dom->m_btjamrc4_itor);
	// return the built string
	return oss.str();
}



NEOIP_NAMESPACE_END







