/*! \file
    \brief Declaration of the bt_cast_mdata_dopublish_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_cast_mdata_dopublish_wikidbg.hpp"
#include "neoip_bt_cast_mdata_dopublish.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_herr_http_t defines the wikidbg stuff for \ref bt_cast_mdata_dopublish_t
 */
class bt_cast_mdata_dopublish_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_cast_mdata_dopublish_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_cast_mdata_dopublish_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_cast_mdata_dopublish_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_cast_mdata_dopublish_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_cast_mdata_dopublish_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_cast_mdata_dopublish_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "bt_cast_mdata_dopublish_t";

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
std::string bt_cast_mdata_dopublish_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_cast_mdata_dopublish_t *	mdata_dopublish	= (bt_cast_mdata_dopublish_t *)object_ptr;
	std::ostringstream		oss;
	html_builder_t			h;

	oss << h.s_link(wikidbg_url("page", mdata_dopublish), wikidbg_html("page_title_attr", mdata_dopublish));
	oss << wikidbg_html("oneword", mdata_dopublish);
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
std::string bt_cast_mdata_dopublish_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_cast_mdata_dopublish_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_cast_mdata_dopublish_t *	mdata_dopublish	= (bt_cast_mdata_dopublish_t *)object_ptr;
	std::ostringstream &		oss 		= request.get_reply();
	html_builder_t			h;

	// put the title
	oss << h.pagetitle("bt_cast_mdata_dopublish_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "mdata_srv_uri"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << mdata_dopublish->m_mdata_srv_uri
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cast_privtext"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << mdata_dopublish->m_cast_privtext
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cast_privhash"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << mdata_dopublish->m_cast_privhash
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "published"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << mdata_dopublish->published()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "publish_type"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << mdata_dopublish->m_publish_type
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "probe_set"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", mdata_dopublish->m_probe_set)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "probe_get"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", mdata_dopublish->m_probe_get)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "periodic_timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &mdata_dopublish->m_periodic_timeout)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "periodic_set"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", mdata_dopublish->m_periodic_set)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(mdata_dopublish->callback)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.e_table();	
	oss << h.br();

	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







