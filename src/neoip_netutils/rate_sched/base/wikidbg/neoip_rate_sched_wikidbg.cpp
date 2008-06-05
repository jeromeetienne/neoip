/*! \file
    \brief Declaration of the rate_sched_wikidbg_t

*/

/* system include */
/* local include */
#include "neoip_rate_sched_wikidbg.hpp"
#include "neoip_rate_sched.hpp"
#include "neoip_rate_limit.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref rate_sched_http_t defines the wikidbg stuff for \ref rate_sched_t
 */
class rate_sched_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	rate_sched_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, rate_sched_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", rate_sched_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", rate_sched_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, rate_sched_wikidbg_t::page);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string rate_sched_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	rate_sched_t *		rate_sched	= (rate_sched_t *)object_ptr;
	std::ostringstream	oss;
	double			precsum_ratio	= rate_sched->used_precsum().to_double()
							/ rate_sched->maxi_precsum().to_double();
	
	oss << "rate_sched";
	oss << " (using "	<< string_t::percent_string(precsum_ratio) << " of "
				<< string_t::size_string(uint64_t(rate_sched->max_rate()))
				<< ")";

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
std::string rate_sched_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	rate_sched_t *		rate_sched	= (rate_sched_t *)object_ptr;
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", rate_sched), wikidbg_html("page_title_attr", rate_sched));
	oss << wikidbg_html("oneword", rate_sched);
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
std::string rate_sched_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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

class rate_limit_cmp_t : public std::binary_function<rate_limit_t *, rate_limit_t *, bool> {
public:
	bool operator()(rate_limit_t *limit1, rate_limit_t *limit2)	const throw()
	{
		if( limit1->used_prec() > limit2->used_prec() )	return true;
		return false;
	}
};

class rate_limit_filter_t : public std::unary_function<rate_limit_t *, bool> {
public:
	bool operator()(rate_limit_t *rate_limit) const throw()
	{
		if( rate_limit->used_prec() == rate_prec_t(0) )	return true;
		return false;
	}
};

/** \brief Handle the "page" keyword
 */
httpd_err_t rate_sched_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	rate_sched_t *		rate_sched	= (rate_sched_t *)object_ptr;
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;
	double			precsum_ratio	= rate_sched->used_precsum().to_double()/rate_sched->maxi_precsum().to_double();

	// put the title
	oss << h.pagetitle("rate_sched_t Page");
	
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "max_rate (as configured)"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << string_t::size_string(uint64_t(rate_sched->max_rate()))
					<< " (" << rate_sched->max_rate() << ")"
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "maxi_precsum"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << rate_sched->maxi_precsum()
					<< h.e_td();
			oss << h.e_tr();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "used_precsum"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << rate_sched->used_precsum()
					<< " (" << string_t::percent_string(precsum_ratio) << " of maxi_precsum)"
					<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	

	// display the table of all the current rate_limit_t
	oss << h.s_sub1title() << "List of all current rate_limit_t: "
					<< rate_sched->limit_db.size() << h.e_sub1title();
	oss << h.s_table();
	// go thru the whole rate_sched_t::limit_db
	std::list<rate_limit_t *>		limit_db	= rate_sched->limit_db;
	std::list<rate_limit_t *>::iterator	iter;
	limit_db.sort(rate_limit_cmp_t());
//	limit_db.remove_if(rate_limit_filter_t());
	for( iter = limit_db.begin(); iter != limit_db.end(); iter++ ){
		const rate_limit_t *	rate_limit	= *iter;
		// if it is the first element, add the tableheader
		if(iter == limit_db.begin())	oss << wikidbg_html("tableheader", rate_limit);	
		// display this row
		oss << wikidbg_html("tablerow", rate_limit);
	}
	oss << h.e_table();
		
	// return no error
	return httpd_err_t::OK;
}


NEOIP_NAMESPACE_END







