/*! \file
    \brief Declaration of the bt_ezswarm_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_ezswarm_wikidbg.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_ezswarm_http_t defines the wikidbg stuff for \ref bt_ezswarm_t
 */
class bt_ezswarm_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_ezswarm_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_ezswarm_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_ezswarm_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_ezswarm_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_ezswarm_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, bt_ezswarm_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_ezswarm_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_ezswarm_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_ezswarm_t *		bt_ezswarm	= (bt_ezswarm_t *)object_ptr;
	std::ostringstream	oss;

	oss << bt_ezswarm->mfile().infohash();

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
std::string bt_ezswarm_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_ezswarm_t *		bt_ezswarm	= (bt_ezswarm_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", bt_ezswarm), wikidbg_html("page_title_attr", bt_ezswarm));
	oss << wikidbg_html("oneword", bt_ezswarm);
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
std::string bt_ezswarm_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t bt_ezswarm_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_ezswarm_t *		bt_ezswarm	= (bt_ezswarm_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("bt_ezswarm_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "bt_ezsession"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_ezswarm->ezsession())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ezswarm_opt"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_ezswarm->opt()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "cur_state"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_ezswarm->cur_state()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "end_state"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << bt_ezswarm->end_state()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "mfile"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &bt_ezswarm->mfile())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "io_vapi"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_ezswarm->io_vapi())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "in_alloc"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_ezswarm->alloc())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "in_check"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_ezswarm->check())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "in_share"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_ezswarm->share())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "in_stopping"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_ezswarm->stopping())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "in_stopped"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", bt_ezswarm->stopped())
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

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
std::string bt_ezswarm_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "infohash"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "name"			<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "current_state"		<< h.e_b() << h.e_th();
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
std::string bt_ezswarm_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_ezswarm_t *		bt_ezswarm	= (bt_ezswarm_t *)object_ptr;
	const bt_mfile_t &	bt_mfile	= bt_ezswarm->mfile();
	std::ostringstream	oss;
	html_builder_t		h;

	// produce the html
	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", bt_ezswarm)	<< h.e_td();
	oss << h.s_td() << bt_mfile.name()				<< h.e_td();
	oss << h.s_td(); 
		if( bt_ezswarm->in_alloc() )	oss << wikidbg_html("oneword_pageurl", bt_ezswarm->alloc());
		if( bt_ezswarm->in_check() )	oss << wikidbg_html("oneword_pageurl", bt_ezswarm->check());
		if( bt_ezswarm->in_share() )	oss << wikidbg_html("oneword_pageurl", bt_ezswarm->share());
		if( bt_ezswarm->in_stopping())	oss << wikidbg_html("oneword_pageurl", bt_ezswarm->stopping());
		if( bt_ezswarm->in_stopped() )	oss << wikidbg_html("oneword_pageurl", bt_ezswarm->stopped());
	oss		<< h.e_td(); 
	oss << h.e_tr();

	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END







