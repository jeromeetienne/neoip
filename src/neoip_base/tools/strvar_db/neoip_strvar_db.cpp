/*! \file
    \brief Declaration of the strvar_db_t

*/

/* system include */
/* local include */
#include "neoip_strvar_db.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

const size_t	strvar_db_t::INDEX_NONE	= std::numeric_limits<size_t>::max();

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
strvar_db_t::strvar_db_t()	throw()
{
	// zero some variable
	m_key_ignorecase	= false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 keys ala memcmp - takeing the key_ignorecase into account
 */ 
int	strvar_db_t::keycmp(const std::string &key1, const std::string &key2)	const throw()
{
	// if key_ignorecase() is set, compare them without case
	if( key_ignorecase() )	return string_t::casecmp(key1, key2);
	// else compare with case
	if( key1 < key2 )	return -1;
	if( key1 > key2 )	return +1;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the idx of the next occurence of this key
 * 
 * @return std::numeric_limits<size_t>::max() if none have been found
 */
size_t	strvar_db_t::next_key_idx(const std::string &key, size_t prev_idx)	const throw()
{
	size_t	first_idx;
	// compute the first_idx to test
	if( prev_idx != INDEX_NONE )	first_idx = prev_idx+1;
	else				first_idx = 0;
	// go thru the whole inherited item_arr_t
	for(size_t i = first_idx; i < size(); i++){
		const strvar_item_t &	item	= item_arr_t<strvar_item_t>::operator[](i);
		// log to debug
		KLOG_DBG("idx=" << i << " looking for " << key << " got " << item.key() );
		// if the key matches the one in the parameter, return this index
		if( keycmp(item.key(), key) == 0 )	return i;
	}
	// if this key has not been found, return INDEX_NONE
	return INDEX_NONE;
}

/** \brief Return the value of the first strvar_item_t with the same key
 */
const std::string &	strvar_db_t::get_first_value(const std::string &key
					, const std::string &default_value)	const throw()
{
	// get the key_idx for the first occurence of this key
	size_t	key_idx	= first_key_idx(key);
	// if the key has not been found, return the default_value
	if( key_idx == INDEX_NONE )	return default_value;
	// return the value
	const strvar_item_t &	item	= item_arr_t<strvar_item_t>::operator[](key_idx);
	return item.val();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Append a new occurence of a strvar_item_t for this key/val
 */
strvar_db_t &	strvar_db_t::append(const std::string &key, const std::string &val)	throw()
{
	// append it to the inherited item_arr_t
	item_arr_t<strvar_item_t>::append(strvar_item_t(key, val));
	// return the object itself
	return *this;
}

/** \brief Update the strvar_db_t 
 * 
 * - if there is already a 
 */
strvar_db_t &	strvar_db_t::update(const std::string &key, const std::string &val)	throw()
{
	// try to get the first occurence of a variable with this key
	size_t	key_idx	= first_key_idx(key);
	// if none have been found, this update is in fact an append
	if( key_idx == INDEX_NONE )	return append(key,val);
	// get the found item
	strvar_item_t &	item	= item_arr_t<strvar_item_t>::operator[](key_idx);
	// sanity check - the item key MUST be the same as the parameter one
	DBG_ASSERT( keycmp(item.key(), key) == 0 );
	// modify the value of this item with the new one
	item.val(val);
	// return the object itself
	return *this;
}



NEOIP_NAMESPACE_END







