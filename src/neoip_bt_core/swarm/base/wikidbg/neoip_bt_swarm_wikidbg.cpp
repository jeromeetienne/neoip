/*! \file
    \brief Declaration of the bt_swarm_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_wikidbg.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_sched.hpp"
#include "neoip_bt_swarm_itor.hpp"
#include "neoip_bt_swarm_helper.hpp"
#include "neoip_bt_swarm_profile_helper.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_bt_ecnx_vapi.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_swarm_t
 */
class bt_swarm_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	menu(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	tableheader(const std::string &keyword, void *object_ptr)	throw();
	static std::string	tablerow(const std::string &keyword, void *object_ptr)		throw();
	
	static std::string	full_oneword(const std::string &keyword, void *object_ptr)	throw();
	static std::string	full_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	full_menu_item(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	full_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	ecnx_oneword(const std::string &keyword, void *object_ptr)	throw();
	static std::string	ecnx_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	ecnx_menu_item(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	ecnx_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	rate_per_reqauth_type(const std::string &keyword, void *object_ptr)throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_swarm_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("menu"		, bt_swarm_wikidbg_t::menu);
	keyword_db.insert_html("oneword"	, bt_swarm_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_swarm_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_swarm_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_swarm_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_swarm_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_swarm_wikidbg_t::tablerow);

	keyword_db.insert_html("full_oneword"		, bt_swarm_wikidbg_t::full_oneword);
	keyword_db.insert_html("full_page_title_attr"	, bt_swarm_wikidbg_t::full_page_title_attr);
	keyword_db.insert_html("full_menu_item"		, bt_swarm_wikidbg_t::full_menu_item);		
	keyword_db.insert_page("full_page"		, bt_swarm_wikidbg_t::full_page);

	keyword_db.insert_html("ecnx_oneword"		, bt_swarm_wikidbg_t::ecnx_oneword);
	keyword_db.insert_html("ecnx_page_title_attr"	, bt_swarm_wikidbg_t::ecnx_page_title_attr);
	keyword_db.insert_html("ecnx_menu_item"		, bt_swarm_wikidbg_t::ecnx_menu_item);		
	keyword_db.insert_page("ecnx_page"		, bt_swarm_wikidbg_t::ecnx_page);

	keyword_db.insert_html("rate_per_reqauth_type"	, bt_swarm_wikidbg_t::rate_per_reqauth_type);
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu" keyword
 */
std::string bt_swarm_wikidbg_t::menu(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// build displayed html itself
	oss << h.s_link(wikidbg_url("page", bt_swarm), "title=\"Main bt_swarm_t page\"")
					<< h.b("bt_swarm_t:") << h.e_link();
	oss << " ";
	oss << wikidbg_html("menu_item", bt_swarm->swarm_peersrc());
	oss << " / ";
	oss << wikidbg_html("menu_item", bt_swarm->swarm_sched());
	oss << " / ";
	oss << wikidbg_html("itor_menu_item", bt_swarm->swarm_peersrc());
	oss << " / ";
	oss << wikidbg_html("full_menu_item", bt_swarm);
	oss << " / ";
	oss << wikidbg_html("ecnx_menu_item", bt_swarm);

	oss << h.br();

	// return the built string
	return oss.str();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_swarm_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	oss;

	oss << bt_swarm->get_mfile().infohash();

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
std::string bt_swarm_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", bt_swarm), wikidbg_html("page_title_attr", bt_swarm));
	oss << wikidbg_html("oneword", bt_swarm);
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
std::string bt_swarm_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_swarm_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	

	// compute the total_recv_rate and total_xmit_rate
	double			total_rrate_full= bt_swarm->recv_rate_full();
	double			total_rrate_ecnx= bt_swarm->recv_rate_ecnx();
	double			total_recv_rate	= total_rrate_full + total_rrate_ecnx;
	double			total_xrate_full= bt_swarm->xmit_rate_full();
	double			total_xrate_ecnx= 0;
	double			total_xmit_rate	= total_xrate_full + total_xrate_ecnx;
	
	// compute the locavail_remwish_avg
	// - TODO find a better name for this number
	bt_pselect_vapi_t *	pselect_vapi		= bt_swarm->pselect_vapi();
	const bt_pieceavail_t &	local_pavail		= bt_swarm->local_pavail();		
	double			locavail_remwish_avg	= 0;;
	for(size_t i = 0; i < local_pavail.size(); i++){
		if( !local_pavail[i] )	continue;
		locavail_remwish_avg	+= pselect_vapi->remote_pwish(i);
	}
	locavail_remwish_avg	/= local_pavail.sum();

	// add the menu
	oss << wikidbg_html("menu", bt_swarm);
		
	// put the title
	oss << h.pagetitle("bt_swarm_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_session"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_swarm->get_session())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "infohash"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_swarm->get_mfile().infohash()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "mfile"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &bt_swarm->get_mfile())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "io_vapi"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_swarm->io_vapi())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "pselect_vapi"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_swarm->pselect_vapi())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_pieceavail"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &bt_swarm->local_pavail())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "swarm_utmsg"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_swarm->swarm_utmsg())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_seed"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_swarm_helper_t::swarm_nb_seed(bt_swarm)	
					<< "(" << bt_swarm_helper_t::peersrc_nb_seed(bt_swarm) << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_leech"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " <<bt_swarm_helper_t::swarm_nb_leech(bt_swarm)	
					<< "(" << bt_swarm_helper_t::peersrc_nb_leech(bt_swarm) << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "locavail_remwish_avg"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << locavail_remwish_avg
					<< " - average of 'how many time local pieces are remotly wished'"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "total_recv_rate"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string((uint64_t)total_recv_rate)
					<< " (full: "	<< string_t::percent_string(total_rrate_full/total_recv_rate)
					<< " ecnx: "	<< string_t::percent_string(total_rrate_ecnx/total_recv_rate)
					<< ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "total_xmit_rate"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string((uint64_t)total_xmit_rate)
					<< " (full: "	<< string_t::percent_string(total_xrate_full/total_xmit_rate)
					<< " ecnx: N/A as readonly"
					<< ")"
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// display the bt_swarm_stats_t
	oss << h.sub1title("bt_swarm_stats_t :");
	oss << wikidbg_html("page_info", &bt_swarm->swarm_stats());

	// display the bt_swarm_profile_t
	// - TODO should be a wikidbg
	oss << h.sub1title("bt_swarm_profile_t :");
	oss << bt_swarm_profile_helper_t::to_html(bt_swarm->profile());

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
std::string bt_swarm_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "infohash"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "name"			<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "completion"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb seeder"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb leecher"		<< h.e_b() << h.e_th();
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
std::string bt_swarm_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	file_size_t		totfile_size	= bt_mfile.totfile_size();
	file_size_t		totfile_avail	= bt_unit_t::totfile_avail(bt_swarm->local_pavail(), bt_mfile);	
	std::ostringstream	oss;
	html_builder_t		h;

	// produce the html
	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", bt_swarm)	<< h.e_td();
	oss << h.s_td() << bt_mfile.name()				<< h.e_td();
	oss << h.s_td() << string_t::percent_string(totfile_avail.to_double() / totfile_size.to_double())
									<< h.e_td();
	oss << h.s_td() << bt_swarm_helper_t::swarm_nb_seed(bt_swarm)	
			<< "(" << bt_swarm_helper_t::peersrc_nb_seed(bt_swarm) << ")"
			<< h.e_td();
	oss << h.s_td() << bt_swarm_helper_t::swarm_nb_leech(bt_swarm)	
			<< "(" << bt_swarm_helper_t::peersrc_nb_leech(bt_swarm) << ")"
			<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             bt_swarm_full_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "full_oneword" keyword
 */
std::string bt_swarm_wikidbg_t::full_oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "(" << bt_swarm->full_db.size() << ")";

	// return the built string
	return oss.str();
}


