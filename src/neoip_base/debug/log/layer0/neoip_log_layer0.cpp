/*! \file
    \brief definition of the \ref log_layer_t

*/

/* system include */
#include <iostream>
#include <libgen.h>
/* local include */
#include "neoip_log_layer.hpp"
#include "neoip_property.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
log_layer_t::log_layer_t(const log_level_t &default_level)		throw()
{
	// sanity check - the default level MUST NOT be null
	DBG_ASSERT( !default_level.is_null() );
	// copy the parameter
	this->default_level	= default_level;
	// allocate the property
	prop_file	= nipmem_new property_t();
	prop_category	= nipmem_new property_t();
}

/** \brief Destructor
 */
log_layer_t::~log_layer_t()		throw()
{
	nipmem_zdelete prop_file;
	nipmem_zdelete prop_category;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           find_category_level
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return the log level of a category
 */
log_level_t log_layer_t::find_category_level(const char *category_name)		throw()
{
	log_level_t	log_level;

	// get the category level
	const std::string &level_str	= prop_category->find_string(category_name);
	//KLOG_STDOUT("level_str=" << level_str << "\n");

	// if no category_level is found, add one
	if( level_str.empty() )	log_level = create_category_level( category_name );
	else			log_level = log_level_t::from_string(level_str.c_str());

	//KLOG_STDOUT("find category level for " << category_name << " and return " << log_level.to_string() << "\n");

	// return the category level
	return log_level;
}

/** \brief return the log level of a category
 */
log_level_t log_layer_t::create_category_level(const std::string &category_name)	throw()
{
	// sanity check - the category level MUST NOT be already set
	DBG_ASSERT( prop_category->find_integer(category_name, -1) == -1 );

	// get the category level string from the file
	const std::string &level_str = prop_file->find_string_pattern(category_name, default_level.to_string());

	// insert the categery level	
	log_level_t	log_level = log_level_t::from_string_nocase(level_str);
	//KLOG_STDOUT("insert category " << category_name << " at level " << log_level.to_string() << "\n");
	prop_category->insert_string(category_name, log_level.to_string());

	// return the level
	return log_level;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  UTILITY function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief load a file containing property with log_level_t
 */
bool	log_layer_t::load_category_file(const std::string &filename)		throw()
{
	return prop_file->load_file(filename);
}

/** \brief log a message 
 */
void 	log_layer_t::do_log( const char *filename, int lineno, const char *fct_name
				, const char *category_name, log_level_t log_level
				, const std::string &str)			throw()
{
	// the category level is supposed to be checked in the macro for CPU efficiency purpose
	DBG_ASSERT( log_level <= find_category_level(category_name) );

	// extract the basename
	std::vector<std::string>	words		= string_t::split(filename, "/");
	std::string			basename	= words[words.size()-1];

	// actually log the line
	KLOG_STDOUT(basename << ":" << lineno << ":" << fct_name << "() " << str << "\n");
}


NEOIP_NAMESPACE_END







