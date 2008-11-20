/*! \file
    \brief Declaration of the rtmp_cam_listener_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_rtmp_cam_listener_wikidbg.hpp"
#include "neoip_rtmp_cam_listener.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref rtmp_cam_listener_http_t defines the wikidbg stuff for \ref rtmp_cam_listener_t
 */
class rtmp_cam_listener_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	rtmp_cam_listener_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw()
{
	// add the keywords
	keyword_db.insert_html("oneword"	, rtmp_cam_listener_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", rtmp_cam_listener_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", rtmp_cam_listener_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, rtmp_cam_listener_wikidbg_t::page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string rtmp_cam_listener_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "rtmp_cam_listener_t";

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
std::string rtmp_cam_listener_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	rtmp_cam_listener_t *	cam_listener	= (rtmp_cam_listener_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", cam_listener), wikidbg_html("page_title_attr", cam_listener));
	oss << wikidbg_html("oneword", cam_listener);
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
std::string rtmp_cam_listener_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t rtmp_cam_listener_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	rtmp_cam_listener_t *	cam_listener	= (rtmp_cam_listener_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("rtmp_cam_listener_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "rtmp_resp_t"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", cam_listener->rtmp_resp)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.e_table();
	oss << h.br();

	// display the table of all the current rtmp_cam_resp_t
	oss << h.s_sub1title() << "List of all current rtmp_cam_resp_t: "
					<< cam_listener->cam_resp_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole cam_listener_t::cam_resp_db
	std::list<rtmp_cam_resp_t *> &		cam_resp_db = cam_listener->cam_resp_db;
	std::list<rtmp_cam_resp_t *>::iterator	iter_resp;
	for( iter_resp = cam_resp_db.begin(); iter_resp != cam_resp_db.end(); iter_resp++ ){
		rtmp_cam_resp_t *	cam_resp	= *iter_resp;
		// if it is the first element, add the tableheader
		if( iter_resp == cam_resp_db.begin() )	oss << wikidbg_html("tableheader", cam_resp);
		// display this row
		oss << wikidbg_html("tablerow", cam_resp);
	}
	oss << h.e_table();

	// display the table of all the current rtmp_cam_full_t
	oss << h.s_sub1title() << "List of all current rtmp_cam_full_t: "
					<< cam_listener->cam_full_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole cam_listener_t::cam_full_db
	std::list<rtmp_cam_full_t *> &		cam_full_db = cam_listener->cam_full_db;
	std::list<rtmp_cam_full_t *>::iterator	iter_full;
	for( iter_full = cam_full_db.begin(); iter_full != cam_full_db.end(); iter_full++ ){
		rtmp_cam_full_t *	cam_full	= *iter_full;
		// if it is the first element, add the tableheader
		if( iter_full == cam_full_db.begin() )	oss << wikidbg_html("tableheader", cam_full);
		// display this row
		oss << wikidbg_html("tablerow", cam_full);
	}
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







