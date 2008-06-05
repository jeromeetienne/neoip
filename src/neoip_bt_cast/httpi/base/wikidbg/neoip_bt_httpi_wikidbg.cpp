/*! \file
    \brief Declaration of the bt_httpi_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_httpi_wikidbg.hpp"
#include "neoip_bt_httpi.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_httpi_http_t defines the wikidbg stuff for \ref bt_httpi_t
 */
class bt_httpi_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_httpi_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_httpi_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_httpi_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_httpi_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_httpi_wikidbg_t::page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_httpi_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bt_httpi_t";

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
std::string bt_httpi_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_httpi_t *		bt_httpi	= (bt_httpi_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", bt_httpi), wikidbg_html("page_title_attr", bt_httpi));
	oss << wikidbg_html("oneword", bt_httpi);
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
std::string bt_httpi_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_httpi_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_httpi_t *		bt_httpi	= (bt_httpi_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_httpi_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "socket_full"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_httpi->socket_full)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "http_uri"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_httpi->http_uri()	<< h.e_td();
			oss << h.e_tr();	
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cur_offset"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_httpi->cur_offset().to_human_string()
						<< " (" << bt_httpi->cur_offset() << ") "
						<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_io_vapi_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_httpi->io_vapi())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_httpi_mod_vapi_t"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_httpi->mod_vapi())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_io_write_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_httpi->bt_io_write)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(bt_httpi->callback)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.e_table();	
	oss << h.br();
		
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







