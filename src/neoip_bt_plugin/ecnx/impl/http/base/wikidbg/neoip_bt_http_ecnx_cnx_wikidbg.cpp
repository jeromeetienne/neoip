/*! \file
    \brief Declaration of the bt_http_ecnx_cnx_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_http_ecnx_cnx_wikidbg.hpp"
#include "neoip_bt_http_ecnx_cnx.hpp"
#include "neoip_bt_http_ecnx_cnx_stats_helper.hpp"
#include "neoip_bt_http_ecnx_pool.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_http_ecnx_cnx_t
 */
class bt_http_ecnx_cnx_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr
								, httpd_request_t &request)	throw();
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
void	bt_http_ecnx_cnx_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_http_ecnx_cnx_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_http_ecnx_cnx_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_http_ecnx_cnx_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_http_ecnx_cnx_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_http_ecnx_cnx_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_http_ecnx_cnx_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_http_ecnx_cnx_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_cnx_t *	ecnx_cnx	= (bt_http_ecnx_cnx_t *)object_ptr;
	std::ostringstream	oss;

	oss << ecnx_cnx->hostport_str();

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
std::string bt_http_ecnx_cnx_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_cnx_t *	ecnx_cnx	= (bt_http_ecnx_cnx_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", ecnx_cnx), wikidbg_html("page_title_attr", ecnx_cnx));
	oss << wikidbg_html("oneword", ecnx_cnx);
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
std::string bt_http_ecnx_cnx_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "ecnx_cnx established connection";
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
httpd_err_t bt_http_ecnx_cnx_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_http_ecnx_cnx_t *	ecnx_cnx	= (bt_http_ecnx_cnx_t *)object_ptr;
	bt_http_ecnx_pool_t *	ecnx_pool	= ecnx_cnx->ecnx_pool();
	bt_swarm_t *		bt_swarm	= ecnx_pool->get_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();	
	file_size_t		totfile_avail	= bt_unit_t::totfile_avail(ecnx_cnx->remote_pavail(), bt_mfile);
	file_size_t		totfile_size	= bt_mfile.totfile_size();		
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", ecnx_pool);
	// put the title
	oss << h.pagetitle("bt_http_ecnx_cnx_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(ecnx_cnx->callback)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_http_ecnx_iov_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " <<  wikidbg_html("oneword_pageurl", ecnx_cnx->m_ecnx_iov)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "is_cnx_close"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << (ecnx_cnx->m_is_cnx_close
						? "true - every bt_http_ecnx_iov will create a new http cnx"
						: "false - the http connection is persistent and will be reused")
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "totfile_avail"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(totfile_avail.to_uint64())
					<< " (" << string_t::percent_string(totfile_avail.to_double() / totfile_size.to_double()) << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "recv_rate"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string((uint64_t)ecnx_cnx->recv_rate_avg())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "req_queue_maxlen"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(ecnx_cnx->req_queue_maxlen())
					<< h.e_td();
			oss << h.e_tr();	
	oss << h.e_table();
	oss << h.br();	

	// display the bt_http_ecnx_cnx_stats_t
	oss << h.sub1title("bt_http_ecnx_cnx_stats_t :");
	oss << bt_http_ecnx_cnx_stats_helper_t::to_html(ecnx_cnx->cnx_stats());

	// display the table of all the current bt_http_ecnx_req_t
	std::list<bt_http_ecnx_req_t *> &		ecnx_req_fifo = ecnx_cnx->ecnx_req_fifo;
	std::list<bt_http_ecnx_req_t *>::iterator	iter_req;
	oss << h.s_sub1title() << "List of all pending bt_http_ecnx_req_t: "
				<< ecnx_req_fifo.size()
				<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_sched_t::ecnx_req_fifo
	for( iter_req = ecnx_req_fifo.begin(); iter_req != ecnx_req_fifo.end(); iter_req++ ){
		bt_http_ecnx_req_t *	ecnx_req	= *iter_req;
		// if it is the first element, add the tableheader
		if( iter_req == ecnx_req_fifo.begin() )	oss << wikidbg_html("tableheader", ecnx_req);	
		// display this row
		oss << wikidbg_html("tablerow", ecnx_req);
	}
	oss << h.e_table();

	// display the table of all the current bt_http_ecnx_iov_t
	bt_http_ecnx_cnx_t::subfile_uri_arr_t &	subfile_uri_arr = ecnx_cnx->subfile_uri_arr;
	oss << h.s_sub1title() << "List of all subfile_uri http_uri_t: "
				<< subfile_uri_arr.size()
				<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_sched_t::ecnx_iov_db
	for(size_t i = 0; i < subfile_uri_arr.size(); i++){
		const http_uri_t	subfile_uri	= subfile_uri_arr[i];
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "subfile_uri " << i	<< h.e_b() << h.e_td();
			oss << h.s_td() << subfile_uri				<< h.e_td();
			oss << h.e_tr();		
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

/** \brief Handle the "tableheader" keyword
 */
std::string bt_http_ecnx_cnx_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "hostport_str"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb pending request"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "available data"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "recv_rate"		<< h.e_b() << h.e_th();	
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
std::string bt_http_ecnx_cnx_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_http_ecnx_cnx_t *	ecnx_cnx	= (bt_http_ecnx_cnx_t *)object_ptr;
	bt_swarm_t *		bt_swarm	= ecnx_cnx->ecnx_pool()->get_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	file_size_t		totfile_avail	= bt_unit_t::totfile_avail(ecnx_cnx->remote_pavail(), bt_mfile);
	file_size_t		totfile_size	= bt_mfile.totfile_size();	
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", ecnx_cnx)			<< h.e_td();
	oss << h.s_td() << ecnx_cnx->nb_pending_request()				<< h.e_td();
	oss << h.s_td() << string_t::percent_string(totfile_avail.to_double() / totfile_size.to_double())
											<< h.e_td();
	oss << h.s_td() << string_t::size_string((uint64_t)ecnx_cnx->recv_rate_avg())	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







