/*! \file
    \brief Implementation of \ref file_path_t to read directories
 
*/

/* system include */
/* local include */
#include "neoip_file_path.hpp"
#include "neoip_file_utils.hpp"
#include "neoip_string.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN;

// definition of \ref nunit_path_t constant
const std::string	file_path_t::FS_DIR_SEPARATOR	= "/";
const std::string	file_path_t::FS_DIR_PLACEHOLDER	= ".";
const std::string	file_path_t::FS_DIR_PARENT	= "..";
// end of constants definition

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor from a char *
 */
file_path_t::file_path_t(const char * path_ptr)		throw()
{
	// if the path_ptr is NULL, return now (and leave a null object)
	if( !path_ptr )	return;
	// init the object
	init(path_ptr);
}

/** \brief Constructor from std::string
 */
file_path_t::file_path_t(const std::string &pathname)		throw()
{
	// if the pathname is empty, return now (and leave a null object)
	if( pathname.empty() )	return;
	// init the object
	init(pathname);
}

/** \brief Init the object (aka the common part of all ctor)
 */
void file_path_t::init(const std::string &pathname)	throw()
{
	// extract the path
	if( pathname.empty() )	name_db = std::vector<std::string>();
	else			name_db = string_t::split(pathname, FS_DIR_SEPARATOR);
	// normalize the path
	normalize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      normalization function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief normalize the file_path_t
 */
void	file_path_t::normalize()	throw()
{
	std::vector<std::string>::iterator		iter;
	
	// remove all FS_DIR_PLACEHOLDER
	do{
		// look for a FS_DIR_PLACEHOLDER
		for(iter = name_db.begin(); iter != name_db.end() && *iter != FS_DIR_PLACEHOLDER; iter++);
		// if one has been found, remove it
		if( iter != name_db.end() )	name_db.erase( iter );
		// if none has been found, exit
	}while( iter != name_db.end() );
	
	// remove *redundant* FS_DIR_SEPARATOR
	do{
		// if the name_db is less than 2 element - to leave the head FS_DIR_SEPARATOR
		if( name_db.size() < 2 )	break;
		for( iter = name_db.begin()+1; iter != name_db.end() && !(*iter).empty(); iter++ );
		// if one has been found, remove it
		if( iter != name_db.end() )	name_db.erase( iter );
		// if none has been found, exit
	}while( iter != name_db.end() );

	// remove all *non-empty* name followed by FS_DIR_PARENT
	do{
		// loop thru the whole name_db
		for( iter = name_db.begin(); iter != name_db.end() ; iter++ ){
			// if this name is empty, continue
			if( iter->empty() )			continue;
			// if this name is already a FS_DIR_PARENT, continue
			if( *iter == FS_DIR_PARENT )		continue;
			// if there are no name after this one, continue
			if( iter+1 == name_db.end() )		continue;
			// if the next name is NOT a FS_DIR_PARENT, continue;
			if( *(iter+1) != FS_DIR_PARENT )	continue;
			// remote this name
			name_db.erase(iter, iter+2);
			// leave the loop now - it is required as it is a usual 'modify while walking'
			break;
		}
		// if none has been found, exit
	}while( iter != name_db.end() );
	
	// it it is a absolute path, remove any FS_DIR_PARENT which follow it
	while( !name_db.empty() && name_db.begin()->empty() && name_db.size() > 1 ){
		iter = name_db.begin() + 1;
		if( *iter != FS_DIR_PARENT )	break;
		name_db.erase(iter);
	}

	// if the name_db is empty, add a single FS_DIR_PLACEHOLDER
	if( name_db.empty() )	name_db.push_back(FS_DIR_PLACEHOLDER);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      append function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Append a file_path_t to another
 */
file_path_t &	file_path_t::operator /=(const file_path_t &other)	throw()
{
	// build the new_path
	std::string	new_path;
	if( !this->is_null() ){
		// if this file_path_t is not null, append the other
		new_path = this->to_string() + FS_DIR_SEPARATOR + other.to_string();
	}else{	// if this file_path_t is null, just copy the other
		new_path = other.to_string();
	}
	
	// copy the new_path in this object
	// - no need to normalize as a string is returned and immediatly converted in a full_path_t
	//   so constructor is called and normalize
	*this	= new_path.c_str();
	
	// return the object itself
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      UTILITY function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is considered null, false otherwise
 */
bool	file_path_t::is_null()		const throw()
{
	// if the name_db is NOT empty, the object is considered non null
	if( !name_db.empty() )	return false;
	// else it is considered null
	return true;
}

/** \brief Return true if the file_path_t is absolute, false otherwise
 */
bool	file_path_t::is_absolute()	const throw()
{
	// sanity check - the path MUST be initialized
	DBG_ASSERT( !is_null() );
	
	// if the first name is empty, it is an absolute path
	if( name_db[0] == "" )	return true;
	// else it isnt
	return false;	
}

/** \brief Return true if this file_path_t contains of the other file_path_t
 * 
 * - it return true is if this file_path_t and the other one are equal or the 
 *   other one is a subpath of this one
 *   - e.g. /tmp/bla is a subpath of /tmp
 */
bool	file_path_t::contain(const file_path_t &other)	const throw()
{
	// if any of the two file_path_t is null, return false
	if( is_null() || other.is_null() )	return false;
	// if the other file_path_t has less name than this one, return false
	if( other.size() < size() )		return false;
	// go thru all the element of this file_path_t
	// - purposely ignoring any additionnaly elements in the other file_path_t
	for(size_t i = 0; i < size(); i++){
		// if this element is different from the other one, return false
		if( name_db[i] != other.name_db[i] )	return false;
	}
	// if this point is reached, return true
	return true;
}

/** \brief Return the extension of the basename
 * 
 * - NOTE: here an extension is the string between the end of the basename
 *   and the last dot of the basename
 *   - if the basename contains no dot, it returns a empty string
 */
std::string	file_path_t::basename_ext()	const throw()
{
	// sanity check - the file_path_t MUST NOT be null
	DBG_ASSERT( !is_null() );
	// Extract the basename
	const std::string &	basename	= name_db[name_db.size()-1];
	// find the last '.' occurence in basename
	size_t			pos		= basename.rfind(".");
	// if no occurence is found, return an empty string
	if( pos == std::string::npos )	return "";
	// else return the whole string after the found "."
	return basename.substr(pos+1);
}

/** \brief Return the basename WITHOUT the extension
 * 
 * - NOTE: here an extension is the string between the end of the basename
 *   and the last dot of the basename
 *   - if the basename contains no dot, it returns a empty string
 */
std::string	file_path_t::basename_noext()	const throw()
{
	// sanity check - the file_path_t MUST NOT be null
	DBG_ASSERT( !is_null() );
	// Extract the basename
	const std::string &	basename	= name_db[name_db.size()-1];
	// find the last '.' occurence in basename
	size_t			pos		= basename.rfind(".");
	// if no occurence is found, return an empty string
	if( pos == std::string::npos )	return "";
	// else return the whole string after the found "."
	return basename.substr(0, pos);
}

/** \brief Return the basename
 */
file_path_t	file_path_t::basename()	const throw()
{
	// sanity check - the file_path_t MUST NOT be null
	DBG_ASSERT( !is_null() );
	// Extract the basename
	return name_db[name_db.size()-1];
}

/** \brief Return the dirname
 */
file_path_t	file_path_t::dirname()	const throw()
{
	// sanity check - the file_path_t MUST NOT be null
	DBG_ASSERT( !is_null() );
	// if this file_path_t contain only a directory, return it as is
	if( name_db.size() <= 1 )	return *this;
	// return the dirname
	if( is_absolute() )	return this->subpath(name_db.size()-2);
	return this->subpath(name_db.size()-1);
}

/** \brief Return the fullpath of this file_path_t
 * 
 * - WARNING: this function assume that the file_path_t is relative to the 
 *            *CURRENT* directory !
 *   - so if the file_path_t is created for with a relative path with a given 
 *     current directory, then the current directory is changed, and then
 *     fullpath() is called, it will return the full path in the SECOND
 *     current directory
 */
file_path_t	file_path_t::fullpath()		const throw()
{
	// if this file_path_t is already 'absolute', return it as is
	if( is_absolute() )	return *this;
	// sanity check - here the 
	DBG_ASSERT( is_relative() );
	// prepend the current directory to this relative path
	return file_utils_t::get_current_dir() / *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Level function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return a sub path of this file_path_t with nb_sub name
 */
file_path_t	file_path_t::subpath(size_t nb_sub)	const throw()
{
	// sanity check - the nb_sub MUST be < size()
	DBG_ASSERT( nb_sub < size() );
	// build the string
	std::ostringstream 	oss;
	for( size_t i = 0; i <= nb_sub; i++ ){
		if( i != 0 )	oss << FS_DIR_SEPARATOR;
		oss << (*this)[i];
	}
	// return and convert and normalize the file_path_t
	return oss.str();
}

/** \brief return the number of level of this file_path_t
 */
size_t	file_path_t::size()			const throw()
{
	if( is_relative() ){
		if( name_db[0] == FS_DIR_PLACEHOLDER )	return name_db.size();
		return name_db.size() + 1;
	}
	return name_db.size();
}

/** \brief return the name at idx in this file_path_t
 */
file_path_t	file_path_t::operator[](size_t idx)	const throw()
{
	if( is_relative() ){
		if( idx == 0 )	return FS_DIR_PLACEHOLDER;
		if( name_db[0] != FS_DIR_PLACEHOLDER )	idx--;
	}else{
		if( idx == 0 )	return FS_DIR_SEPARATOR;
	}
	return name_db[idx];
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare functions
 */
int file_path_t::compare(const file_path_t &other)	const throw()
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
std::string	file_path_t::to_string()	const throw()
{
	std::ostringstream 	oss;
	// go thru all the names
	for( size_t i = 0; i < name_db.size(); i++ ){
		// display the name itself
		oss << name_db[i];
		if( i != name_db.size()-1 ){
			// add the intername FS_DIR_SEPARATOR
			oss << FS_DIR_SEPARATOR;
		}else if( name_db.size() == 1 && name_db[0].empty() ){
			// add the root directory FS_DIR_SEPARATOR
			oss << FS_DIR_SEPARATOR;
		}
	}
	return oss.str();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a file_path_t
 * 
 * - support null file_path_t
 */
serial_t& operator << (serial_t& serial, const file_path_t &file_path)		throw()
{
	// serialize each field of the object
	serial << datum_t(file_path.to_string());
	// return serial
	return serial;
}

/** \brief unserialze a file_path_t
 * 
 * - support null file_path_t
 */
serial_t& operator >> (serial_t & serial, file_path_t &file_path)		throw(serial_except_t)
{	
	datum_t		file_path_datum;
	// unserialize the data
	serial >> file_path_datum;
	// set the returned variable
	file_path	= file_path_datum.to_stdstring();
	// return serial
	return serial;
}

NEOIP_NAMESPACE_END



