/*! \file
    \brief Declaration of the bitcount_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bitcount_wikidbg.hpp"
#include "neoip_bitcount.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bitcount_t
 */
class bitcount_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static std::string	menu_item(const std::string &keyword, void *object_ptr)		throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bitcount_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bitcount_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bitcount_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bitcount_wikidbg_t::page_title_attr);
	keyword_db.insert_html("menu_item"	, bitcount_wikidbg_t::menu_item);
	keyword_db.insert_page("page"		, bitcount_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bitcount_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bitcount_t *		bitcount	= (bitcount_t *)object_ptr;	
	std::ostringstream	oss;

	oss << "bitcount (" << bitcount->size() << "-bit";
	oss << " covered at " << bitcount->coverage_factor() <<")";

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
std::string bitcount_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bitcount_t *		bitcount	= (bitcount_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", bitcount), wikidbg_html("page_title_attr", bitcount));
	oss << wikidbg_html("oneword", bitcount);
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
std::string bitcount_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "click for more info";
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
std::string bitcount_wikidbg_t::menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bitcount_t *		bitcount	= (bitcount_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("page", bitcount), wikidbg_html("page_title_attr", bitcount));
	oss << wikidbg_html("oneword", bitcount);
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
httpd_err_t bitcount_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bitcount_t *		bitcount	= (bitcount_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;


	// put the title
	oss << h.pagetitle("bitcount_t Page");


	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_bit"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bitcount->size()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "coverage_factor"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bitcount->coverage_factor()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	// display the table of the rarity per pieceidx
	oss << h.s_sub1title() << "List of " << bitcount->size() << " counter per idx: " << h.e_sub1title();
	size_t	mod_size	= 4;
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "index"		<< h.e_b() << h.e_th();
	for(size_t i = 0; i < mod_size; i++ )
		oss << h.s_th() << h.s_b() << "+ " << i	<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	// go thru the whole bt_session_t::swarm_db
	for( size_t i = 0; i < bitcount->size(); i += mod_size ){
		// produce the html
		oss << h.s_tr();
		oss << h.s_td() << i			<< h.e_td();
		for(size_t j = i; j < i+mod_size && j < bitcount->size(); j++ ){
			oss << h.s_td() << (*bitcount)[j]	<< h.e_td();
		}
		oss << h.e_tr();
	}
	oss << h.e_table();
	
		// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END









