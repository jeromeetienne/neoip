/*! \file
    \brief Declaration of the nipmem_tracker_http_wikidbg_t

- to provide option in the web page to filter/sort the entry
- display statistic on the tracked zone
- use some garbage collection technique to discover the chained allocation
  - e.g. object A has been allocated and contained a pointer on object B
  - so object A uses the size of object B too
  - selfsize = object A
  - subsize = object B
  - totsize = selfsize + subsize
- TODO to remove the nested class

*/

/* system include */
#include <stdlib.h>
/* local include */
#include "neoip_nipmem_tracker_http_wikidbg.hpp"
#include "neoip_nipmem_tracker_http.hpp"
#include "neoip_nipmem_tracker.hpp"
#include "neoip_lib_httpd_wikidbg.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief \ref nipmem_tracker_http_t defines the wikidbg stuff for \ref nipmem_tracker_t
 */
class nipmem_tracker_http_wikidbg_t : NEOIP_COPY_CTOR_ALLOW {
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
void	nipmem_tracker_http_wikidbg_init(wikidbg_keyword_db_t &keyword_db)	throw() 
{
	// add the keywords
	keyword_db.insert_html("oneword"	, nipmem_tracker_http_wikidbg_t::oneword);
	keyword_db.insert_html("oneword_pageurl", nipmem_tracker_http_wikidbg_t::oneword_pageurl);
	keyword_db.insert_html("page_title_attr", nipmem_tracker_http_wikidbg_t::page_title_attr);
	keyword_db.insert_page("page"		, nipmem_tracker_http_wikidbg_t::page);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the "oneword" keyword
 */
std::string nipmem_tracker_http_wikidbg_t::oneword(const std::string &keyword, void *object_ptr) throw()
{
	std::ostringstream	oss;

	oss << "nipmem_tracker_t";

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
std::string nipmem_tracker_http_wikidbg_t::oneword_pageurl(const std::string &keyword, void *object_ptr) throw()
{
	nipmem_tracker_t *	nipmem_tracker	= get_global_nipmem_tracker();
	std::ostringstream	oss;
	html_builder_t		h;

	oss << h.s_link(wikidbg_url("page", nipmem_tracker), wikidbg_html("page_title_attr", nipmem_tracker));
	oss << wikidbg_html("oneword", nipmem_tracker);
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
std::string nipmem_tracker_http_wikidbg_t::page_title_attr(const std::string &keyword, void *object_ptr) throw()
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
httpd_err_t nipmem_tracker_http_wikidbg_t::page(const std::string &keyword, void *object_ptr
						, httpd_request_t &request) throw()
{
	nipmem_tracker_t *	nipmem_tracker	= get_global_nipmem_tracker();
	std::ostringstream	&oss 		= request.get_reply();
	html_builder_t		h;

	// put the title
	oss << h.pagetitle("nipmem_tracker_t Page");
	
#ifdef	__linux__
	struct mallinfo	m_info	= mallinfo();
	oss << h.s_sub1title() << "mallinfo from libc:" << h.e_sub1title();
	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "arena"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << m_info.arena		<< " - non-mmapped space allocated from system"
					<< h.e_td();
			oss << h.e_tr();		
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "ordblks"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << m_info.ordblks	<< " - number of free chunks"
					<< h.e_td();
			oss << h.e_tr();		
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "smblks"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << m_info.smblks	<< " - number of fastbin blocks"
					<< h.e_td();
			oss << h.e_tr();		
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "hblks"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << m_info.hblks		<< " - number of mmapped regions"
					<< h.e_td();
			oss << h.e_tr();		
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "hblkhd"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << m_info.hblkhd	<< " - space in mmapped regions"
					<< h.e_td();
			oss << h.e_tr();		
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "usmblks"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << m_info.usmblks	<< " - maximum total allocated space"
					<< h.e_td();
			oss << h.e_tr();		
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "fsmblks"		<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << m_info.fsmblks	<< " - space available in freed fastbin blocks"
					<< h.e_td();
			oss << h.e_tr();		
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "uordblks"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << m_info.uordblks	<< " - total allocated space "
					<< h.e_td();
			oss << h.e_tr();		
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "fordblks"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << m_info.fordblks	<< " - total free space"
					<< h.e_td();
			oss << h.e_tr();		
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "keepcost"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << m_info.keepcost	<< " - top-most, releasable (via malloc_trim) space"
					<< h.e_td();
			oss << h.e_tr();				
	oss << h.e_table();
	oss << h.br();
#endif

#if NO_NIPMEM_TRACKING
	oss << "NOT TRACKED";
	return httpd_err_t::OK;
#endif

	std::map<void *, nipmem_tracker_t::zone_t> &		zone_db	= nipmem_tracker->zone_db;
	std::map<void *, nipmem_tracker_t::zone_t>::iterator	iter;

	// compute the length_sum
	size_t	length_sum	= 0;
	for( iter = zone_db.begin(); iter != zone_db.end(); iter++ ){
		nipmem_tracker_t::zone_t &	zone = iter->second;
		length_sum	+= zone.buffer_len;
	}

	oss << h.br();
	oss << h.s_table_packed_noborder();
		oss << h.s_tr();
			oss << h.s_td() << h.s_b() << "length_sum"	<< h.e_b() << h.e_td();
			oss << h.s_td() << ": " << length_sum	<< h.e_td();
			oss << h.e_tr();
	oss << h.e_table();	
	oss << h.br();
	
	oss << h.s_table();
	// put the table header
	oss << h.s_tr() << h.th("Pointer") << h.th("Length") << h.th("Secure") << h.th("Type Name")
						<< h.th("Location") << h.e_tr();
	// put one row for each zone in the zone_db
	for( iter = zone_db.begin(); iter != zone_db.end(); iter++ ){
		nipmem_tracker_t::zone_t &	zone = iter->second;
		oss << h.s_tr();
		oss << h.s_td() << zone.buffer_ptr << h.e_td();
		oss << h.s_td() << zone.buffer_len << h.e_td();
		oss << h.s_td() << (zone.secure_f ? "secured" : "unsecured") << h.e_td();
		oss << h.s_td() << zone.type_name << h.e_td();
		oss << h.s_td() << zone.filename << ":" << zone.lineno << ":"
						<< zone.fct_name << "()" << h.e_td();
	}
	h.e_table();

	// return no error
	return httpd_err_t::OK;
}

NEOIP_NAMESPACE_END







