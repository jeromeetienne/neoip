/*! \file
    \brief Definition of the \ref kad_bstrap_file_t class

- there is a directory from the profile ?
  - this is the current stuff
  - or this hardcoded stuff based on the libsession->config_dir ?
    - with ok not to be able to read/write
- /etc/neoip/kad_bstrap
  - for example 
- it is called the bstrap_file_dirname
- how to generate a fixfile file_path_t
  - read the whole directory
  - get all the basename
  - handle it as a string for the kad_realmid_t
  - compare with the kad_peer_t kad_realmid_t
  - if there is a match it is file
  - if not the file_path_t is kad_realmid_t::to_canonical_string
- how to generate a dynfile file_path_t
  - get the fixfile file_path_t and change the extension
- thus the basename of the dynfile and fixfile are identical which helps maangement
  - moreover as the fixfile will be in ascii and not canonical string, those 
    will be very readable 

*/

/* system include */
/* local include */
#include "neoip_kad_bstrap_file.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_kbucket.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_file.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build the file_path_t for the kad_bstrap_t dynfile (or a null file_path_t if error)
 */
file_path_t	kad_bstrap_file_t::dynfile_path(kad_peer_t *kad_peer)		throw()
{
	// dynfile are stored in lib_session_t::cache_rootdir() as they are dynamic and kept accross boot
	file_path_t	cache_rootdir	= lib_session_get()->cache_rootdir();
	file_path_t	dirname		= cache_rootdir / "kad_bstrap";
	// get the fixfile_path() to derive the dynpath from it
	file_path_t	fixpath		= fixfile_path(kad_peer);
	// if an error occurs, return a null file_path_t
	if( fixpath.is_null() )	return file_path_t();
	// dynpath is the fixpath with a different file extension
	file_path_t	dynpath		= dirname / std::string(fixpath.basename_noext() + ".dynfile");
	// log to debug
	KLOG_ERR("dynfile_path="<< dynpath);
	// return the just built dynpath
	return dynpath;
}

/** \brief Build the file_path_t for the kad_bstrap_t fixfile (or a null file_path_t if error)
 * 
 */
file_path_t	kad_bstrap_file_t::fixfile_path(kad_peer_t *kad_peer)		throw()
{
	// fixfile are stored in lib_session_t::cache_rootdir() as they are static
	file_path_t	conf_rootdir	= lib_session_get()->conf_rootdir();
	file_path_t	dirname		= conf_rootdir / "kad_bstrap";
	// TODO i should create the kad_bstrap directory if needed ?
	kad_realmid_t	kad_realmid	= kad_peer->get_realmid();
	file_dir_t	file_dir;
	file_err_t	file_err;
	// log to debug
	KLOG_DBG("enter dirname=" << dirname);
	// open the directory
	file_err	= file_dir.open(dirname);
	if( file_err.failed() ){
		// log the error
		KLOG_ERR("can't open " << dirname << " due to " << file_err);
		return file_path_t();
	}
	// keep only the filename matching *.public
	file_dir.filter( file_dir_t::filter_glob_nomatch("*.fixfile") );
	// go thru all the matching file_path_t
	for(size_t i = 0; i < file_dir.size(); i++){
		file_path_t	file_path	= file_dir[i];
		// if file_path.basename_noext() matched the kad_peer->kad_realmid, return this file_path 
		if( kad_realmid == kad_realmid_t(file_path.basename_noext()))
			return file_path;
	}
	// if this point is reached, the file_path_t is builded from the kad_realmid_t::to_canonical_str()
	return dirname / std::string(kad_realmid.to_canonical_string() + ".fixfile");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			dynfile_save()
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Save the file containing the kad_addr_arr_t cache
 */
kad_err_t	kad_bstrap_file_t::dynfile_save(kad_peer_t *kad_peer)		throw()
{
	const kad_profile_t &	profile	= kad_peer->get_profile();
	// get the kad_addr_arr_t of peer to save
	size_t		nb_addr		= profile.bstrap().dfile_max_nb_addr();
	kad_addr_arr_t	kad_addr_arr	= kad_peer->get_kbucket()->get_noldest_addr( nb_addr );
	// build the filename
	file_path_t	file_path	= dynfile_path(kad_peer);
	if( file_path.is_null() )
		return kad_err_t(kad_err_t::ERROR, "unable to generate kad dynfile file_path_t");
	// log to debug	
	KLOG_DBG("kad_addr_arr written in " << file_path << " is = " << kad_addr_arr);
	// build the file content
	std::ostringstream	oss;
	for(size_t i=0; i < kad_addr_arr.size(); i++){
		const kad_addr_t & kad_addr	= kad_addr_arr[i];
		// if this item is the local kad_peer_t, skip it
		if( kad_addr.peerid() == kad_peer->local_peerid() )	continue;
		// add this item ipport_addr_t to the file
		oss << kad_addr.oaddr().to_string() << "\n";
	}
	// save the file itself
	file_err_t	file_err;
	file_err	= file_sio_t::writeall(file_path, datum_t(oss.str()));
	if( file_err.failed() )	return kad_err_from_file(file_err);
	// return no error
	return kad_err_t::OK;
}


NEOIP_NAMESPACE_END


