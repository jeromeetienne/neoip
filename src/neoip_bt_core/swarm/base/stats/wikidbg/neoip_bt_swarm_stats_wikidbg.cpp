/*! \file
    \brief Declaration of the bt_swarm_stats_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_stats_wikidbg.hpp"
#include "neoip_bt_swarm_stats.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_swarm_stats_t
 */
class bt_swarm_stats_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static std::string	menu_item(const std::string &keyword, void *object_ptr)		throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	page_info(const std::string &keyword, void *object_ptr)		throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_swarm_stats_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_swarm_stats_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_swarm_stats_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_swarm_stats_wikidbg_t::page_title_attr);
	keyword_db.insert_html("menu_item"	, bt_swarm_stats_wikidbg_t::menu_item);
	keyword_db.insert_page("page"		, bt_swarm_stats_wikidbg_t::page);

	keyword_db.insert_html("page_info"	, bt_swarm_stats_wikidbg_t::page_info);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_swarm_stats_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "swarm stats";

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
std::string bt_swarm_stats_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_stats_t *	swarm_stats	= (bt_swarm_stats_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", swarm_stats), wikidbg_html("page_title_attr", swarm_stats));
	oss << wikidbg_html("oneword", swarm_stats);
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
std::string bt_swarm_stats_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "stats";
	oss << "\"";
	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu_item" keyword
 */
std::string bt_swarm_stats_wikidbg_t::menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_stats_t *	swarm_stats	= (bt_swarm_stats_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("page", swarm_stats), wikidbg_html("page_title_attr", swarm_stats));
	oss << wikidbg_html("oneword", swarm_stats);
	oss << h.e_link();

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
httpd_err_t bt_swarm_stats_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_stats_t *	swarm_stats	= (bt_swarm_stats_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	// put the title
	oss << h.pagetitle("bt_swarm_stats_t Page");
	
	// display the page_info
	oss << wikidbg_html("page_info", swarm_stats);
	
	// return no error
	return httpd_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "page_info" keyword
 */
std::string bt_swarm_stats_wikidbg_t::page_info(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_stats_t *	swarm_stats	= (bt_swarm_stats_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	// build the html
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "uloaded_datalen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(swarm_stats->uloaded_datalen().to_uint64())
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "uloaded / dloaded_datalen"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::ratio_string(swarm_stats->share_ratio())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "dloaded_datalen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(swarm_stats->dloaded_datalen().to_uint64())
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "deleted_dload_datalen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(swarm_stats->deleted_dload_datalen().to_uint64())
					<< " (" << swarm_stats->deleted_dload_datalen() << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "uloaded_fulllen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(swarm_stats->uloaded_fulllen().to_uint64())
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "dloaded_fulllen"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(swarm_stats->dloaded_fulllen().to_uint64())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "uloaded_overhd"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(swarm_stats->uloaded_overhd().to_uint64())
					<< " (" << string_t::ratio_string(swarm_stats->uloaded_overhd_ratio()) << ")" 
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "dloaded_overhd"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(swarm_stats->dloaded_overhd().to_uint64())
					<< " (" << string_t::ratio_string(swarm_stats->dloaded_overhd_ratio()) << ")" 
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_hash_failed"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_stats->nb_hash_failed()
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "timedout_req_nb"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_stats->timedout_req_nb()
					<< "(" << string_t::percent_string( (double)swarm_stats->timedout_req_nb() / (double)swarm_stats->total_req_nb() ) << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "req_total_nb"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_stats->total_req_nb()
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "req_total_len"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(swarm_stats->total_req_len().to_uint64())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "dup_req_nb"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_stats->dup_req_nb()
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "dup_req_len"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(swarm_stats->dup_req_len().to_uint64())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "dup_rep_nb"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_stats->dup_rep_nb()
					<< h.e_td();
			oss << h.s_td() << h.s_b() << "dup_rep_len"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(swarm_stats->dup_rep_len().to_uint64())
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







