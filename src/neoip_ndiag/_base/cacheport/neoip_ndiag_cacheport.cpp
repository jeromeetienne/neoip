/*! \file
    \brief Implementation of the ndiag_cacheport_t
    
*/

/* system include */
/* local include */
#include "neoip_ndiag_cacheport.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the ndiag_cacheport_t class
 */
ndiag_cacheport_t::ndiag_cacheport_t()		throw()
{
}

/** \brief Destructor
 */
ndiag_cacheport_t::~ndiag_cacheport_t()		throw()
{
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a pointer on a ndiag_cacheport_item_t matching the cacheport_id, or NULL if none matches
 */
const ndiag_cacheport_item_t *ndiag_cacheport_t::item_by_key(const std::string &item_key
					, const upnp_sockfam_t &item_sockfam)	const throw()
{
	// go thru the whole item_db
	for(size_t i = 0; i < item_db.size(); i++){
		const item_t &	item	= item_db[i];
		// if this item_key DOES NOT match, goto the next
		if( item.key() != item_key )		continue;
		// if this item_sockfam DOES NOT match, goto the next
		if( item.sockfam() != item_sockfam )	continue;
		// if this point is reached, the item DOES match, return it
		return &item;
	}	
	// if no matching item is found, return a null one
	return NULL;
}

/** \brief Return a const reference on the ndiag_cacheport_item_t matching this item_key
 * 
 * - WARNING: the item_key MUST be contained in the ndiag_cacheport_t
 */
const ndiag_cacheport_item_t &ndiag_cacheport_t::item(const std::string &item_key
					, const upnp_sockfam_t &item_sockfam)	const throw()
{
	// find a pointer on the matching item
	const ndiag_cacheport_item_t *	item	= item_by_key(item_key, item_sockfam);
	// sanity check - the item MUST have been found
	DBG_ASSERT( item );
	// return a reference on the found item
	return *item;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			update function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief update an item into the ndiag_cacheport_t
 */
ndiag_cacheport_t &ndiag_cacheport_t::update(const ndiag_cacheport_item_t &cacheport_item)		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// if there is one for the same std::string, remove it
	if( contain(cacheport_item.key(), cacheport_item.sockfam()) )
		remove(cacheport_item.key(), cacheport_item.sockfam());
	// add this item to the item_db_t
	item_db.append(cacheport_item);
	// return the object itself
	return *this;	
}

/** \brief remove the ndiag_cacheport_item_t maching this id
 * 
 * - WARNING: the item_key MUST be contained in the ndiag_cacheport_t
 */
ndiag_cacheport_t &ndiag_cacheport_t::remove(const std::string &item_key
					, const upnp_sockfam_t &item_sockfam)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// go thru the whole item_db
	size_t	i	= 0;
	for(i = 0; i < item_db.size(); i++){
		const item_t &	item	= item_db[i];
		// if this item_key DOES NOT match, goto the next
		if( item.key() != item_key )		continue;
		// if this item_sockfam DOES NOT match, goto the next
		if( item.sockfam() != item_sockfam )	continue;
		// if this point is reached, the item DOES match, remove it
		break;
	}
	// sanity check - the item MUST have been found
	DBG_ASSERT( i < item_db.size() );
	// remove the element and return
	item_db.remove(i);
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                  	to_string
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the address to a string
 */
std::string	ndiag_cacheport_t::to_string()				const throw()
{
	return item_db.to_string();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a ndiag_cacheport_t
 * 
 * - support null ndiag_cacheport_t
 */
serial_t& operator << (serial_t& serial, const ndiag_cacheport_t &ndiag_cacheport)throw()
{
	// serialize the object
	serial	<< ndiag_cacheport.item_db;
	// return serial
	return serial;
}

/** \brief unserialze a ndiag_cacheport_t
 * 
 * - support null ndiag_cacheport_t
 */
serial_t& operator >> (serial_t & serial, ndiag_cacheport_t &ndiag_cacheport)	throw(serial_except_t)
{
	// unserialize the object
	serial >> ndiag_cacheport.item_db;
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END






