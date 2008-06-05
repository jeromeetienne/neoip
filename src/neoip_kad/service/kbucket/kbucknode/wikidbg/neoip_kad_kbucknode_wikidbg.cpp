/*! \file
    \brief Declaration of the kad_kbucknode_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_kad_kbucknode_wikidbg.hpp"
#include "neoip_kad_kbucknode.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref kad_kbucknode_http_t defines the wikidbg stuff for \ref kad_kbucket_t::bucknode_t
 */
class kad_kbucknode_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	kad_kbucknode_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, kad_kbucknode_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", kad_kbucknode_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", kad_kbucknode_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, kad_kbucknode_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, kad_kbucknode_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, kad_kbucknode_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string kad_kbucknode_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	kad_kbucket_t::bucknode_t *	kad_kbucknode	= (kad_kbucket_t::bucknode_t *)object_ptr;
	std::ostringstream		oss;

	oss << kad_kbucknode->kad_addr;

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
std::string kad_kbucknode_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	kad_kbucket_t::bucknode_t *		kad_kbucknode	= (kad_kbucket_t::bucknode_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", kad_kbucknode), wikidbg_html("page_title_attr", kad_kbucknode));
	oss << wikidbg_html("oneword", kad_kbucknode);
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
std::string kad_kbucknode_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t kad_kbucknode_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	kad_kbucket_t::bucknode_t *	kad_kbucknode	= (kad_kbucket_t::bucknode_t *)object_ptr;
	std::ostringstream &		oss 		= request.get_reply();
	html_builder_t			h;

	// add the menu
	oss << wikidbg_html("menu", kad_kbucknode->kad_kbucket->kad_peer);
	
	// put the title
	oss << h.pagetitle("kad_kbucket_t::bucknode_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_addr"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_kbucknode->kad_addr	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Is Pinging"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << (kad_kbucknode->ping_in_progress() ? "yes" : "no")	<< h.e_td();
			oss << h.e_tr();
		if( kad_kbucknode->ping_in_progress() ){
			oss << h.s_tr();
				oss << h.s_td() << h.s_b() << "ping_rpc"	<< h.e_b() << h.e_td();
				oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", kad_kbucknode->ping_rpc)	<< h.e_td();
				oss << h.e_tr();
			oss << h.s_tr();
				oss << h.s_td() << h.s_b() << "pending node"	<< h.e_b() << h.e_td();
				oss << h.s_td() << ": " << kad_kbucknode->pending_node	<< h.e_td();
				oss << h.e_tr();
		}
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
std::string kad_kbucknode_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "kad_addr_t"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "is_pinging"	<< h.e_b() << h.e_th();
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
std::string kad_kbucknode_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	kad_kbucket_t::bucknode_t *	kad_kbucknode	= (kad_kbucket_t::bucknode_t *)object_ptr;
	std::ostringstream		oss;
	html_builder_t			h;

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", kad_kbucknode)	<< h.e_td();
	oss << h.s_td() << (kad_kbucknode->ping_in_progress() ? "yes" : "no")	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







