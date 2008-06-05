/*! \file
    \brief Declaration of the wikidbg_global_db_t

\par Implementation Notes
This module is fully independant (aka no use of nipmem_new/delete or http stuff)
to avoid any recursive issue during the initialisation.

\par Possible improvement - remove the need for wikidbg_obj_t
- if the wikidbg function are template, one can know the type of the pointer
- one can use this to determine the wikidbg handler function
- thus one needs only to register all the wikidbg handler function per type
- and use this to get the whole without any overhead per object
- PRO: this reduce the cpu overhead
- PRO: this allow a more flexible scheme as compilation of wikidbg handler and object itself
       is fully separated
- PRO: this remove the uglyness in the object declaration
- CON: none :)

*/

/* system include */
/* local include */
#include "neoip_wikidbg_global_db.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief The global database containing all the object with their associated callback
 */
static std::map<void *, wikidbg_init_cb_t> *	wikidbg_cback_db	= NULL;

/** \brief The global database containing all the alias pointer with the canonical object ptr
 * 
 * - used for all the virtual inheritance pointer - used a lot in the many callback and vapi
 */
static std::map<void *, void *> *		wikidbg_alias_db	= NULL;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  insert
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief insert a new object in the wikidbg_global_db_t
 */
void wikidbg_global_db_t::insert(void *object_ptr, wikidbg_init_cb_t callback
						, const std::list<void *> &alias_ptr_db)	throw()
{
	// build the database if not already done
	if( !wikidbg_cback_db ){
		DBG_ASSERT( wikidbg_alias_db == NULL );	
		wikidbg_cback_db	= new std::map<void *, wikidbg_init_cb_t>();
		wikidbg_alias_db	= new std::map<void *, void *>();		
	}
	// insert the element
	bool	succeed = wikidbg_cback_db->insert(std::make_pair(object_ptr, callback)).second;
	DBG_ASSERT( succeed );
	// insert all alias from the alias_ptr_cb
	std::list<void *>::const_iterator	iter;
	for(iter = alias_ptr_db.begin(); iter != alias_ptr_db.end(); iter++){
		bool	succeed = wikidbg_alias_db->insert(std::make_pair(*iter, object_ptr)).second;
		DBG_ASSERT( succeed );
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         remove
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Remove an object from the wikidbg_global_db_t
 */
void wikidbg_global_db_t::remove(void *object_ptr, const std::list<void *> &alias_ptr_db)	throw()
{
	// sanity check - the global wikidbg_cback_db MUST contains the object
	DBG_ASSERT( wikidbg_cback_db->find(object_ptr) != wikidbg_cback_db->end() );
	// remove this object from the map
	wikidbg_cback_db->erase(object_ptr);
	// remote all alias from the alias_ptr_cb
	std::list<void *>::const_iterator	iter;
	for(iter = alias_ptr_db.begin(); iter != alias_ptr_db.end(); iter++){
		DBG_ASSERT( wikidbg_alias_db->find(*iter) != wikidbg_alias_db->end() );
		wikidbg_alias_db->erase(*iter);
	}
	// if the database if now empty, free it
	if( wikidbg_cback_db->empty() ){
		DBG_ASSERT( wikidbg_alias_db->empty() );
		delete wikidbg_cback_db;
		delete wikidbg_alias_db;
		wikidbg_cback_db	= NULL;
		wikidbg_alias_db	= NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  find
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the wikidbg_init_cb_t associated with this object_ptr (or NULL
 *         if none is present)
 */
wikidbg_init_cb_t wikidbg_global_db_t::find(void *object_ptr)			throw()
{
	std::map<void *, wikidbg_init_cb_t>::const_iterator	iter_cback;
	// if the database is not allocated, return NULL
	if( wikidbg_cback_db == NULL )			return NULL;
	// try to find the object_ptr in the database
	iter_cback	= wikidbg_cback_db->find(object_ptr);
	// if the object is not found, return NULL
	if( iter_cback == wikidbg_cback_db->end() )	return NULL;
	// else return the callback associated with it
	return iter_cback->second;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Return a canonical_ptr from this object_ptr
 * 
 * - aka if it is an alias, convert it else dont touch it
 */
static void *	get_canonical_ptr(void *object_ptr)
{
	std::map<void *, void *>::const_iterator		iter_alias;
	// try to find the object_ptr in the alias database
	iter_alias	= wikidbg_alias_db->find(object_ptr);
	// if the object_ptr is NOT in the alias database, return it as itreplace it by the canonical one
	if( iter_alias == wikidbg_alias_db->end() )	return object_ptr;
	// else if the object_ptr is an alias, return the canonical one from the database
	return iter_alias->second;
}

/** \brief Return the wikidbg_keyword_html_cb_t associated with keyword/canonical_ptr if any.
 *         NULL otherwise
 */
static wikidbg_keyword_html_cb_t get_keyword_html_cb(const std::string &keyword, void *canonical_ptr)	throw()
{
	wikidbg_keyword_db_t keyword_db;
	// find the wikidbg_init_cb for this canonical_ptr
	wikidbg_init_cb_t	wikidbg_init_cb = wikidbg_global_db_t::find(canonical_ptr);
	// if the wikidbg_init_cb, is not found, return a httpd_err_t::NOT_FOUND
	if( wikidbg_init_cb == NULL )	return 	NULL;
	// get the keyword database for this pointer
	wikidbg_init_cb(keyword_db);
	// try to find the specified 
	return keyword_db.find_html(keyword);	
}

/** \brief Return the wikidbg_keyword_page_cb_t associated with keyword/canonical_ptr if any.
 *         NULL otherwise
 */
static wikidbg_keyword_page_cb_t get_keyword_page_cb(const std::string &keyword, void *canonical_ptr)	throw()
{
	wikidbg_keyword_db_t keyword_db;
	// find the wikidbg_init_cb for this canonical_ptr
	wikidbg_init_cb_t	wikidbg_init_cb = wikidbg_global_db_t::find(canonical_ptr);
	// if the wikidbg_init_cb, is not found, return a NULL
	if( wikidbg_init_cb == NULL )	return 	NULL;
	// get the keyword database for this pointer
	wikidbg_init_cb(keyword_db);
	// try to find the specified 
	return keyword_db.find_page(keyword);	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          get function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief return the html associted with this keyword/object_ptr
 */
std::string wikidbg_global_db_t::get_keyword_html(const std::string &keyword, void *object_ptr)	throw()
{
	void *				canonical_ptr	= get_canonical_ptr(object_ptr);
	wikidbg_keyword_html_cb_t	html_cb		= get_keyword_html_cb(keyword, canonical_ptr);
	// if the object_ptr is NULL, display it
	if( object_ptr == NULL )	return "NULL pointer";
	// if the wikidbg_init_cb, is not found, return a well visible ERROR on the web page
	if( html_cb == NULL )		return "WIKIDBG_HANDLER_NOT_FOUND";

	// if the handler is found, call it and return the result
	return html_cb(keyword, canonical_ptr);
}

/** \brief return the page associted with this keyword/object_ptr
 */
httpd_err_t wikidbg_global_db_t::get_keyword_page(const std::string &keyword, void *object_ptr
							, httpd_request_t &httpd_request)	throw()
{
	void *				canonical_ptr	= get_canonical_ptr(object_ptr);
	wikidbg_keyword_page_cb_t	page_cb		= get_keyword_page_cb(keyword, canonical_ptr);
	// if the wikidbg_init_cb, is not found, return a httpd_err_t::NOT_FOUND
	if( page_cb == NULL )		return httpd_err_t::NOT_FOUND;
	// if the handler is found, call it and return the result
	return page_cb(keyword, canonical_ptr, httpd_request);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          contain function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if the keyword/object_ptr match a html callback, false otherwise
 */
bool wikidbg_global_db_t::contain_keyword_html(const std::string &keyword, void *object_ptr)	throw()
{
	void *		canonical_ptr	= get_canonical_ptr(object_ptr);	
	if( get_keyword_html_cb(keyword, canonical_ptr) )	return true;
	return false;
}

/** \brief return true if the keyword/object_ptr match a page callback, false otherwise
 */
bool wikidbg_global_db_t::contain_keyword_page(const std::string &keyword, void *object_ptr)	throw()
{
	void *		canonical_ptr	= get_canonical_ptr(object_ptr);	
	if( get_keyword_page_cb(keyword, canonical_ptr) )	return true;
	return false;
}

NEOIP_NAMESPACE_END







