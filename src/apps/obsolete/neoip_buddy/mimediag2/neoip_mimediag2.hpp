/*! \file
    \brief Header of the neoip_mimediag class
    
*/


#ifndef __NEOIP_MIMEDIAG_HPP__ 
#define __NEOIP_MIMEDIAG_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	file_path_t;

/** \brief Class to diagnose the mimetype of data
 */
class mimediag_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::map<std::string, std::string>	type_db;	//!< store the mimetype indexed by
								//!< by file extension
	
	void		add_type(const std::string &extension, const std::string &mimetype )	throw();
	std::string	get_type(const std::string &extension)				const throw();
public:
	/*************** ctor/dtor	***************************************/
	mimediag_t()	throw();
	~mimediag_t()	throw();
	
	/*************** query function	***************************************/
	std::string	from_file(const file_path_t &file_path)	const throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_MIMEDIAG_HPP__  */



