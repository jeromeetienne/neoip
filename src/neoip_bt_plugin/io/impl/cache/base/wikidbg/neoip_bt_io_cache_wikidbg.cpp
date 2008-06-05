/*! \file
    \brief Declaration of the bt_io_cache_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_io_cache_wikidbg.hpp"
#include "neoip_bt_io_cache.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_io_cache_t
 */
class bt_io_cache_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_io_cache_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_io_cache_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_io_cache_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_io_cache_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_io_cache_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_io_cache_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_io_cache_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_io_cache_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_io_cache_t *		io_cache	= (bt_io_cache_t *)object_ptr;
	std::ostringstream	oss;

	oss << "bt_io_vapi_t " << io_cache->mode();

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
std::string bt_io_cache_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_io_cache_t *		io_cache	= (bt_io_cache_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", io_cache), wikidbg_html("page_title_attr", io_cache));
	oss << wikidbg_html("oneword", io_cache);
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
std::string bt_io_cache_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "io_cache established connection";
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
httpd_err_t bt_io_cache_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_io_cache_t *		io_cache	= (bt_io_cache_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_io_cache_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_io_cache_pool_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", io_cache->cache_pool())
					<< h.e_td();
			oss << h.e_tr();
			oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "subio_vapi"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", io_cache->subio_vapi())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "io_mode"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << io_cache->mode()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cleaning_timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &io_cache->cleaning_timeout)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "delayed_write_err"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << io_cache->delayed_write_err()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// bt_io_stats_t
	oss << h.s_sub1title() << "bt_io_stats_t:" << h.e_sub1title();
	oss << wikidbg_html("page_info", &io_cache->stats());

	// display the table of all the current bt_io_cache_read_t
	std::list<bt_io_read_t *> &		read_db = io_cache->read_db;
	std::list<bt_io_read_t *>::iterator	read_iter;
	oss << h.s_sub1title() << "List of all pending bt_io_cache_read_t: "
				<< read_db.size()	<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_http_io_cache_t::cnx_db
	for( read_iter = read_db.begin(); read_iter != read_db.end(); read_iter++ ){
		bt_io_read_t *	io_read	= *read_iter;
		// if it is the first element, add the tableheader
		if( read_iter == read_db.begin() )	oss << wikidbg_html("tableheader", io_read);	
		// display this row
		oss << wikidbg_html("tablerow", io_read);
	}
	oss << h.e_table();


	// display the table of all the current bt_io_cache_write_t
	std::list<bt_io_write_t *> &		write_db = io_cache->write_db;
	std::list<bt_io_write_t *>::iterator	write_iter;
	oss << h.s_sub1title() << "List of all pending bt_io_cache_write_t: "
				<< write_db.size()	<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_http_io_cache_t::cnx_db
	for( write_iter = write_db.begin(); write_iter != write_db.end(); write_iter++ ){
		bt_io_write_t *	io_write	= *write_iter;
		// if it is the first element, add the tableheader
		if( write_iter == write_db.begin() )	oss << wikidbg_html("tableheader", io_write);	
		// display this row
		oss << wikidbg_html("tablerow", io_write);
	}
	oss << h.e_table();

	// display the table of all the current bt_io_cache_block_t
	bt_io_cache_t::block_db_t &		block_db = io_cache->block_db;
	bt_io_cache_t::block_db_t::iterator	block_iter;
	oss << h.s_sub1title() << "List of all pending bt_io_cache_block_t block_db: "
				<< block_db.size()	<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_http_io_cache_t::cnx_db
	for( block_iter = block_db.begin(); block_iter != block_db.end(); block_iter++ ){
		bt_io_cache_block_t *	cache_block	= block_iter->second;
		// if it is the first element, add the tableheader
		if( block_iter == block_db.begin() )	oss << wikidbg_html("tableheader", cache_block);	
		// display this row
		oss << wikidbg_html("tablerow", cache_block);
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
std::string bt_io_cache_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "io_cache"		<< h.e_b() << h.e_th();
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
std::string bt_io_cache_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_io_cache_t *		io_cache	= (bt_io_cache_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", io_cache)		<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







