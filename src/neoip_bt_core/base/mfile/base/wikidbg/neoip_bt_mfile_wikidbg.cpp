/*! \file
    \brief Declaration of the bt_mfile_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_mfile_wikidbg.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_mfile_http_t defines the wikidbg stuff for \ref bt_mfile_t
 */
class bt_mfile_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
	static httpd_err_t	piecehash_arr(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_mfile_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_mfile_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_mfile_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_mfile_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_mfile_wikidbg_t::page);

	keyword_db.insert_page("piecehash_arr"	, bt_mfile_wikidbg_t::piecehash_arr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_mfile_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_mfile_t *		bt_mfile	= (bt_mfile_t *)object_ptr;
	std::ostringstream	oss;

	oss << bt_mfile->name();

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
std::string bt_mfile_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_mfile_t *		bt_mfile	= (bt_mfile_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", bt_mfile), wikidbg_html("page_title_attr", bt_mfile));
	oss << wikidbg_html("oneword", bt_mfile);
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
std::string bt_mfile_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "Click for more info";
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
httpd_err_t bt_mfile_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_mfile_t *		bt_mfile	= (bt_mfile_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_mfile_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "announce_uri"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_mfile->announce_uri()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "infohash"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_mfile->infohash()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "name"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_mfile->name()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "piecelen"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(bt_mfile->piecelen())
						<< "(" << bt_mfile->piecelen() << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "totfile_size"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(bt_mfile->totfile_size().to_uint64())
						<< "(" << bt_mfile->totfile_size() << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_piece"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_mfile->nb_piece()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "piecehash_arr"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << h.s_link(wikidbg_url("piecehash_arr", bt_mfile))
					<< bt_mfile->piecehash_arr().size() << " pieces" << h.e_link()
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	// display the table of all the current bt_swarm_t
	oss << h.s_sub1title() << "List of bt_mfile_subfile_t: "
					<< bt_mfile->subfile_arr().size() << h.e_sub1title();
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "local path"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "mfile path"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "length"			<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "totfile_beg"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "totfile_end"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "percent"			<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	// go thru the whole bt_session_t::swarm_db
	bt_mfile_subfile_arr_t &	subfile_arr = bt_mfile->subfile_arr();
	for( size_t i = 0; i < subfile_arr.size(); i++ ){
		bt_mfile_subfile_t &	mfile_subfile	= subfile_arr[i];
		// produce the html
		oss << h.s_tr();
		oss << h.s_td() << mfile_subfile.local_path()	<< h.e_td();
		oss << h.s_td() << mfile_subfile.mfile_path()	<< h.e_td();
		oss << h.s_td() << string_t::size_string(mfile_subfile.len().to_uint64())
				<< h.e_td();
		oss << h.s_td() << string_t::size_string(mfile_subfile.totfile_beg().to_uint64())
				<< h.e_td();
		oss << h.s_td() << string_t::size_string(mfile_subfile.totfile_end().to_uint64())
				<< h.e_td();
		oss << h.s_td() << string_t::percent_string(mfile_subfile.len().to_double() / bt_mfile->totfile_size().to_double())
				<< h.e_td();
		
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

/** \brief Handle the "piecehash_arr" keyword
 */
httpd_err_t bt_mfile_wikidbg_t::piecehash_arr(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_mfile_t *		bt_mfile	= (bt_mfile_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_mfile_t PieceHash Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_mfile"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_mfile)
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	// display the table of all the current bt_swarm_t
	oss << h.s_sub1title() << "List of bt_id_arr_t: "
					<< bt_mfile->piecehash_arr().size() << h.e_sub1title();
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "index"			<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "piece hash + 0"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "piece hash + 1"		<< h.e_b() << h.e_th();
	oss << h.e_tr();	
	// go thru the whole bt_session_t::swarm_db
	bt_id_arr_t &	piecehash_arr = bt_mfile->piecehash_arr();
	for( size_t i = 0; i < piecehash_arr.size(); i+=2 ){
		// produce the html
		oss << h.s_tr();
		oss << h.s_td() << i			<< h.e_td();
		if( i < piecehash_arr.size() )	oss << h.s_td() << piecehash_arr[i]	<< h.e_td();
		if( i+1 < piecehash_arr.size())	oss << h.s_td() << piecehash_arr[i+1]	<< h.e_td();
		oss << h.e_tr();
	}
	oss << h.e_table();
		
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







