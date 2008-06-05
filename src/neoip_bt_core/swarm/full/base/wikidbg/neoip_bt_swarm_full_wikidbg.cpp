/*! \file
    \brief Declaration of the bt_swarm_full_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_full_wikidbg.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_prec.hpp"
#include "neoip_bt_swarm_full_sched.hpp"
#include "neoip_bt_swarm_full_sendq.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_swarm_full_t
 */
class bt_swarm_full_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	tableheader(const std::string &keyword, void *object_ptr)	throw();
	static std::string	tablerow(const std::string &keyword, void *object_ptr)		throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_swarm_full_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_swarm_full_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_swarm_full_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_swarm_full_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_swarm_full_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_swarm_full_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_swarm_full_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_swarm_full_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_full_t *	swarm_full	= (bt_swarm_full_t *)object_ptr;
	std::ostringstream	oss;

	oss << swarm_full->remote_peerid();

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
std::string bt_swarm_full_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_full_t *	swarm_full	= (bt_swarm_full_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", swarm_full), wikidbg_html("page_title_attr", swarm_full));
	oss << wikidbg_html("oneword", swarm_full);
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
std::string bt_swarm_full_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "fully established connection";
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
httpd_err_t bt_swarm_full_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_full_t *	swarm_full	= (bt_swarm_full_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	bt_swarm_t *		bt_swarm	= swarm_full->bt_swarm;
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	file_size_t		totfile_size	= bt_mfile.totfile_size();
	file_size_t		totfile_avail	= bt_unit_t::totfile_avail(swarm_full->remote_pavail(), bt_mfile);

	// compute the remwish_locavail_pct
	// - TODO find a better name for this number
	bitfield_t		remwish_locavail_field;
	double			remwish_locavail_pct;
	remwish_locavail_field	= swarm_full->remote_pwish() & bt_swarm->local_pavail();
	remwish_locavail_pct	= double(remwish_locavail_field.sum()) / swarm_full->remote_pwish().sum();

	// add the menu
	oss << wikidbg_html("menu", swarm_full->bt_swarm);
	
	// put the title
	oss << h.pagetitle("bt_swarm_full_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", swarm_full->bt_swarm)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "socket_full"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", swarm_full->socket_full)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote peerid"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_full->remote_peerid()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote protoflag"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_full->protoflag()
					<< "(value=0x" << std::hex << swarm_full->protoflag().value() << std::dec << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote program"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << swarm_full->remote_peerid().peerid_progfull();
			if( swarm_full->remote_peerid().peerid_progfull().substr(0,7) == "unknown" )
				oss << h.s_pre() << swarm_full->remote_peerid().to_datum() << h.e_pre();		
			oss << h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote_pavail"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &swarm_full->remote_pavail())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "local req"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << (swarm_full->local_doauth_req()?"DOAUTH":"noauth")
					<< "/"	<< (swarm_full->local_dowant_req()?"DOWANT":"nowant")
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "other req"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << (swarm_full->other_doauth_req()?"DOAUTH":"noauth")
					<< "/"	<< (swarm_full->other_dowant_req()?"DOWANT":"nowant")
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote_pwish"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &swarm_full->remote_pwish())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm_full_prec_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", swarm_full->full_prec())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm_full_sched_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", swarm_full->full_sched())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm_full_sendq_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", swarm_full->sendq)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm_full_utmsg_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", swarm_full->full_utmsg())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "recv_rate"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string((uint64_t)swarm_full->recv_rate_avg())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmit_rate"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string((uint64_t)swarm_full->xmit_rate_avg())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "totfile_avail"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(totfile_avail.to_uint64())
					<< " (" << string_t::percent_string(totfile_avail.to_double() / totfile_size.to_double()) << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remwish_locavail_pct"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::percent_string(remwish_locavail_pct)
					<< " - percent of piece remotly wished which are locally avail"
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

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
std::string bt_swarm_full_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "remote program"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "available data percent"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "local req"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "other req"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "reqauth_type"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "recv_rate"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "xmit_rate"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb queued cmd"		<< h.e_b() << h.e_th();
	
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
std::string bt_swarm_full_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_full_t *	swarm_full	= (bt_swarm_full_t *)object_ptr;
	bt_swarm_full_sched_t *	full_sched	= swarm_full->full_sched();
	bt_swarm_t *		bt_swarm	= swarm_full->bt_swarm;
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();	
	file_size_t		totfile_avail	= bt_unit_t::totfile_avail(swarm_full->remote_pavail(), bt_mfile);
	file_size_t		totfile_size	= bt_mfile.totfile_size();
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << h.s_link(wikidbg_url("page", swarm_full), wikidbg_html("page_title_attr", swarm_full))
				<< swarm_full->remote_peerid().peerid_progfull()
				<< h.e_link() << h.e_td();
	oss << h.s_td() << string_t::percent_string(totfile_avail.to_double() / totfile_size.to_double())
											<< h.e_td();
	oss << h.s_td() << (swarm_full->local_dowant_req()?"DOWANT":"nowant")		<< "/"
			<< (swarm_full->local_doauth_req()?"DOAUTH":"noauth")		<< h.e_td();
	oss << h.s_td() << (swarm_full->other_dowant_req()?"DOWANT":"nowant")		<< "/"
			<< (swarm_full->other_doauth_req()?"DOAUTH":"noauth")		<< h.e_td();
	oss << h.s_td() << full_sched->reqauth_type()					<< h.e_td();
	oss << h.s_td() << string_t::size_string((uint64_t)swarm_full->recv_rate_avg())	<< h.e_td();
	oss << h.s_td() << string_t::size_string((uint64_t)swarm_full->xmit_rate_avg())	<< h.e_td();
	oss << h.s_td() << swarm_full->sendq->nb_queued_cmd()				<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







