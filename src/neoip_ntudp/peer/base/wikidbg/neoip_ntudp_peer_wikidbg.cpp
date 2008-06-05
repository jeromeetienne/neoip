/*! \file
    \brief Declaration of the ntudp_peer_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_peer_wikidbg.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref ntudp_peer_wikidbg_t defines the wikidbg stuff for \ref ntudp_peer_t
 */
class ntudp_peer_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	wikidbg_menu(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	wikidbg_page_title_attr(const std::string &keyword, void *object_ptr)	throw();	
	static std::string	wikidbg_listen_addr(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	wikidbg_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	wikidbg_resp_oneword(const std::string &keyword, void *object_ptr)throw();
	static std::string	wikidbg_resp_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	wikidbg_resp_menu_item(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	wikidbg_resp_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	wikidbg_itor_oneword(const std::string &keyword, void *object_ptr)throw();
	static std::string	wikidbg_itor_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	wikidbg_itor_menu_item(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	wikidbg_itor_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	wikidbg_full_oneword(const std::string &keyword, void *object_ptr)throw();
	static std::string	wikidbg_full_page_title_attr(const std::string &keyword, void *object_ptr)throw();
	static std::string	wikidbg_full_menu_item(const std::string &keyword, void *object_ptr)throw();
	static httpd_err_t	wikidbg_full_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	ntudp_peer_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("menu"			, ntudp_peer_wikidbg_t::wikidbg_menu);
	keyword_db.insert_html("oneword"		, ntudp_peer_wikidbg_t::wikidbg_oneword);
	keyword_db.insert_html("oneword_pageurl"	, ntudp_peer_wikidbg_t::wikidbg_oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, ntudp_peer_wikidbg_t::wikidbg_page_title_attr);	
	keyword_db.insert_html("listen_addr"		, ntudp_peer_wikidbg_t::wikidbg_listen_addr);
	keyword_db.insert_page("page"			, ntudp_peer_wikidbg_t::wikidbg_page);
	
	keyword_db.insert_html("resp_oneword"		, ntudp_peer_wikidbg_t::wikidbg_resp_oneword);
	keyword_db.insert_html("resp_page_title_attr"	, ntudp_peer_wikidbg_t::wikidbg_resp_page_title_attr);
	keyword_db.insert_html("resp_menu_item"		, ntudp_peer_wikidbg_t::wikidbg_resp_menu_item);
	keyword_db.insert_page("resp_page"		, ntudp_peer_wikidbg_t::wikidbg_resp_page);
	
	keyword_db.insert_html("itor_oneword"		, ntudp_peer_wikidbg_t::wikidbg_itor_oneword);
	keyword_db.insert_html("itor_page_title_attr"	, ntudp_peer_wikidbg_t::wikidbg_itor_page_title_attr);
	keyword_db.insert_html("itor_menu_item"		, ntudp_peer_wikidbg_t::wikidbg_itor_menu_item);
	keyword_db.insert_page("itor_page"		, ntudp_peer_wikidbg_t::wikidbg_itor_page);
	
	keyword_db.insert_html("full_oneword"		, ntudp_peer_wikidbg_t::wikidbg_full_oneword);
	keyword_db.insert_html("full_page_title_attr"	, ntudp_peer_wikidbg_t::wikidbg_full_page_title_attr);
	keyword_db.insert_html("full_menu_item"		, ntudp_peer_wikidbg_t::wikidbg_full_menu_item);
	keyword_db.insert_page("full_page"		, ntudp_peer_wikidbg_t::wikidbg_full_page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_menu(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// build displayed html itself
	oss << h.s_link(wikidbg_url("page", ntudp_peer), "title=\"Main ntudp_peer_t page\"");
	oss << h.b("ntudp_peer_t:");
	oss << h.e_link();
	oss << " ";
	oss << wikidbg_html("menu_item", ntudp_peer->pserver_pool());
	if( ntudp_peer->current_npos_res.natted() ){
		oss << " / ";
		oss << wikidbg_html("menu_item", ntudp_peer->aview_pool());
	}
	oss << " / ";
	if( ntudp_peer->current_npos_res.inetreach() ){
		oss << wikidbg_html("menu_item", ntudp_peer->ntudp_pserver);
	}else{
		oss << wikidbg_html("menu_item", ntudp_peer->ntudp_relpeer);
	}
	oss << " / " << wikidbg_html("resp_menu_item", ntudp_peer);
	oss << " / " << wikidbg_html("itor_menu_item", ntudp_peer);
	oss << " / " << wikidbg_html("full_menu_item", ntudp_peer);
	oss << " / " << wikidbg_html("oneword_pageurl", &ntudp_peer->current_npos_res);
	oss << h.br();

	oss << h.br();

	// return the built string
	return oss.str();
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_oneword(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;

	oss << ntudp_peer->local_peerid();

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
std::string ntudp_peer_wikidbg_t::wikidbg_oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", ntudp_peer), wikidbg_html("page_title_attr", ntudp_peer));
	oss << wikidbg_html("oneword", ntudp_peer);
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
std::string ntudp_peer_wikidbg_t::wikidbg_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "PeerID=" << ntudp_peer->local_peerid();
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
httpd_err_t ntudp_peer_wikidbg_t::wikidbg_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", ntudp_peer);

	// put the title
	oss << h.pagetitle("ntudp_peer_t Page");
	

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_listener"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ntudp_peer->kad_listener())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "kad_peer"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ntudp_peer->kad_peer())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "udp_vresp"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ntudp_peer->udp_vresp())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ndiag_aview"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ntudp_peer->ndiag_aview())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "npos_watch"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", ntudp_peer->npos_watch())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Local peerid"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << ntudp_peer->local_peerid()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "rdvpt array"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": ";
			ntudp_rdvpt_arr_t rdvpt_arr	= ntudp_peer->get_rdvpt_arr();
			for(size_t i = 0; i < rdvpt_arr.size();i++){
				if( i != 0 )	oss << " ";
				oss << "[" << rdvpt_arr[i] << "]";
			}
			oss 		<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	// Display the listen_addr stuff
	oss << wikidbg_html("listen_addr", ntudp_peer);

	// return no error
	return httpd_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "listen_addr" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_listen_addr(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// Display the listen_addr stuff
	oss << h.s_sub1title() << "Listen Address: "<< h.e_sub1title();
	oss << h.s_table();
	oss << h.s_tr();
		oss << h.s_th() << h.s_b() << "Listen Address"	<< h.e_b() << h.e_th();
		oss << h.s_th() << h.s_b() << "Local View"	<< h.e_b() << h.e_th();
		oss << h.s_th() << h.s_b() << "Public View"	<< h.e_b() << h.e_th();
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "Current"		<< h.e_b() << h.e_td();
		oss << h.s_td() << ntudp_peer->listen_addr_lview() << h.e_td();
		if( ntudp_peer->current_npos_res.inetreach() ){
			oss << h.s_td() << ntudp_peer->listen_addr_pview() << h.e_td();
		}else{
			oss << h.s_td() << "n/a as not inetreach" << h.e_td();
		}
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.s_b() << "From config"		<< h.e_b() << h.e_td();
		oss << h.s_td() << ntudp_peer->listen_aview_cfg() << h.e_td();
		oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();

	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             ntudp_resp_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "resp_oneword" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_resp_oneword(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "(" << ntudp_peer->ntudp_resp_db.size() << ")";

	// return the built string
	return oss.str();
}


/** \brief Handle the "resp_page_title_attr" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_resp_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << ntudp_peer->ntudp_resp_db.size() << " Reponders";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "resp_menu_item" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_resp_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("resp_page", ntudp_peer), wikidbg_html("resp_page_title_attr", ntudp_peer));
	oss << "resp";
	oss << wikidbg_html("resp_oneword", ntudp_peer);
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "resp_page" keyword
 */
httpd_err_t ntudp_peer_wikidbg_t::wikidbg_resp_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", ntudp_peer);

	// put the title
	oss << h.pagetitle("ntudp_resp_t Page");
	
	// display the table of all the current ntudp_resp_t
	oss << h.s_sub1title() << "List of all current ntudp_resp_t: "
					<< ntudp_peer->ntudp_resp_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole ntudp_peer_t::ntudp_resp_db
	std::list<ntudp_resp_t *> &		resp_db = ntudp_peer->ntudp_resp_db;
	std::list<ntudp_resp_t *>::iterator	iter;
	for( iter = resp_db.begin(); iter != resp_db.end(); iter++ ){
		ntudp_resp_t *	ntudp_resp	= *iter;
		// if it is the first element, add the tableheader
		if( iter == resp_db.begin() )	oss << wikidbg_html("tableheader", ntudp_resp);	
		// display this row
		oss << wikidbg_html("tablerow", ntudp_resp);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             ntudp_itor_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "itor_oneword" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_itor_oneword(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "(" << ntudp_peer->ntudp_itor_db.size() << ")";

	// return the built string
	return oss.str();
}


/** \brief Handle the "itor_page_title_attr" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_itor_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << ntudp_peer->ntudp_itor_db.size() << " Reponders";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "itor_menu_item" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_itor_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("itor_page", ntudp_peer), wikidbg_html("itor_page_title_attr", ntudp_peer));
	oss << "itor";
	oss << wikidbg_html("itor_oneword", ntudp_peer);
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "itor_page" keyword
 */
httpd_err_t ntudp_peer_wikidbg_t::wikidbg_itor_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", ntudp_peer);

	// put the title
	oss << h.pagetitle("ntudp_itor_t Page");
	
	// display the table of all the current ntudp_itor_t
	oss << h.s_sub1title() << "List of all current ntudp_itor_t: "
					<< ntudp_peer->ntudp_itor_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole ntudp_peer_t::ntudp_itor_db
	std::list<ntudp_itor_t *> &		itor_db = ntudp_peer->ntudp_itor_db;
	std::list<ntudp_itor_t *>::iterator	iter;
	for( iter = itor_db.begin(); iter != itor_db.end(); iter++ ){
		ntudp_itor_t *	ntudp_itor	= *iter;
		// if it is the first element, add the tableheader
		if( iter == itor_db.begin() )	oss << wikidbg_html("tableheader", ntudp_itor);	
		// display this row
		oss << wikidbg_html("tablerow", ntudp_itor);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                             ntudp_full_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "full_oneword" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_full_oneword(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "(" << ntudp_peer->ntudp_full_db.size() << ")";

	// return the built string
	return oss.str();
}


/** \brief Handle the "full_page_title_attr" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_full_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	
	oss << "title=\"";
	oss << ntudp_peer->ntudp_full_db.size() << " Reponders";
	oss << "\"";

	// return the built string
	return oss.str();
}

/** \brief Handle the "full_menu_item" keyword
 */
std::string ntudp_peer_wikidbg_t::wikidbg_full_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;
	
	oss << h.s_link(wikidbg_url("full_page", ntudp_peer), wikidbg_html("full_page_title_attr", ntudp_peer));
	oss << "full";
	oss << wikidbg_html("full_oneword", ntudp_peer);
	oss << h.e_link();

	// return the built string
	return oss.str();
}

/** \brief Handle the "full_page" keyword
 */
httpd_err_t ntudp_peer_wikidbg_t::wikidbg_full_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	ntudp_peer_t *		ntudp_peer	= (ntudp_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", ntudp_peer);

	// put the title
	oss << h.pagetitle("ntudp_full_t Page");
	
	// display the table of all the current ntudp_full_t
	oss << h.s_sub1title() << "List of all current ntudp_full_t: "
					<< ntudp_peer->ntudp_full_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole ntudp_peer_t::ntudp_full_db
	std::list<ntudp_full_t *> &		full_db = ntudp_peer->ntudp_full_db;
	std::list<ntudp_full_t *>::iterator	iter;
	for( iter = full_db.begin(); iter != full_db.end(); iter++ ){
		ntudp_full_t *	ntudp_full	= *iter;
		// if it is the first element, add the tableheader
		if( iter == full_db.begin() )	oss << wikidbg_html("tableheader", ntudp_full);	
		// display this row
		oss << wikidbg_html("tablerow", ntudp_full);
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







