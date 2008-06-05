/*! \file
    \brief Declaration of the rate_limit_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_rate_limit_wikidbg.hpp"
#include "neoip_rate_limit.hpp"
#include "neoip_rate_sched.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref bt_swarm_http_t defines the wikidbg stuff for \ref rate_limit_t
 */
class rate_limit_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
public:
	static std::string	oneword(const std::string &keyword, void *object_ptr)		throw();
	static std::string	oneword_pageurl(const std::string &keyword, void *object_ptr)	throw();
	static std::string	page_title_attr(const std::string &keyword, void *object_ptr)	throw();
	static httpd_err_t	page(const std::string &keyword, void *object_ptr
								, httpd_request_t &request)	throw();
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
void	rate_limit_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, rate_limit_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", rate_limit_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", rate_limit_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, rate_limit_wikidbg_t::page);

	keyword_db.insert_html("tableheader"	, rate_limit_wikidbg_t::tableheader);
	keyword_db.insert_html("tablerow"	, rate_limit_wikidbg_t::tablerow);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string rate_limit_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "rate_limit_t";

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
std::string rate_limit_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	rate_limit_t *		rate_limit	= (rate_limit_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", rate_limit), wikidbg_html("page_title_attr", rate_limit));
	oss << wikidbg_html("oneword", rate_limit);
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
std::string rate_limit_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "title=\"";
	oss << "rate_limit_t stuff";
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
httpd_err_t rate_limit_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	rate_limit_t *		rate_limit	= (rate_limit_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("rate_limit_t Page");

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "rate_sched"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", rate_limit->rate_sched())
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "maxi_prec"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << rate_limit->maxi_prec()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "used_prec"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << rate_limit->used_prec()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "absrate_max"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": ";
			if( rate_limit->absrate_max() == std::numeric_limits<double>::max() ){
				oss << "none";
			}else{
				oss << ": " << rate_limit->absrate_max();
				oss << " (" << string_t::size_string(uint64_t(rate_limit->absrate_max())) << ")";
			}
			oss << h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "is_used"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << (rate_limit->is_used() ? "yes" : "no")
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "inuse_timeout"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html("oneword_pageurl", &rate_limit->inuse_timeout)
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "callback"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << wikidbg_html_callback(rate_limit->callback)
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
std::string rate_limit_wikidbg_t::tableheader(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_tr();
	oss << h.s_th() << h.s_b() << "key"		<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "maxi_prec"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "used_prec"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "used_prec ratio"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "absrate_max"	<< h.e_b() << h.e_th();
	oss << h.s_th() << h.s_b() << "callback"	<< h.e_b() << h.e_th();
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
std::string rate_limit_wikidbg_t::tablerow(const std::string &keyword, void *object_ptr) throw()
{
	rate_limit_t *		rate_limit	= (rate_limit_t *)object_ptr;
	rate_sched_t *		rate_sched	= rate_limit->rate_sched();
	double			used_prec_ratio	= rate_limit->used_prec().to_double() / rate_sched->used_precsum().to_double();
	std::ostringstream	oss;
	html_builder_t		h;
	

	oss << h.s_tr();
	oss << h.s_td() << wikidbg_html("oneword_pageurl", rate_limit)			<< h.e_td();
	oss << h.s_td() << rate_limit->maxi_prec()					<< h.e_td();
	oss << h.s_td() << rate_limit->used_prec()					<< h.e_td();
	oss << h.s_td() << string_t::percent_string(used_prec_ratio) 
			<< " (" << string_t::size_string(uint64_t(used_prec_ratio * rate_sched->max_rate())) << ")"
											<< h.e_td();
	oss << h.s_td();
			if( rate_limit->absrate_max() == std::numeric_limits<double>::max() ){
				oss << "none";
			}else{
				oss << string_t::size_string(uint64_t(rate_limit->absrate_max()));
			}
			oss << h.e_td();
	oss << h.s_td() << wikidbg_html_callback(rate_limit->callback)	<< h.e_td();
	oss << h.e_tr();

	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END







