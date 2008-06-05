/*! \file
    \brief Class to handle the xmlrpc_resp_t

*/

/* system include */
/* local include */
#include "neoip_xmlrpc_resp.hpp"
#include "neoip_xmlrpc_listener.hpp"
#include "neoip_xmlrpc_err.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
xmlrpc_resp_t::xmlrpc_resp_t()		throw()
{
	// zero some fields
	m_xmlrpc_listener	= NULL;
}

/** \brief Destructor
 */
xmlrpc_resp_t::~xmlrpc_resp_t()		throw()
{
	// unlink this object from the bt_cast_prefetch
	if( m_xmlrpc_listener )	m_xmlrpc_listener->resp_unlink(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation
 */
xmlrpc_err_t	xmlrpc_resp_t::start(xmlrpc_listener_t *m_xmlrpc_listener
				, xmlrpc_resp_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->m_xmlrpc_listener	= m_xmlrpc_listener;
	this->callback		= callback;
	this->userptr		= userptr;
	// link this object to the bt_cast_prefetch
	m_xmlrpc_listener->resp_dolink(this);
	
	// return no error
	return xmlrpc_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if this xmlrpc_resp_t contain this method_name
 */
bool	xmlrpc_resp_t::contain_method(const std::string &method_name)		const throw()
{
	std::list<std::string>::const_iterator	iter;
	// go thru the whole m_method_db
	for(iter = m_method_db.begin(); iter != m_method_db.end(); iter++){
		// if iterator matches the method_name, return true
		if( *iter == method_name )	return true;
	}
	// if this point is reached, no matches have been found
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Insert a method_name
 */
xmlrpc_resp_t &	xmlrpc_resp_t::insert_method(const std::string &method_name)	throw()
{
	// add the method_name into the m_method_db
	m_method_db.push_back(method_name);
	// return the object itself
	return *this;
}

/** \brief Delete a method_name
 */
xmlrpc_resp_t &	xmlrpc_resp_t::delete_method(const std::string &method_name)	throw()
{
	// add the method_name into the m_method_db
	m_method_db.remove(method_name);
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool xmlrpc_resp_t::notify_callback(const std::string &method_name, xmlrpc_err_t &err_out
		, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_xmlrpc_resp_cb(userptr, *this, method_name, err_out
							, xmlrpc_parse, xmlrpc_build);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





