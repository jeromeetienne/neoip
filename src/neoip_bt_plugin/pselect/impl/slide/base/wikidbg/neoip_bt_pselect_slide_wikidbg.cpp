/*! \file
    \brief Declaration of the bt_pselect_slide_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_pselect_slide_wikidbg.hpp"
#include "neoip_bt_pselect_slide.hpp"
#include "neoip_bt_pselect_slide_curs.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_herr_http_t defines the wikidbg stuff for \ref bt_pselect_slide_t
 */
class bt_pselect_slide_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_pselect_slide_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_pselect_slide_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_pselect_slide_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_pselect_slide_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_pselect_slide_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_pselect_slide_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bt_pselect_slide_t";

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
std::string bt_pselect_slide_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_pselect_slide_t *	bt_pselect_slide	= (bt_pselect_slide_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", bt_pselect_slide), wikidbg_html("page_title_attr", bt_pselect_slide));
	oss << wikidbg_html("oneword", bt_pselect_slide);
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
std::string bt_pselect_slide_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_pselect_slide_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_pselect_slide_t *	pselect_slide= (bt_pselect_slide_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_pselect_slide_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote_pfreq"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &pselect_slide->remote_pfreq())
					<< h.e_td();
			oss << h.e_tr();	
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "remote_pwish"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &pselect_slide->remote_pwish())
					<< h.e_td();
			oss << h.e_tr();	
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "local_pdling"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &pselect_slide->m_local_pdling)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "local_pavail"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &pselect_slide->local_pavail())
					<< h.e_td();
			oss << h.e_tr();	
	oss << h.e_table();	
	oss << h.br();

	// display the table of all the current bt_pselect_slide_curs_t
	oss << h.s_sub1title() << "List of all current bt_pselect_slide_curs_t: "
					<< pselect_slide->curs_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole bt_oload_t::httpw_full_db
	std::list<bt_pselect_slide_curs_t *> &		curs_db = pselect_slide->curs_db;
	std::list<bt_pselect_slide_curs_t *>::iterator	iter;
	for(iter = curs_db.begin(); iter != curs_db.end(); iter++){
		bt_pselect_slide_curs_t *	slide_curs	= *iter;
		// if it is the first element, add the tableheader
		if( iter == curs_db.begin() )	oss << wikidbg_html("tableheader", slide_curs);	
		// display this row
		oss << wikidbg_html("tablerow", slide_curs);
	}
	oss << h.e_table();
	
	// display the table of all the current bt_pselect_slide_curs_t::pieceprec_arr()
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
	for( size_t i = 0; i < pselect_slide->nb_piece(); i += mod_size ){
		// produce the html
		oss << h.s_tr();
		oss << h.s_td() << i			<< h.e_td();
		for(size_t j = i; j < i+mod_size; j++ ){
			if( j >= pselect_slide->size())	continue;
			oss << h.s_td();
			oss << "/" << (pselect_slide->isavail(j) ? "A" : "a");
			oss << "/" << (pselect_slide->is_dling(j) ? "D" : "d");
			oss << "/ra=" << pselect_slide->remote_pfreq(j);
			oss << "/rw=" << pselect_slide->remote_pwish(j);
			oss << "/p=" << pselect_slide->pieceprec(j);
			oss << h.e_td();
		}
		oss << h.e_tr();
	}
	oss << h.e_table();
	
	// display the table of all the current bt_pselect_slide_curs_t::pieceprec_arr()
	oss << h.s_sub1title() << "List of pieceprec and is_ignored per pieceidx: "
							 << h.e_sub1title();
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "precedence"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "slide_curs"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "curs_idx"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "piece_idx"	<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	// go thru the whole bt_pselect_slide_t::prec_db_t
	bt_pselect_slide_t::prec_db_t::reverse_iterator	iter_prec;
	for(iter_prec = pselect_slide->prec_db.rbegin(); iter_prec != pselect_slide->prec_db.rend(); iter_prec++){
		bt_pselect_slide_t::piece_db_t 	piece_db	= iter_prec->second;
		size_t					prec		= iter_prec->first;			
		for(size_t i = 0; i < piece_db.size(); i ++){
			bt_pselect_slide_curs_t *	slide_curs	= piece_db[i].first;
			size_t				curs_idx	= piece_db[i].second;
			oss << h.s_tr();
			if( i == 0 )	oss << h.s_td() << prec			<< h.e_td();
			else		oss << h.s_td() << h.e_td();
			oss << h.s_td() << wikidbg_html("oneword_pageurl", slide_curs)	<< h.e_td();
			oss << h.s_td() << curs_idx					<< h.e_td();
			oss << h.s_td() << slide_curs->cursidx_to_pieceidx(curs_idx)	<< h.e_td();
			oss << h.e_tr();
		}
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







