/*! \file
    \brief Declaration of the property_t

*/

/* system include */
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
/* local include */
#include "neoip_property.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                             load_file
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief load properties from a file to the database
 * 
 * @return true on error, false otherwise
 */
bool property_t::load_file(const std::string &filename) throw()
{
	char		line[1024+1];
	std::ifstream 	file_in;
	// open the file
	try {
		file_in.open( filename.c_str(), std::ios::in );
	} catch(...){
		KLOG_ERR("Cant open filename " + filename);
		return true;
	}
		
	while( file_in.good() ){
		file_in.getline( line, sizeof(line)-1 );
		// if the line start with a '#', it is a comment, skip it
		if( line[0] == '#' )	continue;
		// if the line is empty, skip it
		if( strlen(line) == 0 )	continue;
		char	*p_equal = strchr(line, '=');
		DBG_ASSERT( p_equal );
		// replace '=' by '\0'
		*p_equal = '\0';
		insert_string( line, p_equal+1 );
	}
	return false;
}

/** \brief dump the whole database to stdout (mainly for debug)
 */
void property_t::dump_dbg()		const throw()
{
	KLOG_STDOUT(*this);
}

/** \brief return a string containing all the property
 */
std::string property_t::to_string()	const throw()
{
	std::map<std::string, var_t>::const_iterator	iter;
	std::ostringstream				oss;
	oss << "# this property contains " << var_db.size() << " variables.";
	for( iter = var_db.begin(); iter != var_db.end(); iter++ ){
		const var_t &	var = iter->second;
		oss << var.name << " = " << var.value << "\n";
	}
	return oss.str();
}

/** \brief find a string property and return its value
 */
const property_t::var_t *property_t::find_var(const std::string &name)			const throw()
{
	std::map<std::string, var_t>::const_iterator	iter;
	iter = var_db.find(name);
	if( iter == var_db.end() )	return NULL;
	return &(iter->second);
}

/** \brief insert a new variable in the database
 * 
 * @return true if the insert failed, false otherwise
 */
bool	property_t::insert_var(const std::string &name, const std::string &value)	throw()
{
	bool succeed = var_db.insert(std::make_pair(name, var_t(name, value))).second;
	if( succeed )	return false;
	return true;
}

/** \brief return true if the variable name exists in the database, false otherwise
 */
bool	property_t::exist(const std::string &name)				const throw()
{
	const var_t *	var = find_var(name);
	if( !var )	return false;
	return true;
}

/** \brief remove a variable by its name
 */
void	property_t::remove(const std::string &name)				throw()
{
	var_db.erase(name);
}

/** \brief Insert all the variable of the other property_t into this one
 * 
 * @return true if at least one variable insertion failed, false otherwise
 */
bool	property_t::insert(const property_t &other)		throw()
{
	std::map<std::string, var_t>::const_iterator	iter;
	bool	failed = false;
	for( iter = other.var_db.begin(); iter != other.var_db.end(); iter++ ){
		const var_t &	var = iter->second;
		// insert one other variable
		if( insert_var(var.name, var.value) )
			failed = true;
	}
	return failed;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          find/insert STRING
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief find a string property and return its value
 */
const std::string &property_t::find_string(const std::string &name
						, const std::string &default_value)
					const throw()
{
	// try to get the variable
	const var_t *	var = find_var(name);
	// if no name match, return the default value
	if( var == NULL )	return default_value;
	// else return the variable value
	return var->value;
}

/** \brief find a string value of the first property pattern matching the provided name
 */
const std::string &property_t::find_string_pattern(const std::string &name
						, const std::string &default_value)
					const throw()
{
	std::map<std::string, var_t>::const_iterator	iter;
	for( iter = var_db.begin(); iter != var_db.end(); iter++ ){
		const var_t &	var	= iter->second;
		if( string_t::glob_match(var.name, name) )	return var.value;
	}
	// if no name match, return the default value
	return default_value;
}

/** \brief add a string property
 */
bool	property_t::insert_string(const std::string &name, const std::string &value)
					throw()
{
	return insert_var(name, value);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          find/insert INTEGER
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief find a integer property and return its value
 */
int property_t::find_integer(const std::string &name, int default_value)	const throw()
{
	// try to get the variable
	const var_t *	var = find_var(name);
	// if no name match, return the default value
	if( var == NULL )	return default_value;

	// convert the string into an integer
	std::istringstream	iss(var->value);
	int			value	= default_value;
	iss >> value;
	// else return the variable value
	return value;
}

/** \brief add a integer property
 */
bool property_t::insert_integer(const std::string &name, int value )		throw()
{
	// convert the integer into a string
	std::ostringstream	oss;
	oss << value;
	// add the property in the list
	return insert_var(name, oss.str());
}

NEOIP_NAMESPACE_END







