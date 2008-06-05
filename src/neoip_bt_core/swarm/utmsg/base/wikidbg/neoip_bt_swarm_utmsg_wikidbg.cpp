/*! \file
    \brief Declaration of the bt_swarm_utmsg_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_utmsg_wikidbg.hpp"
#include "neoip_bt_swarm_utmsg.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_utmsg_vapi.hpp"
#include "neoip_bt_utmsgtype.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_swarm_utmsg_t
 */
class bt_swarm_utmsg_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	bt_swarm_utmsg_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_swarm_utmsg_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_swarm_utmsg_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_swarm_utmsg_wikidbg_t::page_title_attr);
	keyword_db.insert_html("menu_item"	, bt_swarm_utmsg_wikidbg_t::menu_item);
	keyword_db.insert_page("page"		, bt_swarm_utmsg_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_swarm_utmsg_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_utmsg_t *	swarm_utmsg	= (bt_swarm_utmsg_t *)object_ptr;	
	std::ostringstream	oss;

	oss << "swarm utmsg (" << swarm_utmsg->utmsg_vapi_db().size() << ")";

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
std::string bt_swarm_utmsg_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_utmsg_t *	swarm_utmsg	= (bt_swarm_utmsg_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", swarm_utmsg), wikidbg_html("page_title_attr", swarm_utmsg));
	oss << wikidbg_html("oneword", swarm_utmsg);
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
std::string bt_swarm_utmsg_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "utmsg";
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
std::string bt_swarm_utmsg_wikidbg_t::menu_item(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_utmsg_t *	swarm_utmsg	= (bt_swarm_utmsg_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("page", swarm_utmsg), wikidbg_html("page_title_attr", swarm_utmsg));
	oss << wikidbg_html("oneword", swarm_utmsg);
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
httpd_err_t bt_swarm_utmsg_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_utmsg_t *	swarm_utmsg	= (bt_swarm_utmsg_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", swarm_utmsg->bt_swarm());

	// put the title
	oss << h.pagetitle("bt_swarm_utmsg_t Page");
	
	// display all the registered bt_utmsg_vapi_t
	oss << h.br();
	oss << h.s_table();	
	const	bt_swarm_utmsg_t::utmsg_vapi_db_t &		utmsg_vapi_db	= swarm_utmsg->utmsg_vapi_db();
	bt_swarm_utmsg_t::utmsg_vapi_db_t::const_iterator	iter;
	for(iter = utmsg_vapi_db.begin(); iter != utmsg_vapi_db.end(); iter++ ){
		bt_utmsg_vapi_t * utmsg_vapi	= *iter;
		if( iter == utmsg_vapi_db.begin() ){
			oss << h.s_tr();
			oss << h.s_th() << h.s_b() << "link"		<< h.e_b() << h.e_th();
			oss << h.s_th() << h.s_b() << "utmsgtype"	<< h.e_b() << h.e_th();
			oss << h.s_th() << h.s_b() << "utmsgstr"	<< h.e_b() << h.e_th();
			oss << h.e_tr();
		}
		oss << h.s_tr();
		oss << h.s_td() << wikidbg_html("oneword_pageurl", utmsg_vapi)	<< h.e_td();
		oss << h.s_td() << utmsg_vapi->utmsgtype()			<< h.e_td();
		oss << h.s_td() << utmsg_vapi->utmsgstr()			<< h.e_td();
		oss << h.e_tr();
	}
	oss << h.e_table();	
	oss << h.br();
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







