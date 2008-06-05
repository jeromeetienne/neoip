/*! \file
    \brief Declaration of the bitfield_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bitfield_wikidbg.hpp"
#include "neoip_bitfield.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bitfield_t
 */
class bitfield_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bitfield_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bitfield_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bitfield_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bitfield_wikidbg_t::page_title_attr);
	keyword_db.insert_html("menu_item"	, bitfield_wikidbg_t::menu_item);
	keyword_db.insert_page("page"		, bitfield_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bitfield_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bitfield_t *		bitfield	= (bitfield_t *)object_ptr;	
	std::ostringstream	oss;

	oss << "bitfield (" << bitfield->size() << "-bit";
	oss << " covered at " << string_t::percent_string(bitfield->coverage_factor())<<")";

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
std::string bitfield_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bitfield_t *	bitfield	= (bitfield_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", bitfield), wikidbg_html("page_title_attr", bitfield));
	oss << wikidbg_html("oneword", bitfield);
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
std::string bitfield_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
std::string bitfield_wikidbg_t::menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bitfield_t *		bitfield	= (bitfield_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("page", bitfield), wikidbg_html("page_title_attr", bitfield));
	oss << wikidbg_html("oneword", bitfield);
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
httpd_err_t bitfield_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bitfield_t *		bitfield	= (bitfield_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bitfield_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_bit"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bitfield->size()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "sum"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bitfield->sum()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "coverage_factor"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::percent_string(bitfield->coverage_factor())
					<< "(" << bitfield->coverage_factor() << ")"
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();


	// display the table of the rarity per pieceidx
	oss << h.s_sub1title() << "List of " << bitfield->size() << " value per idx: " << h.e_sub1title();
	size_t	mod_size	= 4;
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "index"		<< h.e_b() << h.e_th();
	for(size_t i = 0; i < mod_size; i++ )
		oss << h.s_th() << h.s_b() << "+ " << i	<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	// go thru the whole bt_session_t::swarm_db
	for( size_t i = 0; i < bitfield->size(); i += mod_size ){
		// produce the html
		oss << h.s_tr();
		oss << h.s_td() << i			<< h.e_td();
		for(size_t j = i; j < i+mod_size; j++ ){
			if( j >= bitfield->size())	continue;
			oss << h.s_td() << (bitfield->get(j) ? "YES" : "no") << h.e_td();
		}
		oss << h.e_tr();
	}
	oss << h.e_table();
	
		// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







