/*! \file
    \brief Declaration of the bt_io_cache_pool_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_io_cache_pool_wikidbg.hpp"
#include "neoip_bt_io_cache_pool.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_io_cache_pool_t
 */
class bt_io_cache_pool_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr
								, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_io_cache_pool_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_io_cache_pool_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_io_cache_pool_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_io_cache_pool_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_io_cache_pool_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_io_cache_pool_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bt_io_cache_pool_t";

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
std::string bt_io_cache_pool_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_io_cache_pool_t *	cache_pool	= (bt_io_cache_pool_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", cache_pool), wikidbg_html("page_title_attr", cache_pool));
	oss << wikidbg_html("oneword", cache_pool);
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
std::string bt_io_cache_pool_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_io_cache_pool_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_io_cache_pool_t *	cache_pool	= (bt_io_cache_pool_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_io_cache_pool_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "pool_curlen"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << cache_pool->pool_curlen().to_human_string()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();

	// display the table of all the current bt_io_cache_t
	std::list<bt_io_cache_t *> &		cache_db = cache_pool->cache_db;
	std::list<bt_io_cache_t *>::iterator	cache_iter;
	oss << h.s_sub1title() << "List of all pending bt_io_cache_t: "
				<< cache_db.size()	<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_http_io_cache_t::cnx_db
	for( cache_iter = cache_db.begin(); cache_iter != cache_db.end(); cache_iter++ ){
		bt_io_cache_t *	io_cache	= *cache_iter;
		// if it is the first element, add the tableheader
		if( cache_iter == cache_db.begin() )	oss << wikidbg_html("tableheader", io_cache);	
		// display this row
		oss << wikidbg_html("tablerow", io_cache);
	}
	oss << h.e_table();
	
	// display the table of all the current bt_io_cache_block_t
	mlist_head_t<bt_io_cache_block_t> &	block_lru = cache_pool->block_lru;
	mlist_iter_t<bt_io_cache_block_t>	block_iter;
	oss << h.s_sub1title() << "List of all pending bt_io_cache_block_t block_lru: "
				<< block_lru.size()	<< h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_http_io_cache_t::cnx_db
	for( block_iter = block_lru.begin(); block_iter != block_lru.end(); block_iter++ ){
		bt_io_cache_block_t *	cache_block	= block_iter.object();
		// if it is the first element, add the tableheader
		if( block_iter == block_lru.begin() )	oss << wikidbg_html("tableheader", cache_block);	
		// display this row
		oss << wikidbg_html("tablerow", cache_block);
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







