/*! \file
    \brief Definition of the \ref thumb_factory_t class

// this one failed
// - apparently this function is depreciated and advices to use gnome_icon_lookup() in libgnomeui
// - there is even the stuff to generate icon and stuff
// - try to make it work
//		oss << h.s_td()	<< gnome_vfs_icon_path_from_filename(file_dir[i].c_str())	<< h.e_td();
    
*/

/* system include */
#include <fnmatch.h>
/* local include */
#include "neoip_thumb_factory.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_file_utils.hpp"
#include "neoip_mimediag.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_log.hpp"

#define	USE_EXISTING_FREEDESKTOP_THUMBNAIL	1	//!< 1 if the freedesktop thumbnail scheme MUST 
							//!< be tested.
#if USE_EXISTING_FREEDESKTOP_THUMBNAIL
#	include "neoip_skey_auth.hpp"
static const std::string	URL_ESCAPE_CHARSET	=
				"<>#%\""	// the delims from rfc2396.2.4.3
				"{}|\\^[]`"	// the unwise from rfc2396.2.4.3
				" "		// the space  from rfc2396.2.4.3
				;
#endif // USE_EXISTING_FREEDESKTOP_THUMBNAIL

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
thumb_factory_t::thumb_factory_t()	throw()
{
	// compute the directory path which contains the thumbnail
	file_path_t	thumb_dirpath	= file_utils_t::get_current_dir()
						/ lib_session_get()->conf_rootdir()
						/ "http_fdir_files";
	// log to debug
	KLOG_DBG("thumb_dirpath=" << thumb_dirpath );
	// add mimetype and their associated thumbnail file_path_t
	add_type("x-directory/normal"	, thumb_dirpath / "directory.png");
	add_type("video/*"		, thumb_dirpath / "file-video.png");
	add_type("audio/*"		, thumb_dirpath / "file-audio.png");
	add_type("application/ogg"	, thumb_dirpath / "file-audio.png");
	add_type("default"		, thumb_dirpath / "file.png");
}

/** \brief Destructor
 */
thumb_factory_t::~thumb_factory_t()	throw()
{
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            database function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Add a mimetype pattern assocaited with this thumbnail file_path
 */
void	thumb_factory_t::add_type(const std::string &mimetype_pat, const file_path_t &file_path)	throw()
{
	bool	succeed	= type_db.insert(std::make_pair(mimetype_pat, file_path.to_string())).second;
	DBG_ASSERT( succeed );
}

/** \brief Return the file_path associated with this mimetype, or a null if none is found
 */
file_path_t thumb_factory_t::get_type(const std::string &mimetype)		const throw()
{
	std::map<std::string, std::string>::const_iterator	iter;
	for( iter = type_db.begin(); iter != type_db.end(); iter++ ){
		const std::string &	mimetype_pat	 = iter->first;
		// if this pattern match the parameter, return the file_path_t
		if( !fnmatch(mimetype_pat.c_str(), mimetype.c_str(), FNM_NOESCAPE) )
			return file_path_t(iter->second.c_str());
	}
	// if none is found return a null one
	return file_path_t();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                 query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Return full path of the thumb_factory image associated with this file
 */
file_path_t thumb_factory_t::get_thumb_name(const file_path_t &file_fullpath)	const throw()
{
	file_path_t	thumb_path;
	
	// log to debug
	KLOG_DBG("file_path=" << file_fullpath );

#if USE_EXISTING_FREEDESKTOP_THUMBNAIL
	// test the freedesktop thumbnail
	thumb_path	= get_freedesktop_thumb(file_fullpath);
	// if a freedesktop thumbnail is found, return it now
	if( !thumb_path.is_null() )	return thumb_path;
#endif // USE_EXISTING_FREEDESKTOP_THUMBNAIL

	// try to get mimetype
	std::string	mimetype	= mimediag.from_file(file_fullpath);
	// see if a special thumbnail for this mimetype
	thumb_path	= get_type(mimetype);
	// if no thumb_path have been found, get the default one
	if( thumb_path.is_null() )	thumb_path	= get_type("default");
	// sanity check - HERE the thumb_path MUST NOT be null
	DBG_ASSERT( !thumb_path.is_null() );
	// return the result
	return thumb_path;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                            freedesktop thumbnail
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the name of an existing freedesktop thumb, or an null file_path_t if none exist
 */
file_path_t	thumb_factory_t::get_freedesktop_thumb(const file_path_t &file_fullpath) const throw()
{
#if USE_EXISTING_FREEDESKTOP_THUMBNAIL
	// build the file_url
	std::string 	file_url = "file://" + file_fullpath.to_string();
	file_url	= string_t::escape(file_url, URL_ESCAPE_CHARSET);
	// compute a md5 hash over the file_url
	skey_auth_t	skey_auth("md5/nokey/16");
	skey_auth.update( file_url.c_str(), file_url.size() );
	// build the thumb_fullpath of the thumbnail
	file_path_t	home_path	= file_utils_t::get_home_dir();
	std::string	thumb_basename	= skey_auth.get_output().to_string().substr(2) + ".png";
	file_path_t	thumb_fullpath	= home_path / ".thumbnails" / "normal" / thumb_basename.c_str();
	// log to debug
	KLOG_DBG("tumbnail_fullpath=" << thumb_fullpath << " for file_url=" << file_url);
	
	// test if the thumbnail file exist
	file_stat_t	thumb_stat(thumb_fullpath.to_string());
	// if it doesnt exist, return an null 
	if( thumb_stat.is_null() )	return file_path_t();
	// else return the path
	return thumb_fullpath;
#else
	return file_path_t();
#endif // USE_EXISTING_FREEDESKTOP_THUMBNAIL
}

NEOIP_NAMESPACE_END


