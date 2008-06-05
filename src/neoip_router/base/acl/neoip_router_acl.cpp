/*! \file
    \brief Definition of the router_acl_t

*/

/* system include */
#include <iomanip>
#include <iostream>
#include <fstream>
/* local include */
#include "neoip_router_acl.hpp"
#include "neoip_file_path.hpp"
#include "neoip_string.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
router_acl_t::router_acl_t()	throw()
{

}

/** \brief Destructor
 */
router_acl_t::~router_acl_t()	throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the name if rejected, and false it is accepted
 */
bool	router_acl_t::reject(const std::string &name)	const throw()
{
	item_db_t::const_iterator	iter;
	// go thru the whole item_db_t
	for(iter = item_db.begin(); iter != item_db.end(); iter++){
		const router_acl_item_t & acl_item	= *iter;
		// if this acl_item.pattern() doesnt match the name, goto the next
		if( !string_t::glob_match(acl_item.pattern(), name) )	continue;
		// if this acl_item is ACCEPT, return false
		if( acl_item.type() == router_acl_type_t::ACCEPT )	return false;
		// else return true
		return true;
	}
	// if all previous tests passed, return true by default
	// - thus it is rejected if there is no match, which is 'allow only if explicitly specified'
	// - this is considered the safest policy for ACL
	return true;
}


/** \brief allow the [] operator to access in readonly the single itements
 */
const router_acl_item_t &	router_acl_t::operator[](size_t idx)	const throw()
{
	item_db_t::const_iterator	iter	= item_db.begin();
	// sanity check - idx MUST be < size()
	DBG_ASSERT( idx < size() );
	// go through the database
	std::advance(iter, idx);
	// return the value()
	return *iter;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			update function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief append a new item in the router_acl_t
 */
router_acl_t &	router_acl_t::append(const router_acl_type_t &type, const std::string &pattern)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// put the router_acl_item_t into the item_db
	item_db.push_back( router_acl_item_t().type(type).pattern(pattern) );
	// return this object
	return *this;
}

/** \brief Load a file and append its content into the router_act_t
 */
router_err_t	router_acl_t::load_file(const file_path_t &file_path)		throw()
{
	size_t		line_num	= 0;
	std::ifstream 	file_in;
	// open the file
	try {
		file_in.open( file_path.to_string().c_str(), std::ios::in );
	}catch(...){
		return router_err_t(router_err_t::ERROR, "Cant open filename " + file_path.to_string());
	}
	// read until the end of the file
	while( file_in.good() ){
		char	line_raw[1024+1];
		// read one file
		file_in.getline( line_raw, sizeof(line_raw)-1 );
		// update the line_num
		line_num++;
		// convert it to a std::string
		std::string line	= string_t::strip(line_raw);
		// if the line start with a '#', it is a comment, skip it
		if( line[0] == '#' )	continue;
		// if the line is empty, skip it
		if( line.size() == 0 )	continue;
		// split the line into words
		std::vector<std::string> words	= string_t::split(line, " ");
		// if words.size() != 2, this is a syntax error, return it
		if( words.size() != 2 ){
			std::string	reason	= "Invalid router_acl_item_t syntax at line " + OSTREAMSTR(line_num);
			return router_err_t(router_err_t::ERROR, reason);
		}
		// convert the line into type
		router_acl_type_t	type	= router_acl_type_t::from_string_nocase(words[0]);
		std::string		pattern	= words[1];
		// check if the router_acl_type_t has been correctly parsed
		if( type.is_null() ){
			std::string	reason	= "Invalid router_acl_item_t syntax at line " + OSTREAMSTR(line_num);
			return router_err_t(router_err_t::ERROR, reason);
		}
		// append this item into the router_acl_t
		append(type, pattern);
	}
	// return no error
	return router_err_t::OK;
}


NEOIP_NAMESPACE_END

