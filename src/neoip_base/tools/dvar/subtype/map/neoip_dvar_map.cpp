/*! \file
    \brief Declaration of the dvar_map_t
*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_dvar_map.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief clone this object
 */
dvar_vapi_t *	dvar_map_t::clone()	const throw()
{
	return nipmem_new dvar_map_t(*this);
}

/** \brief Insert a new dvar_t in the dvar_map_t
 */
dvar_map_t &	dvar_map_t::insert(const std::string &key, const dvar_t &dvar)	throw()
{
	// sanity check - the key MUST NOT be contained in the dvar_map_t
	DBG_ASSERT( !contain(key) );
	// insert the new variable in the map
	bool succeed = map.insert(std::make_pair(key, dvar)).second;
	DBG_ASSERT( succeed );
	// return the object itself
	return *this;
}

/** \brief Return true if the key is contained in the dvar_map_t and if it is of a particular dvar_type_t
 */
bool	dvar_map_t::contain(const std::string &key, const dvar_type_t &dvar_type)	const throw()
{
	std::map<std::string, dvar_t>::const_iterator	iter;
	// try to find the key in this map
	iter	= map.find(key);
	// if the key is not in the map, return false
	if( iter == map.end() )							return false;
	// if the dvar_type_t is specified and if the contained one is different, return false
	if( dvar_type != dvar_type_t::NONE && iter->second.type() != dvar_type)	return false;
	// if the key is in the map, return true
	return true;
}

/** \brief operator [] for dvar_map_t
 */
const dvar_t &	dvar_map_t::operator[](const std::string &key)	const throw()
{
	// sanity check - the key MUST be contained in the dvar_map_t
	DBG_ASSERT( contain(key) );
	// return the object
	return map.find(key)->second;
}

/** \brief operator [] for dvar_map_t
 */
dvar_t &	dvar_map_t::operator[](const std::string &key)	throw()
{
	// sanity check - the key MUST be contained in the dvar_map_t
	DBG_ASSERT( contain(key) );
	// return the object
	return map.find(key)->second;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      compare() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Compare the object ala memcmp
 */
int 	dvar_map_t::compare(const dvar_vapi_t &other_vapi)	const throw()
{
	// sanity check - check that other_vapi is of the same dvar_type_t
	DBG_ASSERT( other_vapi.type() == type() );
	// convert the other_vapi
	const dvar_map_t &	other	= dynamic_cast <const dvar_map_t &>(other_vapi);
	// compare the value
	if( map < other.map )	return -1;
	if( map > other.map )	return +1;
	// here both are considered equal
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	dvar_map_t::to_string()	const throw()
{
	std::ostringstream				oss;
	std::map<std::string, dvar_t>::const_iterator	iter;
	// put the begining of the map
	oss << "{";
	// put each element of the map
	for(iter = map.begin(); iter != map.end(); iter++ ){
		const std::string &	key	= iter->first;
		const dvar_t		dvar	= iter->second;
		oss << "[\"" << key << "\"=" << dvar << "]";
	}
	// put the end of the map
	oss << "}";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END







