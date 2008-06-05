/*! \file
    \brief Declaration of the nslan_rec_src_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_nslan_rec_src_wikidbg.hpp"
#include "neoip_nslan_rec_src.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref nslan_rec_src_wikidbg_t defines the wikidbg stuff for \ref nslan_rec_src_t
 */
class nslan_rec_src_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	nslan_rec_src_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, nslan_rec_src_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl"	, nslan_rec_src_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, nslan_rec_src_wikidbg_t::page_title_attr);	
	keyword_db.insert_page("page"			, nslan_rec_src_wikidbg_t::page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string nslan_rec_src_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	nslan_rec_src_t *	nslan_rec_src	= (nslan_rec_src_t *)object_ptr;
	std::ostringstream	oss;

	oss << nslan_rec_src->keyid;

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
std::string nslan_rec_src_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	nslan_rec_src_t *		nslan_rec_src	= (nslan_rec_src_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", nslan_rec_src), wikidbg_html("page_title_attr", nslan_rec_src));
	oss << wikidbg_html("oneword", nslan_rec_src);
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
std::string nslan_rec_src_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t nslan_rec_src_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	nslan_rec_src_t *	nslan_rec_src	= (nslan_rec_src_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("nslan_rec_src_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nslan_peer"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", nslan_rec_src->nslan_peer)
						<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "keyid"			<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nslan_rec_src->keyid		<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "want_more"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nslan_rec_src->want_more	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "nslan_query"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", nslan_rec_src->nslan_query)
						<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(nslan_rec_src->callback)
						<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();
	oss << h.br();
	

	const std::list<std::pair<nslan_rec_t, ipport_addr_t> > &rec_db	= nslan_rec_src->nslan_rec_db;
	std::list<std::pair<nslan_rec_t, ipport_addr_t> >::const_iterator	iter;


	// display the table of all the current nslan_peer_t
	oss << h.s_sub1title() << "List of all nslan_rec_t to be delivered_t: "
					<< rec_db.size() << h.e_sub1title();
	oss << h.s_table();
	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "keyid"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "ttl"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "payload length"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "source ipport"	<< h.e_b() << h.e_th();
	oss << h.e_tr();
	for(iter = rec_db.begin(); iter != rec_db.end(); iter++ ){
		const nslan_rec_t &	nslan_rec	= iter->first;
		const ipport_addr_t &	source_addr	= iter->second;
		oss << h.s_tr();
		oss << h.s_td() << nslan_rec.get_keyid()			<< h.e_td();
		oss << h.s_td() << nslan_rec.get_ttl()				<< h.e_td();
		oss << h.s_td() << nslan_rec.get_payload().get_len() << "-byte"	<< h.e_td();	
		oss << h.s_td() << source_addr					<< h.e_link();
		oss << h.e_tr();
	}
	oss << h.e_table();
	
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







