/*! \file
    \brief Implementation of \ref file_path_t to read directories
 
\par Brief Description
file_path_t implements a object to handle path. it may be file path
or url path. It handles unix file path and window file path too.

\par TODO
- TODO there is a crappy stuff about the way absolute/relative path
  are coded
  - for absolute path, the first name_db is empty
  - for relative path, it isnt
  - a LOT of special case due to that...
  - how to make it better ? 
    - like a special token for absolute, or relative in the first ?
- TODO the FS_DIR_SEPARATOR is used everywhere even if it is a unix/url constant
  not compatible with window
  - moreover, it is not even internal only
    - in the [] operator this is returned to the caller without knowing if it 
      is a window or unix/url path 

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
	ctor_from_str(path_ptr);
}

/** \brief Constructor from std::string
 */
file_path_t::file_path_t(const std::string &pathname)		throw()
{
	// if the pathname is empty, return now (and leave a null object)
	if( pathname.empty() )	return;
	// init the object
	ctor_from_str(pathname);
}

/** \brief Static function returning true IIF str may be a window 'driver letter'
 * 
 * - NOTE: the str MUST include the ':'
 * - TODO to replace all the 'colon' stuff by DRIVE_SEPA
 */
bool	file_path_t::is_drvletter(const std::string &str)			throw()
{
	// the minimal size for a drive letter is 2 - 1 for the drive letter and 1 for separator
	if( str.size() < 2 )			return false;
	
	// try to find the first colon
	size_t	sepa_pos	= str.find(':', 0);
	// if no separator has been found, it is not a drive letter
	if( sepa_pos == std::string::npos )	return false;
	// if the found separator is not the last character, it it not a driver letter
	if( sepa_pos != str.size()-1 )		return false;
	// if all previous tests passed, it is a drive letter
	return true;
}

/** \brief Init the object (aka the common part of all ctor)
 */
void file_path_t::ctor_from_str(const std::string &pathname)	throw()
{
	// log to debug
	KLOG_DBG("enter pathname=" << pathname);

	// extract the path from the string
	if( !pathname.empty() ){
		name_db = string_t::split(pathname, "/\\");
	}else{
		name_db = std::vector<std::string>();
	}

	// if the first name_db item contains a drive-letter, handle it 
	do {
		// if name_db[0] doesnt exist, there are no drive letter
		if( name_db.size() < 1 )		break;
		// try to find the  colon
		size_t	sepa_pos	= name_db[0].find(':', 0);
		// if no separator has been found, it is not a drive letter
		if( sepa_pos == std::string::npos )	break;
		// set up the drive letter
		m_drvletter	= name_db[0].substr(0, sepa_pos);
		// update the first name_db
		name_db[0]	= name_db[0].substr(sepa_pos+1);
	}while(0);

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
	// if this object is null, simply copy the object
	if( is_null() ){
		*this	= other;
		return *this;
	}

// TODO to handle the drvletter compatibility between the 2
// - sure but how ?
// - what are the compatibility rules :)

	// append the other name_db
	for(size_t i = 0; i < other.name_db.size(); i++)
		name_db.push_back( other.name_db[i] );
	
	// normalize the path
	normalize();

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
	// if there is a drvletter, the object is considered non null
	if( !drvletter().empty() )	return false;
	// if the name_db is NOT empty, the object is considered non null
	if( !name_db.empty() )		return false;
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
#if 1	// NOTE: attemps not to use the string conversion to be faster
	// - leaving the old version in case my attempt failed :)
	
	// sanity check - the nb_sub+1 MUST be < size()
	DBG_ASSERT( nb_sub < size() );
	// NOTE: some kludge about the is_absolute	
	if( is_absolute() )	nb_sub++;
	// copy this object into result - all object fields
	file_path_t	result	= *this;
	// special copy for the name_db field
	result.name_db	= std::vector<std::string>();
	for(size_t i = 0; i < nb_sub; i++)	result.name_db.push_back(name_db[i]);
	// normalize the result
	result.normalize();
	// return the result
	return result;
#else
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
#endif
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

/** \brief Return a string describing the file_path_t as a unix file path
 */
std::string	file_path_t::to_unxpath_string()	const throw()
{
#if 1
	// sanity check - the drvletter MUST be empty
	DBG_ASSERT( drvletter().empty() );
#else
	if( !drvletter().empty() )	KLOG_ERR("there is a driveletter when it should not!!!");
#endif
	// forward to the to_string_sepa for unix
	// - TODO put this in a constant
	return to_string_sepa("/");
}

/** \brief Return a string describing the file_path_t as a win32 file path
 */
std::string	file_path_t::to_winpath_string()	const throw()
{
	// forward to the to_string_sepa for win32
	// - TODO put this in a constant
	return to_string_sepa("\\");
}

/** \brief Return a string describing the file_path_t as a url file path
 */
std::string	file_path_t::to_urlpath_string()	const throw()
{
#if 1
	// sanity check - the drvletter MUST be empty
	DBG_ASSERT( drvletter().empty() );
#else
	if( !drvletter().empty() )	KLOG_ERR("there is a driveletter when it should not!!!");
#endif
	// forward to the to_string_sepa for url
	// - TODO put this in a constant
	return to_string_sepa("/");
}

/** \brief Return it as a path for the compilation target OS file path
 */
std::string	file_path_t::to_os_path_string()	const throw()
{
#ifndef	_WIN32
	return to_unxpath_string();
#else
	return to_winpath_string();
#endif
}

/** \brief Return a std::string for the file_path_t
 * 
 * - this is the default function, keep it conservative
 */
std::string	file_path_t::to_string() 		const throw()
{
	return to_string_sepa("/");	
}


/** \brief Convert the object into a string
 */
std::string	file_path_t::to_string_sepa(const std::string &dir_separator)	const throw()
{
	std::ostringstream 	oss;
	// add the drvletter if needed
	if( !drvletter().empty() )	oss << drvletter() << ":";
	// go thru all the names
	for( size_t i = 0; i < name_db.size(); i++ ){
		// display the name itself
		oss << name_db[i];
		if( i != name_db.size()-1 ){
			// add the intername dir_separator
			oss << dir_separator;
		}else if( name_db.size() == 1 && name_db[0].empty() ){
			// add the intername dir_separator
			oss << dir_separator;
		}
	}
	// return the result
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
serial_t& operator << (serial_t& serial, const file_path_t &file_path)	throw()
{
	// serialize each field of the object
	serial << datum_t(file_path.to_os_path_string());
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



