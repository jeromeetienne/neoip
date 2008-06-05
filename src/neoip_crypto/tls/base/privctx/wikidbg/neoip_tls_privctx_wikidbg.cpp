/*! \file
    \brief Declaration of the tls_privctx_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_tls_privctx_wikidbg.hpp"
#include "neoip_tls_privctx.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref tls_privctx_http_t defines the wikidbg stuff for \ref tls_privctx_t
 */
class tls_privctx_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	tls_privctx_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, tls_privctx_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", tls_privctx_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", tls_privctx_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, tls_privctx_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string tls_privctx_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "tls_privctx_t";

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
std::string tls_privctx_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	tls_privctx_t *	tls_privctx	= (tls_privctx_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", tls_privctx), wikidbg_html("page_title_attr", tls_privctx));
	oss << wikidbg_html("oneword", tls_privctx);
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
std::string tls_privctx_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t tls_privctx_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	tls_privctx_t *		tls_privctx	= (tls_privctx_t *)object_ptr;
	std::ostringstream &	oss 		= request.get_reply();
	html_builder_t		h;
	gnutls_session_t	gnutls_sess	= tls_privctx->session();

	// put the title
	oss << h.pagetitle("tls_privctx_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Key exchange"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << gnutls_kx_get_name(gnutls_kx_get(gnutls_sess))
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Protocol"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << gnutls_protocol_get_name(gnutls_protocol_get_version(gnutls_sess))
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Certificate type"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << gnutls_certificate_type_get_name(gnutls_certificate_type_get(gnutls_sess))
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Compression"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << gnutls_compression_get_name(gnutls_compression_get(gnutls_sess))
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Cipher"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << gnutls_cipher_get_name(gnutls_cipher_get(gnutls_sess))
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "Mac"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << gnutls_mac_get_name(gnutls_mac_get(gnutls_sess))
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
		
	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







