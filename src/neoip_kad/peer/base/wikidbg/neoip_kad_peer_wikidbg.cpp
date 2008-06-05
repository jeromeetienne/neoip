/*! \file
    \brief Declaration of the kad_peer_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_kad_peer_wikidbg.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref kad_peer_http_t defines the wikidbg stuff for \ref kad_peer_t
 */
class kad_peer_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	menu(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();

	static std::string	tableheader(const std::string &keyword, void *object_ptr)	throw();
	static std::string	tablerow(const std::string &keyword, void *object_ptr)		throw();

	static std::string	stat_menu_item(const std::string &keyword, void *object_ptr)	throw();
	static std::string	stat_page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	stat_page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	kad_peer_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("menu"		, kad_peer_wikidbg_t::menu);
	keyword_db.insert_html("oneword"	, kad_peer_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", kad_peer_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", kad_peer_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, kad_peer_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, kad_peer_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, kad_peer_wikidbg_t::tablerow);

	keyword_db.insert_html("stat_menu_item"		, kad_peer_wikidbg_t::stat_menu_item);
	keyword_db.insert_html("stat_page_title_attr"	, kad_peer_wikidbg_t::stat_page_title_attr);
	keyword_db.insert_page("stat_page"		, kad_peer_wikidbg_t::stat_page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "menu" keyword
 */
std::string kad_peer_wikidbg_t::menu(const std::string &keyword, void *object_ptr) throw()
{
	kad_peer_t *		kad_peer	= (kad_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	// build displayed html itself
	oss << h.s_link(wikidbg_url("page", kad_peer), "title=\"Main kad_peer_t page\"");
	oss << h.b("kad_peer_t:");
	oss << h.e_link();
	oss << " ";
	oss << wikidbg_html("menu_item", kad_peer->local_db);
	oss << " / ";
	oss << wikidbg_html("menu_item", kad_peer->remote_db);
	oss << " / ";
	oss << wikidbg_html("menu_item", kad_peer->kbucket);
	oss << " / ";
	oss << wikidbg_html("stat_menu_item", kad_peer);

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
std::string kad_peer_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	kad_peer_t *		kad_peer	= (kad_peer_t *)object_ptr;
	std::ostringstream	oss;

	oss << kad_peer->local_peerid();

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
std::string kad_peer_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	kad_peer_t *		kad_peer	= (kad_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", kad_peer), wikidbg_html("page_title_attr", kad_peer));
	oss << wikidbg_html("oneword", kad_peer);
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
std::string kad_peer_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t kad_peer_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	kad_peer_t *		kad_peer	= (kad_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", kad_peer);
	
	// put the title
	oss << h.pagetitle("kad_peer_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "PeerID"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_peer->local_peerid()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "RealmID"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_peer->get_realmid()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Kad Listener"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", kad_peer->kad_listener)<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nslan Publish"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", kad_peer->nslan_publish)<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Local database"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", kad_peer->local_db)<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Remote database"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", kad_peer->remote_db)<< h.e_td();
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
std::string kad_peer_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "PeerID"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "RealmID"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "local db"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "remote db"	<< h.e_b() << h.e_th();
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
std::string kad_peer_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	kad_peer_t*		kad_peer	= (kad_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << h.s_link(wikidbg_url("page", kad_peer), wikidbg_html("page_title_attr", kad_peer))
				<< kad_peer->local_peerid()
				<< h.e_link();
	oss << h.s_td() << kad_peer->get_realmid()				<< h.e_td();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", kad_peer->local_db)	<< h.e_td();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", kad_peer->remote_db)	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "page_menu_item" keyword
 */
std::string kad_peer_wikidbg_t::stat_menu_item(const std::string &keyword, void *object_ptr) throw()
{
	kad_peer_t *		kad_peer	= (kad_peer_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("stat_page", kad_peer), wikidbg_html("stat_page_title_attr", kad_peer));
	oss << "stat";
	oss << h.e_link();
	
	// return the built string
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "stat_page_title_attr" keyword
 */
std::string kad_peer_wikidbg_t::stat_page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	kad_peer_t *		kad_peer	= (kad_peer_t *)object_ptr;
	kad_stat_t *		kad_stat	= kad_peer->get_stat();
	std::ostringstream	oss;

	oss << "title=\"";
	oss << kad_stat->nb_byte_sent << "-byte sent";
	oss << "/";
	oss << kad_stat->nb_byte_recv << "-byte recv";
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
httpd_err_t kad_peer_wikidbg_t::stat_page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	kad_peer_t *		kad_peer	= (kad_peer_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", kad_peer);
	
	// put the title
	oss << h.pagetitle("kad_stat_t Page");

	// display the rpc statistic
	oss << h.sub1title("RPC statistic");
	kad_stat_t &	stat		= kad_peer->stat;
	uint64_t	nb_rpc_sent	= stat.nb_ping_sent + stat.nb_store_sent + stat.nb_findnode_sent
						+ stat.nb_findsomeval_sent + stat.nb_findallval_sent 
						+ stat.nb_delete_sent;
	uint64_t	nb_rpc_recv	= stat.nb_ping_recv + stat.nb_store_recv + stat.nb_findnode_recv
						+ stat.nb_findsomeval_recv + stat.nb_findallval_recv
						+ stat.nb_delete_recv;
	oss << h.s_table() << h.s_tr();
	oss << h.s_th() << "direction \\ rpc request "	<< h.e_th();
	oss << h.s_th() << "ping"		<< h.e_th();
	oss << h.s_th() << "store"		<< h.e_th();
	oss << h.s_th() << "findnode"		<< h.e_th();
	oss << h.s_th() << "findsomeval"	<< h.e_th();
	oss << h.s_th() << "findallval"		<< h.e_th();
	oss << h.s_th() << "delete"		<< h.e_th();
	oss << h.s_th() << "total"		<< h.e_th();
	oss << h.e_tr();	
	oss << h.s_tr();
		oss << h.s_td() << h.b("Sent") << h.e_td();
		oss << h.s_td() << stat.nb_ping_sent		<< h.e_td();
		oss << h.s_td() << stat.nb_store_sent		<< h.e_td();
		oss << h.s_td() << stat.nb_findnode_sent	<< h.e_td();
		oss << h.s_td() << stat.nb_findsomeval_sent	<< h.e_td();
		oss << h.s_td() << stat.nb_findallval_sent	<< h.e_td();
		oss << h.s_td() << stat.nb_delete_sent		<< h.e_td();
		oss << h.s_td() << nb_rpc_sent			<< h.e_td();		
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.b("Received") << h.e_td();
		oss << h.s_td() << stat.nb_ping_recv		<< h.e_td();
		oss << h.s_td() << stat.nb_store_recv		<< h.e_td();
		oss << h.s_td() << stat.nb_findnode_recv	<< h.e_td();
		oss << h.s_td() << stat.nb_findsomeval_recv	<< h.e_td();
		oss << h.s_td() << stat.nb_findallval_recv	<< h.e_td();
		oss << h.s_td() << stat.nb_delete_recv		<< h.e_td();
		oss << h.s_td() << nb_rpc_recv			<< h.e_td();		
		oss << h.e_tr();
	oss << h.e_table();
	
	oss << h.sub1title("Network statistic");
	oss << h.s_table() << h.s_tr();
	oss << h.s_th() << " "	<< h.e_th();
	oss << h.s_th() << "nb byte"		<< h.e_th();
	oss << h.e_tr();	
	oss << h.s_tr();
		oss << h.s_td() << h.b("Sent") << h.e_td();
		oss << h.s_td() << stat.nb_byte_sent	<< h.e_td();
		oss << h.e_tr();
	oss << h.s_tr();
		oss << h.s_td() << h.b("Received") << h.e_td();
		oss << h.s_td() << stat.nb_byte_recv	<< h.e_td();
		oss << h.e_tr();
	oss << h.e_table();

	// return no error
	return httpd_err_t::OK;
}
NEOIP_NAMESPACE_END







