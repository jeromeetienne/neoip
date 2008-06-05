/*! \file
    \brief Declaration of the nslan_publish_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_nslan_publish_wikidbg.hpp"
#include "neoip_nslan_publish.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref nslan_publish_wikidbg_t defines the wikidbg stuff for \ref nslan_publish_t
 */
class nslan_publish_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();

	static std::string	tableheader(const std::string &keyword, void *object_ptr)	throw();
	static std::string	tablerow(const std::string &keyword, void *object_ptr)		throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr, httpd_request_t &request)	throw();
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief define all the handled keyword and their attached callbacks
 */
void	nslan_publish_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"		, nslan_publish_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl"	, nslan_publish_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr"	, nslan_publish_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"			, nslan_publish_wikidbg_t::page);

	keyword_db.insert_html("tableheader"		, nslan_publish_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"		, nslan_publish_wikidbg_t::tablerow);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string nslan_publish_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	nslan_publish_t *	nslan_publish	= (nslan_publish_t *)object_ptr;
	const nslan_rec_t &	nslan_rec	= nslan_publish->nslan_rec;
	std::ostringstream	oss;

	oss << nslan_rec.get_payload().get_len() << "-byte";

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
std::string nslan_publish_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	nslan_publish_t *	nslan_publish	= (nslan_publish_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", nslan_publish), wikidbg_html("page_title_attr", nslan_publish));
	oss << wikidbg_html("oneword", nslan_publish);
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
std::string nslan_publish_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	nslan_publish_t *	nslan_publish	= (nslan_publish_t *)object_ptr;
	const nslan_rec_t &	nslan_rec	= nslan_publish->nslan_rec;	
	std::ostringstream	oss;

	oss << "title=\"";
	oss << nslan_rec.get_payload().get_len() << "-byte";	
	oss << " with keyid=" << nslan_rec.get_keyid();
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
httpd_err_t nslan_publish_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	nslan_publish_t *	nslan_publish	= (nslan_publish_t *)object_ptr;
	const nslan_rec_t &	nslan_rec	= nslan_publish->nslan_rec;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", nslan_publish->nslan_peer);

	// put the title
	oss << h.pagetitle("nslan_publish_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "keyid"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nslan_rec.get_keyid()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ttl"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << nslan_rec.get_ttl()	<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	oss << h.sub1title("Payload:");
	oss << h.s_pre() << nslan_rec.get_payload() << h.e_pre();	

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
std::string nslan_publish_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "keyid"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "ttl"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "payload length"	<< h.e_b() << h.e_th();
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
std::string nslan_publish_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	nslan_publish_t*	nslan_publish	= (nslan_publish_t *)object_ptr;
	const nslan_rec_t &	nslan_rec	= nslan_publish->nslan_rec;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << h.s_link(wikidbg_url("page", nslan_publish), wikidbg_html("page_title_attr", nslan_publish))
				<< nslan_rec.get_keyid()
				<< h.e_link() << h.e_td();
	oss << h.s_td() << nslan_rec.get_ttl()			<< h.e_td();
	oss << h.s_td() << nslan_rec.get_payload().get_len() << "-byte"	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}


NEOIP_NAMESPACE_END







