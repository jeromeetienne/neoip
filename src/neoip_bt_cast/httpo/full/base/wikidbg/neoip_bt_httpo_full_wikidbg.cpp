/*! \file
    \brief Declaration of the bt_httpo_full_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_httpo_full_wikidbg.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_resp.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_httpo_full_t
 */
class bt_httpo_full_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_httpo_full_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_httpo_full_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_httpo_full_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_httpo_full_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_httpo_full_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_httpo_full_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_httpo_full_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_httpo_full_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "a httpo_full_t";

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
std::string bt_httpo_full_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_httpo_full_t *	httpo_full	= (bt_httpo_full_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", httpo_full), wikidbg_html("page_title_attr", httpo_full));
	oss << wikidbg_html("oneword", httpo_full);
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
std::string bt_httpo_full_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "bt_httpo_full_t";
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
httpd_err_t bt_httpo_full_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_httpo_full_t *	httpo_full	= (bt_httpo_full_t *)object_ptr;
	const bt_mfile_t &	bt_mfile	= httpo_full->bt_swarm->get_mfile();
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	
	// put the title
	oss << h.pagetitle("bt_httpo_full_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "httpo_resp"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", httpo_full->httpo_resp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", httpo_full->bt_swarm)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_reqhd"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << httpo_full->http_reqhd()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "range_tosend"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << httpo_full->range_tosend().to_human_string()
						<< " ***** (" << httpo_full->range_tosend() << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "piecedel_in_dtor"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << (httpo_full->piecedel_in_dtor ? "YES":"NO")
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "piecedel_as_delivered"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << (httpo_full->piecedel_as_delivered ? "YES":"NO")
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "piecedel_pre_newlyavail"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << (httpo_full->piecedel_pre_newlyavail ? "YES":"NO")
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "slide_curs"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", httpo_full->slide_curs())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "current_pos"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << httpo_full->current_pos().to_human_string()
						<< " ***** (" << httpo_full->current_pos() << ")"
						<< " (in pieceidx "
						<< bt_unit_t::totfile_to_pieceidx(httpo_full->current_pos(), bt_mfile)
						<< ")"	
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "sent_length"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << httpo_full->sent_length().to_human_string()
						<< " ***** (" << httpo_full->sent_length() << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "socket_full"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", httpo_full->socket_full)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "xmit_rsched"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", httpo_full->m_xmit_rsched)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "is_started"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << httpo_full->is_started()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_io_read"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", httpo_full->bt_io_read)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(httpo_full->callback)
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
std::string bt_httpo_full_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "httpo_full"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "current position"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "begining position"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "range length"		<< h.e_b() << h.e_th();
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
std::string bt_httpo_full_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_httpo_full_t *	httpo_full	= (bt_httpo_full_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", httpo_full)		<< h.e_td();
	oss << h.s_td() << httpo_full->current_pos().to_human_string()		<< h.e_td();
	oss << h.s_td() << httpo_full->range_tosend().beg().to_human_string()	<< h.e_td();
	oss << h.s_td() << httpo_full->range_tosend().length().to_human_string()<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}
NEOIP_NAMESPACE_END







