/*! \file
    \brief Declaration of the kad_db_rec_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_kad_db_rec_wikidbg.hpp"
#include "neoip_kad_db_rec.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref kad_db_rec_http_t defines the wikidbg stuff for \ref kad_db_t::rec_t
 */
class kad_db_rec_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
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
void	kad_db_rec_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_page("page"		, kad_db_rec_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, kad_db_rec_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, kad_db_rec_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "page" keyword
 */
httpd_err_t kad_db_rec_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	kad_db_t::rec_t *	kad_db_rec	= (kad_db_t::rec_t *)object_ptr;
	const kad_rec_t &	kad_rec		= kad_db_rec->kad_rec;
	kad_db_t *		kad_db		= kad_db_rec->kad_db;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// add the menu
	oss << wikidbg_html("menu", kad_db->kad_peer);
	
	// put the title
	oss << h.pagetitle("kad_db_t::rec_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Database"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << h.s_link(wikidbg_url("page", kad_db), wikidbg_html("page_title_attr", kad_db));
			if( kad_db->is_local_db() )	oss << "local";
			else				oss << "remote";
			oss 		<< h.e_link() << h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "recid"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_rec.get_recid()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "keyid"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_rec.get_keyid()	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ttl"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_rec.get_ttl()		<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "expire timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_db_rec->expire_timeout	<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "republish timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << kad_db_rec->expire_timeout	<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	
	oss << h.sub1title("Payload:");
	oss << h.s_pre() << kad_rec.get_payload() << h.e_pre();	

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
std::string kad_db_rec_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "RecID"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "KeyID"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "Payload length"	<< h.e_b() << h.e_th();
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
std::string kad_db_rec_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	kad_db_t::rec_t*		kad_db_rec	= (kad_db_t::rec_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_td() << h.s_link(wikidbg_url("page", kad_db_rec), wikidbg_html("page_title_attr", kad_db_rec))
				<< kad_db_rec->get_recid()
				<< h.e_link();
	oss << h.s_td() << kad_db_rec->get_keyid()				<< h.e_td();
	oss << h.s_td() << kad_db_rec->get_payload().get_len() << "-byte"		<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}
NEOIP_NAMESPACE_END







