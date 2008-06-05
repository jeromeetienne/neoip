/*! \file
    \brief Declaration of the bt_swarm_sched_block_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_sched_block_wikidbg.hpp"
#include "neoip_bt_swarm_sched_block.hpp"
#include "neoip_bt_swarm_sched_request.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_swarm_sched_block_t
 */
class bt_swarm_sched_block_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_swarm_sched_block_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_swarm_sched_block_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_swarm_sched_block_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_swarm_sched_block_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_swarm_sched_block_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_swarm_sched_block_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_swarm_sched_block_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_swarm_sched_block_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_sched_block_t *sched_block	= (bt_swarm_sched_block_t *)object_ptr;
	std::ostringstream	oss;

	oss << sched_block->totfile_range();

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
std::string bt_swarm_sched_block_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_sched_block_t *sched_block	= (bt_swarm_sched_block_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", sched_block), wikidbg_html("page_title_attr", sched_block));
	oss << wikidbg_html("oneword", sched_block);
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
std::string bt_swarm_sched_block_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "sched_blocky established connection";
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
httpd_err_t bt_swarm_sched_block_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_sched_block_t *sched_block	= (bt_swarm_sched_block_t *)object_ptr;
	bt_swarm_sched_piece_t *sched_piece	= sched_block->sched_piece;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	
	// put the title
	oss << h.pagetitle("bt_swarm_sched_block_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm_sched_piece"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", sched_piece)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "totfile_range"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << sched_block->totfile_range()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();	

	
	// display the table of all the current bt_swarm_sched_request_t
	bt_swarm_sched_block_t::request_db_t &		pending_req_db = sched_block->pending_req_db;
	bt_swarm_sched_block_t::request_db_t::iterator	iter;
	oss << h.s_sub1title() << "List of all pending bt_swarm_sched_request_t: "
				<< pending_req_db.size()
				<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_sched_t::pending_req_db
	for( iter = pending_req_db.begin(); iter != pending_req_db.end(); iter++ ){
		bt_swarm_sched_request_t *	sched_request	= *iter;
		// if it is the first element, add the tableheader
		if( iter == pending_req_db.begin() )	oss << wikidbg_html("tableheader", sched_request);	
		// display this row
		oss << wikidbg_html("tablerow", sched_request);
	}
	oss << h.e_table();

	// display the table of all the current bt_swarm_sched_request_t
	bt_swarm_sched_block_t::request_db_t &		expired_req_db = sched_block->expired_req_db;
	oss << h.s_sub1title() << "List of all expired bt_swarm_sched_request_t: "
				<< expired_req_db.size()
				<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_sched_t::expired_req_db
	for( iter = expired_req_db.begin(); iter != expired_req_db.end(); iter++ ){
		bt_swarm_sched_request_t *	sched_request	= *iter;
		// if it is the first element, add the tableheader
		if( iter == expired_req_db.begin() )	oss << wikidbg_html("tableheader", sched_request);	
		// display this row
		oss << wikidbg_html("tablerow", sched_request);
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
std::string bt_swarm_sched_block_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "block_idx"			<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "block_len"			<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb pending request"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb expired request"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "connection of first request"	<< h.e_b() << h.e_th();
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
std::string bt_swarm_sched_block_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_sched_block_t *sched_block	= (bt_swarm_sched_block_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", sched_block)		<< h.e_td();
	oss << h.s_td() << sched_block->length()				<< h.e_td();
	oss << h.s_td() << sched_block->pending_req_db.size()			<< h.e_td();
	oss << h.s_td() << sched_block->expired_req_db.size()			<< h.e_td();
	if( sched_block->pending_req_db.empty() == false ){
		bt_swarm_sched_request_t *	first_request	= *sched_block->pending_req_db.begin();
		bt_swarm_sched_cnx_vapi_t *	sched_cnx	= first_request->get_sched_cnx();
		oss << h.s_td() << wikidbg_html("oneword_pageurl", sched_cnx)	<< h.e_td();
	}else{
		oss << h.s_td() << "none"					<< h.e_td();
	}
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







