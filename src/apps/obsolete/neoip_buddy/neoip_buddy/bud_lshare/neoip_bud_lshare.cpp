/*! \file
    \brief Definition of the \ref bud_lshare_t class

*/

/* system include */
/* local include */
#include "neoip_bud_lshare.hpp"
#include "neoip_file_dir.hpp"
#include "neoip_property.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bud_lshare_t::bud_lshare_t(const file_path_t &file_path)	throw()
{
	// log to debug
	KLOG_DBG("read a bud_lshare_t from " << file_path);
	// init the object from the data in the file_path
	init_from_file(file_path);
}

/** \brief Destructor
 */
bud_lshare_t::~bud_lshare_t()	throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                  init_from_file
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief init the object from the data found in a file
 */
void	bud_lshare_t::init_from_file(const file_path_t &file_path)	throw()
{
	property_t			property;
	std::vector<std::string>	words, words2;
	
	// log to debug
	KLOG_DBG("enter file_path=" << file_path);
	// load the file as a property_t
	bool	failed = property.load_file(file_path.to_string());
	if( failed ){
		KLOG_ERR("Cant load local share property_t from " << file_path);
		return;
	}
	
	// only to debug
	//property.dump_dbg();
	
	// get the keywords
	std::string	keywords_str	= property.find_string("keywords");
	words = string_t::split(keywords_str, ";");
	for( size_t i = 0; i < words.size(); i++ )
		keyword_db_val.push_back(string_t::strip(words[i]));
	
	// get the users
	std::string	users_str	= property.find_string("users");
	if( users_str.empty() == false ){
		// split each user/password
		words = string_t::split(users_str, ";");
		// go thru each user/password
		for( size_t i = 0; i < words.size(); i++ ){
			// separate the user from the passwd
			std::string	user_pass_str	= string_t::strip(words[i]);
			words2	= string_t::split(user_pass_str, ":", 2);
			if(words2.size() != 2)	return;
			// log to debug
			KLOG_DBG("user=" << words2[0] << " pass=" << words2[1]);
	
			// put the user/password in the user_db_val		
			bool succeed = user_db_val.insert(std::make_pair(words2[0], words2[1])).second;
			if( !succeed )	return;
		}
	}	
	// get the local_id
	local_id_val	= property.find_string("local_id");
	if( local_id_val.empty() )		return;

	// get the file_rootpath
	// - NOTE: file_rootpath MUST be last as it is used for .is_null();
	file_rootpath_val	= property.find_string("file_rootpath");
	if( file_rootpath_val.is_null() )	return;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bud_lshare_t::to_string()			const throw()
{
	std::vector<std::string>::const_iterator		iter;
	std::map<std::string, std::string>::const_iterator	iter2;
	std::ostringstream					oss;

	oss << "file_rootpath="	<< file_rootpath_val	<< "   ";
	oss << "local_id="	<< local_id_val		<< "   ";
	// put the keywords
	oss << "keywords=";
	for( iter = keyword_db_val.begin(); iter != keyword_db_val.end(); iter++ ){
		if( iter != keyword_db_val.begin() )	oss << ";";
		oss << *iter;
	}
	oss << "   ";
	// put the users
	oss << "users=";
	for( iter2 = user_db_val.begin(); iter2 != user_db_val.end(); iter2++ ){
		if( iter2 != user_db_val.begin() )	oss << ";";
		oss << iter2->first << ":" << iter2->second;
	}
	// return the result
	return oss.str();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                read all lshare config file from a directory
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Read all lshare config file from a directory
 */
bud_lshare_db_t	bud_lshare_t::read_from_directory(const file_path_t &dir_path)	throw()
{
	file_dir_t	file_dir;
	file_err_t	file_err = file_dir.open(dir_path);
	bud_lshare_db_t	result;
	if( file_err.failed() ){
		KLOG_ERR("Cant open directory " << dir_path << " due to " << file_err);
		return bud_lshare_db_t();	
	}
	
	// remove all file_dir_t elements which doesnt end with .lshare
	file_dir.filter(file_dir_t::filter_glob_nomatch("*\\.lshare"));
	
	// go thru all the .lshare file in this directory	
	for( size_t i = 0; i < file_dir.size(); i++ ){
		// log to debug
		KLOG_DBG("the dir contains " << file_dir[i]);
		// read this file and put it in a bud_lshare_t
		bud_lshare_t	bud_lshare(file_dir[i]);
		// if this init for this lshare failed, return now a null element
		if( bud_lshare.is_null() ){
			KLOG_ERR("lshare file " << file_dir[i] << " contains errors.");
			return bud_lshare_db_t();
		}
		
		// put buf_lshare_t in the database
		bool succeed = result.insert(std::make_pair(bud_lshare.local_id(), bud_lshare)).second;
		if( !succeed )			return bud_lshare_db_t();
	}
	
	// return the result
	return result;
}

NEOIP_NAMESPACE_END

