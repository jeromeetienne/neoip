/*! \file
    \brief Declaration of the bt_swarm_full_utmsg_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_full_utmsg_wikidbg.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_utmsg.hpp"
#include "neoip_bt_utmsg_vapi.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref bt_swarm_full_utmsg_t
 */
class bt_swarm_full_utmsg_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr
							, httpd_request_t &request)	throw();
	static std::string	page_info(const std::string &keyword, void *object_ptr)	throw();

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
void	bt_swarm_full_utmsg_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, bt_swarm_full_utmsg_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", bt_swarm_full_utmsg_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", bt_swarm_full_utmsg_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, bt_swarm_full_utmsg_wikidbg_t::page);

	keyword_db.insert_html("page_info"	, bt_swarm_full_utmsg_wikidbg_t::page_info);

	keyword_db.insert_html("tableheader"	, bt_swarm_full_utmsg_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, bt_swarm_full_utmsg_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string bt_swarm_full_utmsg_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_full_utmsg_t *	full_utmsg	= (bt_swarm_full_utmsg_t *)object_ptr;
	std::ostringstream	oss;

	oss << "full_utmsg (" << full_utmsg->convtable_db.size() << ")";

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
std::string bt_swarm_full_utmsg_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_full_utmsg_t *	full_utmsg	= (bt_swarm_full_utmsg_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", full_utmsg), wikidbg_html("page_title_attr", full_utmsg));
	oss << wikidbg_html("oneword", full_utmsg);
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
std::string bt_swarm_full_utmsg_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "full_utmsg established connection";
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
httpd_err_t bt_swarm_full_utmsg_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	bt_swarm_full_utmsg_t *	full_utmsg	= (bt_swarm_full_utmsg_t *)object_ptr;
	bt_swarm_t *		bt_swarm	= full_utmsg->swarm_full->get_swarm();
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", bt_swarm);
	// put the title
	oss << h.pagetitle("bt_swarm_full_utmsg_t Page");
	
	// put the page_info 
	oss << wikidbg_html("page_info", full_utmsg);

	// return no error
	return httpd_err_t::OK;
}

/** \brief Handle the "page_info" keyword
 */
std::string bt_swarm_full_utmsg_wikidbg_t::page_info(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_full_utmsg_t *	full_utmsg	= (bt_swarm_full_utmsg_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "swarm_full"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", full_utmsg->swarm_full)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "prog_version"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << full_utmsg->prog_version()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "tcp_listen_ipport"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << full_utmsg->tcp_listen_ipport()
					<< h.e_td();
			oss << h.e_tr();

	bt_swarm_full_utmsg_t::convtable_db_t &	convtable_db	= full_utmsg->convtable_db;
	bt_swarm_full_utmsg_t::convtable_db_t::iterator	iter;
	for(iter = convtable_db.begin(); iter != convtable_db.end(); iter++){
		bt_utmsgtype_t	bt_utmsgtype	= iter->first;
		size_t		remote_idx	= iter->second;
		bt_utmsg_cnx_vapi_t *cnx_vapi	= full_utmsg->cnx_vapi(bt_utmsgtype);
		oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "utmsgtype "<< bt_utmsgtype	
				<< h.e_b() << h.e_td();
		oss << h.s_td() << ": " << remote_idx		
				<< h.e_td();
		if( cnx_vapi ){
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", cnx_vapi)
					<< h.e_td();
		}
		oss << h.e_tr();
	}
	oss << h.e_table();
	oss << h.br();

	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "tableheader" keyword
 */
std::string bt_swarm_full_utmsg_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "key"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "nb msg"		<< h.e_b() << h.e_th();
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
std::string bt_swarm_full_utmsg_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	bt_swarm_full_utmsg_t *	full_utmsg	= (bt_swarm_full_utmsg_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", full_utmsg)	<< h.e_td();
	oss << h.s_td() << full_utmsg->convtable_db.size()		<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







