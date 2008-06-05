/*! \file
    \brief Header of the neoip_thumb_factory class
    
*/


#ifndef __NEOIP_THUMB_FACTORY_HPP__ 
#define __NEOIP_THUMB_FACTORY_HPP__ 
/* system include */
/* local include */
#include "neoip_mimediag.hpp"
#include "neoip_file_path.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief Class to diagnose the mimetype of data
 */
class thumb_factory_t : NEOIP_COPY_CTOR_DENY {
private:
	mimediag_t	mimediag;

	std::map<std::string, std::string>	type_db;	//!< store the thumbnail file path
								//!< associated with a mimetype pattern
	void		add_type(const std::string &mimetype_pat, const file_path_t &file_path)	throw();
	file_path_t	get_type(const std::string &mimetype)			const throw();	

	file_path_t	get_freedesktop_thumb(const file_path_t &file_fullpath) const throw();
public:
	/*************** ctor/dtor	***************************************/
	thumb_factory_t()	throw();
	~thumb_factory_t()	throw();
	
	/*************** query function	***************************************/
	file_path_t	get_thumb_name(const file_path_t &file_path)	const throw();
	
	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_THUMB_FACTORY_HPP__  */



