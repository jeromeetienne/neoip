/*! \file
    \brief Definition of the \ref bt_http_ecnx_cnx_stats_helper_t class
    
*/

/* system include */
/* local include */
#include "neoip_bt_http_ecnx_cnx_stats_helper.hpp"
#include "neoip_bt_http_ecnx_cnx_stats.hpp"
#include "neoip_html_builder.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                to_html function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a string containing html to representing this bt_http_ecnx_cnx_stats_t
 */
std::string bt_http_ecnx_cnx_stats_helper_t::to_html(const bt_http_ecnx_cnx_stats_t &cnx_stats)	throw()
{
	std::ostringstream	oss;
	html_builder_t		h;
	// build the html
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "uloaded_ovhdlen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(cnx_stats.uloaded_ovhdlen().to_uint64())
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "dloaded_ovhdlen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(cnx_stats.dloaded_ovhdlen().to_uint64())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "dloaded_datalen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(cnx_stats.dloaded_datalen().to_uint64())
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "total overhead"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::percent_string( (cnx_stats.uloaded_ovhdlen().to_double()
								+cnx_stats.dloaded_ovhdlen().to_double())
								/ cnx_stats.dloaded_datalen().to_double())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "iov_nb"			<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << cnx_stats.iov_nb()
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "iov_total_len"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(cnx_stats.iov_total_len().to_uint64())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "average iov len"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string( size_t(cnx_stats.iov_total_len().to_double() / cnx_stats.iov_nb()))
					<< h.e_td();
			oss << h.e_tr();			
	oss << h.e_table();
	oss << h.br();
	// return the just built html
	return oss.str();
}


NEOIP_NAMESPACE_END


