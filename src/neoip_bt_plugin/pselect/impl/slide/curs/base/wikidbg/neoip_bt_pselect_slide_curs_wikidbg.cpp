/*! \file
    \brief Declaration of the bt_pselect_slide_curs_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_pselect_slide_curs_wikidbg.hpp"
#include "neoip_bt_pselect_slide_curs.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_herr_http_t defines the wikidbg stuff for \ref bt_pselect_slide_curs_t
 */
class bt_pselect_slide_curs_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_pselect_slide_curs_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_pselect_slide_curs_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_pselect_slide_curs_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_pselect_slide_curs_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_pselect_slide_curs_wikidbg_t::page);


	keyword_db.insert_html("tableheader"	, bt_pselect_slide_curs_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_pselect_slide_curs_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_pselect_slide_curs_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bt_pselect_slide_curs_t";

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
std::string bt_pselect_slide_curs_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_pselect_slide_curs_t *slide_curs	= (bt_pselect_slide_curs_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", slide_curs), wikidbg_html("page_title_attr", slide_curs));
	oss << wikidbg_html("oneword", slide_curs);
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
std::string bt_pselect_slide_curs_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_pselect_slide_curs_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_pselect_slide_curs_t *slide_curs	= (bt_pselect_slide_curs_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;


	// put the title
	oss << h.pagetitle("bt_pselect_slide_curs_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_pselect_slide_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", slide_curs->pselect_slide())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "offset"			<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << slide_curs->offset()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "pieceprec_arr size"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << slide_curs->pieceprec_arr().size()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current bt_pselect_slide_curs_t::pieceprec_arr()
	oss << h.s_sub1title() << "List of bt_pieceprec_arr_t: "
					<< slide_curs->pieceprec_arr().size() << h.e_sub1title();
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "index"			<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "prec + 0"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "prec + 1"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "prec + 2"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "prec + 3"		<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	// go thru the whole bt_session_t::swarm_db
	const bt_pieceprec_arr_t &	pieceprec_arr = slide_curs->pieceprec_arr();
	for( size_t i = 0; i < pieceprec_arr.size(); i += 4 ){
		// produce the html
		oss << h.s_tr();
		oss << h.s_td() << i			<< h.e_td();
		if( i+0 < pieceprec_arr.size())	oss << h.s_td() << pieceprec_arr[i+0]	<< h.e_td();
		if( i+1 < pieceprec_arr.size())	oss << h.s_td() << pieceprec_arr[i+1]	<< h.e_td();
		if( i+2 < pieceprec_arr.size())	oss << h.s_td() << pieceprec_arr[i+2]	<< h.e_td();
		if( i+3 < pieceprec_arr.size())	oss << h.s_td() << pieceprec_arr[i+3]	<< h.e_td();
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
std::string bt_pselect_slide_curs_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "key"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "offset"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb_piece"	<< h.e_b() << h.e_th();
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
std::string bt_pselect_slide_curs_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_pselect_slide_curs_t *slide_curs	= (bt_pselect_slide_curs_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", slide_curs)		<< h.e_td();
	oss << h.s_td() << slide_curs->offset()					<< h.e_td();
	oss << h.s_td() << slide_curs->nb_piece()				<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







