/*! \file
    \brief Definition of the \ref http_client_pool_stat_helper_t class
    
*/

/* system include */
/* local include */
#include "neoip_http_client_pool_stat_helper.hpp"
#include "neoip_http_client_pool_stat.hpp"
#include "neoip_html_builder.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                to_html function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a string containing html to representing this http_client_pool_stat_t
 */
std::string http_client_pool_stat_helper_t::to_html(const http_client_pool_stat_t &pool_stat)	throw()
{
	std::ostringstream	oss;
	html_builder_t		h;
	// build the html
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_cnx_peak"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << pool_stat.nb_cnx_peak()
					<< h.e_td();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_cnx_put"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << pool_stat.nb_cnx_put()
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "nb_cnx_get"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << pool_stat.nb_cnx_get()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_cnx_died_internal"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << pool_stat.nb_cnx_died_internal()
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "nb_cnx_died_external"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << pool_stat.nb_cnx_died_external()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// return the just built html
	return oss.str();
}


NEOIP_NAMESPACE_END


