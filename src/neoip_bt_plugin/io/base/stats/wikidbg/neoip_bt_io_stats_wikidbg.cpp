/*! \file
    \brief Declaration of the bt_io_stats_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_io_stats_wikidbg.hpp"
#include "neoip_bt_io_stats.hpp"
#include "neoip_string.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_io_stats_http_t defines the wikidbg stuff for \ref bt_io_stats_t
 */
class bt_io_stats_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
	static std::string	page_info(const std::string &keyword, void *object_ptr)		throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	bt_io_stats_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_io_stats_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_io_stats_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_io_stats_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_io_stats_wikidbg_t::page);

	keyword_db.insert_html("page_info"	, bt_io_stats_wikidbg_t::page_info);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_io_stats_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bt_io_stats_t";

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
std::string bt_io_stats_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_io_stats_t *		io_stats	= (bt_io_stats_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", io_stats), wikidbg_html("page_title_attr", io_stats));
	oss << wikidbg_html("oneword", io_stats);
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
std::string bt_io_stats_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_io_stats_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_io_stats_t *		io_stats	= (bt_io_stats_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_io_stats_t Page");
	
	oss << h.br();
	oss << wikidbg_html("page_info", io_stats);
	oss << h.br();

	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "page_info" keyword
 */
std::string bt_io_stats_wikidbg_t::page_info(const std::string &keyword, void *object_ptr) throw()
{
	bt_io_stats_t *		io_stats	= (bt_io_stats_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	double	avglen_read	= io_stats->totlen_read().to_double()  / io_stats->nb_read();
	double	avglen_write	= io_stats->totlen_write().to_double() / io_stats->nb_write();

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_read"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << io_stats->nb_read()		<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "totlen_read"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(io_stats->totlen_read().to_uint64())
					<< "("  << io_stats->totlen_read()	<<")"<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "avglen_read"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << avglen_read			<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_write"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << io_stats->nb_write()		<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "totlen_write"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(io_stats->totlen_write().to_uint64())
					<< "("  << io_stats->totlen_write()	<<")"<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "avglen_write"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << avglen_write			<< h.e_td();
			oss << h.e_tr();

		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_read_beg_align16k"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << io_stats->nb_read_beg_align16k()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_read_len_align16k"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << io_stats->nb_read_len_align16k()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_write_beg_align16k"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << io_stats->nb_write_beg_align16k()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nb_write_len_align16k"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << io_stats->nb_write_len_align16k()
					<< h.e_td();
			oss << h.e_tr();

	oss << h.e_table();	
	oss << h.br();

	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END







