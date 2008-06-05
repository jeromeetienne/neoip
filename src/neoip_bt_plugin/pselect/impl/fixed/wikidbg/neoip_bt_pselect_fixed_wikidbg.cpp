/*! \file
    \brief Declaration of the bt_pselect_fixed_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_pselect_fixed_wikidbg.hpp"
#include "neoip_bt_pselect_fixed.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_herr_http_t defines the wikidbg stuff for \ref bt_pselect_fixed_t
 */
class bt_pselect_fixed_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_pselect_fixed_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_pselect_fixed_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_pselect_fixed_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_pselect_fixed_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_pselect_fixed_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_pselect_fixed_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bt_pselect_fixed_t";

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
std::string bt_pselect_fixed_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_pselect_fixed_t *	bt_pselect_fixed	= (bt_pselect_fixed_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", bt_pselect_fixed), wikidbg_html("page_title_attr", bt_pselect_fixed));
	oss << wikidbg_html("oneword", bt_pselect_fixed);
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
std::string bt_pselect_fixed_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "Click for more info.";
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
httpd_err_t bt_pselect_fixed_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_pselect_fixed_t *	pselect_fixed	= (bt_pselect_fixed_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_pselect_fixed_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote_pfreq"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &pselect_fixed->remote_pfreq())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote_pwish"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &pselect_fixed->remote_pwish())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote_pfreq+pwish coverage"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << (pselect_fixed->remote_pfreq()+pselect_fixed->remote_pwish()).coverage_factor()
					<< h.e_td();
			oss << h.e_tr();
			
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "local_pdling"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &pselect_fixed->local_pdling())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "local_pavail"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &pselect_fixed->local_pavail())
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current bt_pselect_fixed_curs_t::pieceprec_arr()
	oss << h.s_sub1title() << "List of status for each pieceidx: "		<< h.e_sub1title();
	oss << h.br();
	oss << "A for IS locally available/a for NOT locally available" 	<< h.br();
	oss << "D for IS currently downloading/d for NOT currently downloading" << h.br();
	oss << "ra= for counter of remotely available" 				<< h.br();
	oss << "rw= for counter of remotely wished"				<< h.br();
	oss << "p= for local bt_pieceprec_t"					<< h.br();
	size_t	mod_size	= 4;
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "index"		<< h.e_b() << h.e_th();
	for(size_t i = 0; i < mod_size; i++ )
		oss << h.s_th() << h.s_b() << "+ " << i	<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	// go thru the whole bt_session_t::swarm_db
	for( size_t i = 0; i < pselect_fixed->nb_piece(); i += mod_size ){
		// produce the html
		oss << h.s_tr();
		oss << h.s_td() << i			<< h.e_td();
		for(size_t j = i; j < i+mod_size; j++ ){
			if( j >= pselect_fixed->size())	continue;
			oss << h.s_td();
			oss << "/" << (pselect_fixed->isavail(j) ? "A" : "a");
			oss << "/" << (pselect_fixed->is_dling(j) ? "D" : "d");
			oss << "/ra=" << pselect_fixed->remote_pfreq(j);
			oss << "/rw=" << pselect_fixed->remote_pwish(j);
			oss << "/p=" << pselect_fixed->pieceprec(j);
			oss << h.e_td();
		}
		oss << h.e_tr();
	}
	oss << h.e_table();
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







