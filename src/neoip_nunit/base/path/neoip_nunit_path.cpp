/*! \file
    \brief Implementation of \ref nunit_path_t to read directories
 
*/

/* system include */
/* local include */
#include "neoip_nunit_path.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN;

// definition of \ref nunit_path_t constant
const std::string	nunit_path_t::NAME_SEPARATOR	= "/";
// end of constants definition

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor from a char *
 */
nunit_path_t::nunit_path_t(const char * path_ptr)		throw()
{
	// if the path_ptr is NULL, return now (and leave a null object)
	if( !path_ptr )	return;
	// init the object
	init(path_ptr);
}

/** \brief Constructor from std::string
 */
nunit_path_t::nunit_path_t(const std::string &pathname)		throw()
{
	// if the pathname is empty, return now (and leave a null object)
	if( pathname.empty() )	return;
	// init the object
	init(pathname);
}

/** \brief Init the object (aka the common part of all ctor)
 */
void nunit_path_t::init(const std::string &pathname)	throw()
{
	// log to debug
	KLOG_DBG("***************enter pathname=" << pathname);
	
	// extract the path
	if( pathname.empty() )	name_db = std::vector<std::string>();
	else			name_db = string_t::split(pathname, NAME_SEPARATOR);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      append function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Append a nunit_path_t to another
 */
nunit_path_t &	nunit_path_t::operator /=(const nunit_path_t &other)	throw()
{
	// add the name in the database
	for( size_t i = 0; i < other.size(); i++ )
		name_db.push_back(other[i]);

	// return the object itself
	return *this;
}


/** \brief Return true if the object is considered null, false otherwise
 */
bool	nunit_path_t::is_null()		const throw()
{
	// if the name_db is NOT empty, the object is considered non null
	if( !name_db.empty() )	return false;
	// else it is considered null
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Level function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return a name at the idx of this nunit_path_t
 */
const std::string &nunit_path_t::operator[](size_t idx)	const throw()
{
	// sanity check - the idx MUST be < size()
	DBG_ASSERT( idx < size() );
	// return the name at the idx position
	return name_db[idx];
}

/** \brief Return the sub-path of this nunit_path_t
 */
nunit_path_t	nunit_path_t::get_subpath()		const throw()
{
	nunit_path_t	result;
	// build the sub-path
	for( size_t i = 1; i < name_db.size(); i++ )
		result /= name_db[i];
	// return the sub-path
	return result;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int nunit_path_t::compare(const nunit_path_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null

	// compare the name_db
	if( name_db < other.name_db )		return -1;
	if( name_db > other.name_db )		return +1;
	// NOTE: here both name_db are equal

	// here both are considered equal
	return 0;
}
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     to_string() function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Convert the object into a string
 */
std::string	nunit_path_t::to_string()	const throw()
{
	std::ostringstream 	oss;
	// go thru all the names
	for( size_t i = 0; i < name_db.size(); i++ ){
		// add the intername NAME_SEPARATOR
		if( i != 0 )	oss << NAME_SEPARATOR;
		// display the name itself
		oss << name_db[i];
	}
	// return the built string
	return oss.str();
}

NEOIP_NAMESPACE_END



