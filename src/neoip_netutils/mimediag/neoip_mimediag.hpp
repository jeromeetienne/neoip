/*! \file
    \brief Header of the neoip_string class
    
*/


#ifndef __NEOIP_MIMEDIAG_HPP__ 
#define __NEOIP_MIMEDIAG_HPP__ 
/* system include */
#include <map>
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	file_path_t;

/** \brief helper to manipulate string
 */
class mimediag_t {
private:
	/*************** mimetype_db	***************************************/
	typedef	std::map<std::string, std::string>	mimetype_db_t;
	static	mimetype_db_t *	mimetype_db;
	
	/*************** internal function	*******************************/
	static void			ctor_mimetype_db()	throw();	
public:
	/*************** query function	***************************************/
	static const std::string &	from_file_ext(const std::string &file_ext
							, const std::string &default_str = "")	throw();	
	static const std::string &	from_file_path(const file_path_t &file_path
							, const std::string &default_str = "")	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_MIMEDIAG_HPP__  */



