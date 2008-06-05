/*! \file
    \brief Declaration of the wikidbg_keyword_db_t

*/

/* system include */
/* local include */
#include "neoip_wikidbg_keyword_db.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      handle the html_db
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Insert a new keyword with its associated handler in the wikidbg_keyword_db_t
 */
wikidbg_keyword_db_t &	wikidbg_keyword_db_t::insert_html(const std::string &keyword
						, wikidbg_keyword_html_cb_t callback)	throw()
{
	// insert the element
	bool	succeed = html_db.insert(std::make_pair(keyword, callback)).second;
	DBG_ASSERT( succeed );
	// return the object itself
	return *this;
}

/** \brief Remove a keyword from the wikidbg_keyword_db_t
 */
wikidbg_keyword_db_t &	wikidbg_keyword_db_t::remove_html(const std::string &keyword)		throw()
{
	// sanity check - the keyword MUST be present in the database
	DBG_ASSERT( html_db.find(keyword) != html_db.end() );
	// remove this keyword from the database
	html_db.erase(keyword);
	// return the object itself
	return *this;
}

/** \brief Return the specific handler callback for a specific keyword, NULL if none is found
 */
wikidbg_keyword_html_cb_t	wikidbg_keyword_db_t::find_html(const std::string &keyword)	const throw()
{
	std::map<std::string, wikidbg_keyword_html_cb_t>::const_iterator	iter;
	// try to find the keyword in the database
	iter = html_db.find(keyword);
	// if the keyword is not found, return NULL
	if( iter == html_db.end() )	return NULL;
	// if the keyword is found, return the handler callback
	return iter->second;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      handle the page_db
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Insert a new keyword with its associated handler in the wikidbg_keyword_db_t
 */
wikidbg_keyword_db_t &	wikidbg_keyword_db_t::insert_page(const std::string &keyword
						, wikidbg_keyword_page_cb_t callback)	throw()
{
	// insert the element
	bool	succeed = page_db.insert(std::make_pair(keyword, callback)).second;
	DBG_ASSERT( succeed );
	// return the object itself
	return *this;
}

/** \brief Remove a keyword from the wikidbg_keyword_db_t
 */
wikidbg_keyword_db_t &	wikidbg_keyword_db_t::remove_page(const std::string &keyword)		throw()
{
	// sanity check - the keyword MUST be present in the database
	DBG_ASSERT( page_db.find(keyword) != page_db.end() );
	// remove this keyword from the database
	page_db.erase(keyword);
	// return the object itself
	return *this;
}

/** \brief Return the specific handler callback for a specific keyword, NULL if none is found
 */
wikidbg_keyword_page_cb_t	wikidbg_keyword_db_t::find_page(const std::string &keyword)	const throw()
{
	std::map<std::string, wikidbg_keyword_page_cb_t>::const_iterator	iter;
	// try to find the keyword in the database
	iter = page_db.find(keyword);
	// if the keyword is not found, return NULL
	if( iter == page_db.end() )	return NULL;
	// if the keyword is found, return the handler callback
	return iter->second;
}

NEOIP_NAMESPACE_END







