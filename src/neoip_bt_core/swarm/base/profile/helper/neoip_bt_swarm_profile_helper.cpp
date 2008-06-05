/*! \file
    \brief Definition of the \ref bt_swarm_profile_helper_t class
    
*/

/* system include */
/* local include */
#include "neoip_bt_swarm_profile_helper.hpp"
#include "neoip_bt_swarm_profile.hpp"
#include "neoip_html_builder.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                to_html function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a string containing html to representing this bt_swarm_profile_t
 */
std::string bt_swarm_profile_helper_t::to_html(const bt_swarm_profile_t &swarm_profile)	throw()
{
	std::ostringstream	oss;
	html_builder_t		h;
	// build the html
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "keepalive_period"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_profile.keepalive_period()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "full_idle_timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_profile.full_idle_timeout()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmit_req_maxlen"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_profile.xmit_req_maxlen()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "recv_req_maxlen"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_profile.recv_req_maxlen()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "recv_cmx_maxlen"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_profile.recv_cmd_maxlen()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "no_redundant_have"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_profile.no_redundant_have()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "no_new_full_limit"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_profile.no_new_full_limit()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "no_new_itor_limit"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_profile.no_new_itor_limit()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	// return the just built html
	return oss.str();
}


NEOIP_NAMESPACE_END