/** \brief Handle the "full_page_title_attr" keyword
 */
std::string bt_swarm_wikidbg_t::full_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << bt_swarm->full_db.size() << " initiators";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "full_menu_item" keyword
 */
std::string bt_swarm_wikidbg_t::full_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("full_page", bt_swarm), wikidbg_html("full_page_title_attr", bt_swarm));
	oss << "full";
	oss << wikidbg_html("full_oneword", bt_swarm);
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "full_page" keyword
 */
httpd_err_t bt_swarm_wikidbg_t::full_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	double			total_recv_rate	= bt_swarm->recv_rate();
	double			total_xmit_rate	= bt_swarm->xmit_rate();

	size_t	swarm_nb_seed	= bt_swarm_helper_t::swarm_nb_seed(bt_swarm);
	size_t	swarm_nb_leech	= bt_swarm_helper_t::swarm_nb_leech(bt_swarm);
	size_t	swarm_nb_total	= swarm_nb_seed + swarm_nb_leech;
	size_t	peersrc_nb_seed	= bt_swarm_helper_t::peersrc_nb_seed(bt_swarm);
	size_t	peersrc_nb_leech= bt_swarm_helper_t::peersrc_nb_leech(bt_swarm);

	size_t nb_full_local_doauth_req	= bt_swarm_helper_t::nb_full_local_doauth_req(bt_swarm);
	size_t nb_full_local_dowant_req	= bt_swarm_helper_t::nb_full_local_dowant_req(bt_swarm);
	size_t nb_full_other_doauth_req	= bt_swarm_helper_t::nb_full_other_doauth_req(bt_swarm);
	size_t nb_full_other_dowant_req	= bt_swarm_helper_t::nb_full_other_dowant_req(bt_swarm);

	// add the menu
	oss << wikidbg_html("menu", bt_swarm);

	// put the title
	oss << h.pagetitle("bt_swarm_full_t List Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "total_recv_rate"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string((uint64_t)total_recv_rate)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "total_xmit_rate"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string((uint64_t)total_xmit_rate)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_seed"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_nb_seed
					<< " among " << peersrc_nb_seed << " from peersrc"
					<< " (" << string_t::ratio_string((double)swarm_nb_seed/peersrc_nb_seed)<<")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_leech"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_nb_leech
					<< " among " << peersrc_nb_leech << " from peersrc"
					<< " (" << string_t::ratio_string((double)swarm_nb_leech/peersrc_nb_leech)<<")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_full_local_doauth_req"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nb_full_local_doauth_req
					<< " remote peer are authorized to download from local peer"
					<< " among " << swarm_nb_leech << " leeches"
					<< " (" << string_t::percent_string((double)nb_full_local_doauth_req/swarm_nb_leech)<<")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_full_local_dowant_req"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nb_full_local_dowant_req
					<< " remote peers have pieces which are wished by local peer."
					<< " among " << swarm_nb_total << " peers"
					<< " (" << string_t::percent_string((double)nb_full_local_dowant_req/swarm_nb_total)<<")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_full_other_doauth_req"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nb_full_other_doauth_req
					<< " remote peer authorizes the local peer to download from them."
					<< " among " << swarm_nb_total << " peers"
					<< " (" << string_t::percent_string((double)nb_full_other_doauth_req/swarm_nb_total)<<")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_full_other_dowant_req"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nb_full_other_dowant_req
					<< " remote peer wishes to download pieces from local peer."
					<< " among " << swarm_nb_leech	<< " leeches"
					<< " (" << string_t::percent_string((double)nb_full_other_dowant_req/swarm_nb_leech)<<")"
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();	

	// displayt a summary of rate per bt_reqauth_type_t
	oss << wikidbg_html("rate_per_reqauth_type", bt_swarm);
	
	
	// display the table of all the current bt_swarm_full_t
	oss << h.s_sub1title() << "List of all current bt_swarm_full_t: "
					<< bt_swarm->full_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_t::full_db
	std::list<bt_swarm_full_t *> &		full_db = bt_swarm->full_db;
	std::list<bt_swarm_full_t *>::iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		bt_swarm_full_t *	swarm_full	= *iter;
		// if it is the first element, add the tableheader
		if( iter == full_db.begin() )	oss << wikidbg_html("tableheader", swarm_full);	
		// display this row
		oss << wikidbg_html("tablerow", swarm_full);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             bt_ecnx_vapi_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "ecnx_oneword" keyword
 */
std::string bt_swarm_wikidbg_t::ecnx_oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "(" << bt_swarm->ecnx_vapi_db.size() << ")";

	// return the built string
	return oss.str();
}


