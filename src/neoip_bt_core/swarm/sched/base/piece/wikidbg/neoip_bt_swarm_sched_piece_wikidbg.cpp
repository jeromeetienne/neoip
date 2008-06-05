/*! \file
    \brief Declaration of the bt_swarm_sched_piece_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_sched_piece_wikidbg.hpp"
#include "neoip_bt_swarm_sched_piece.hpp"
#include "neoip_bt_swarm_sched_block.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_swarm_sched_piece_t
 */
class bt_swarm_sched_piece_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_swarm_sched_piece_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_swarm_sched_piece_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_swarm_sched_piece_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_swarm_sched_piece_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_swarm_sched_piece_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_swarm_sched_piece_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_swarm_sched_piece_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_swarm_sched_piece_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_sched_piece_t *sched_piece	= (bt_swarm_sched_piece_t *)object_ptr;
	std::ostringstream	oss;

	oss << sched_piece->pieceidx();

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
std::string bt_swarm_sched_piece_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_sched_piece_t *sched_piece	= (bt_swarm_sched_piece_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", sched_piece), wikidbg_html("page_title_attr", sched_piece));
	oss << wikidbg_html("oneword", sched_piece);
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
std::string bt_swarm_sched_piece_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "sched_piecey established connection";
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
httpd_err_t bt_swarm_sched_piece_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request)	throw()
{
	bt_swarm_sched_piece_t *sched_piece	= (bt_swarm_sched_piece_t *)object_ptr;
	bt_swarm_sched_t *	swarm_sched	= sched_piece->swarm_sched();
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	
	// put the title
	oss << h.pagetitle("bt_swarm_sched_piece_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_swarm_sched"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", swarm_sched)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "piece_idx"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << sched_piece->pieceidx()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "isavail_inval"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << sched_piece->isavail_inval
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nevereq_inval"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << sched_piece->nevereq_inval
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();	

	// display the table of all the current bt_swarm_sched_block_t in block_do_needreq_db
	const bt_swarm_sched_piece_t::block_db_t &		block_do_needreq_db = sched_piece->block_do_needreq_db;
	bt_swarm_sched_piece_t::block_db_t::const_iterator	iter;
	oss << h.s_sub1title() << "List of all bt_swarm_sched_block_t DO_NEEDREQ: "
				<< block_do_needreq_db.size()
				<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_sched_t::block_do_needreq_db
	for( iter = block_do_needreq_db.begin(); iter != block_do_needreq_db.end(); iter++ ){
		const bt_swarm_sched_block_t *	sched_block	= iter->second;
		// if it is the first element, add the tableheader
		if( iter == block_do_needreq_db.begin() )	oss << wikidbg_html("tableheader", sched_block);	
		// display this row
		oss << wikidbg_html("tablerow", sched_block);
	}
	oss << h.e_table();

	// display the table of all the current bt_swarm_sched_block_t in block_no_needreq_db
	const bt_swarm_sched_piece_t::block_db_t &	block_no_needreq_db = sched_piece->block_no_needreq_db;
	oss << h.s_sub1title() << "List of all bt_swarm_sched_block_t NO_NEEDREQ: "
				<< block_no_needreq_db.size()
				<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_swarm_sched_t::block_no_needreq_db
	for( iter = block_no_needreq_db.begin(); iter != block_no_needreq_db.end(); iter++ ){
		const bt_swarm_sched_block_t *	sched_block	= iter->second;
		// if it is the first element, add the tableheader
		if( iter == block_no_needreq_db.begin() )	oss << wikidbg_html("tableheader", sched_block);	
		// display this row
		oss << wikidbg_html("tablerow", sched_block);
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
std::string bt_swarm_sched_piece_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "piece_idx"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "block requested"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "block to request"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "hash checking"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "partavail_len"		<< h.e_b() << h.e_th();
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
std::string bt_swarm_sched_piece_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_sched_piece_t *sched_piece	= (bt_swarm_sched_piece_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", sched_piece)		<< h.e_td();
	oss << h.s_td() << sched_piece->block_no_needreq_db.size()		<< h.e_td();
	oss << h.s_td() << sched_piece->block_do_needreq_db.size()		<< h.e_td();
	oss << h.s_td() << (sched_piece->piece_cpuhash ? "Checking" : "no")	<< h.e_td();
	oss << h.s_td() << sched_piece->partavail_len().to_human_string()	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