/** \brief Handle the "ecnx_page_title_attr" keyword
 */
std::string bt_swarm_wikidbg_t::ecnx_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << bt_swarm->ecnx_vapi_db.size() << " initiators";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "ecnx_menu_item" keyword
 */
std::string bt_swarm_wikidbg_t::ecnx_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("ecnx_page", bt_swarm), wikidbg_html("ecnx_page_title_attr", bt_swarm));
	oss << "ecnx";
	oss << wikidbg_html("ecnx_oneword", bt_swarm);
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "ecnx_page" keyword
 */
httpd_err_t bt_swarm_wikidbg_t::ecnx_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	double			total_recv_rate	= bt_swarm->recv_rate();

	// add the menu
	oss << wikidbg_html("menu", bt_swarm);

	// put the title
	oss << h.pagetitle("bt_ecnx_vapi_t List Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "total_recv_rate"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string((uint64_t)total_recv_rate)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();	

	// display the table of all the current bt_ecnx_vapi_t
	oss << h.s_sub1title() << "List of all current bt_ecnx_vapi_t: "
					<< bt_swarm->ecnx_vapi_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_t::ecnx_vapi
	bt_swarm_t::ecnx_vapi_db_t &		ecnx_vapi_db = bt_swarm->ecnx_vapi_db;
	bt_swarm_t::ecnx_vapi_db_t::iterator	iter;
	for( iter = ecnx_vapi_db.begin(); iter != ecnx_vapi_db.end(); iter++ ){
		bt_ecnx_vapi_t *	ecnx_vapi	= iter->second;
		// if it is the first element, add the tableheader
		if( iter == ecnx_vapi_db.begin() )	oss << wikidbg_html("tableheader", ecnx_vapi);	
		// display this row
		oss << wikidbg_html("tablerow", ecnx_vapi);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "rate_per_reqauth_type" keyword
 */
std::string bt_swarm_wikidbg_t::rate_per_reqauth_type(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_t *		bt_swarm	= (bt_swarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_table();
	oss << h.s_tr();
		oss << h.s_th() << h.s_b() << "dir/reqauth" << h.e_b() << h.e_th();
		for(size_t i = bt_reqauth_type_t::NONE+1; i < bt_reqauth_type_t::MAX; i++){
			bt_reqauth_type_t reqauth_type((bt_reqauth_type_t::strtype_enum)i);
			oss << h.s_th() << h.s_b() << reqauth_type << h.e_b() << h.e_th();
		}
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_th() << h.s_b() << "recv rate" << h.e_b() << h.e_th();
		for(size_t i = bt_reqauth_type_t::NONE+1; i < bt_reqauth_type_t::MAX; i++){
			bt_reqauth_type_t 	reqauth_type((bt_reqauth_type_t::strtype_enum)i);
			double			rate;
			rate	= bt_swarm_helper_t::recv_rate_for_reqauth_type(bt_swarm, reqauth_type);
			oss << h.s_th() << string_t::size_string((uint64_t)rate) << h.e_th();
		}
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_th() << h.s_b() << "xmit rate" << h.e_b() << h.e_th();
		for(size_t i = bt_reqauth_type_t::NONE+1; i < bt_reqauth_type_t::MAX; i++){
			bt_reqauth_type_t 	reqauth_type((bt_reqauth_type_t::strtype_enum)i);
			double			rate;
			rate	= bt_swarm_helper_t::xmit_rate_for_reqauth_type(bt_swarm, reqauth_type);
			oss << h.s_th() << string_t::size_string((uint64_t)rate) << h.e_th();
		}
		oss << h.e_tr();	
	oss << h.e_table();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







