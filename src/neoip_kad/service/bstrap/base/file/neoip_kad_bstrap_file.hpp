/*! \file
    \brief Declaration of kad_bstrap_file_t
    
*/


#ifndef __NEOIP_KAD_BSTRAP_FILE_HPP__ 
#define __NEOIP_KAD_BSTRAP_FILE_HPP__ 
/* system include */
/* local include */
#include "neoip_file_path.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_err_t;
class	kad_peer_t;

/** \brief helper to handle kad_bstrap_t files
 */
class kad_bstrap_file_t {
public:
	/*************** file_path_t builder	*******************************/
	static file_path_t	dynfile_path(kad_peer_t *kad_peer)	throw();
	static file_path_t	fixfile_path(kad_peer_t *kad_peer)	throw();

	/*************** files load/save	*******************************/
	// - but the load is done directly by ipport_strlist_src_t
	// - and fixfile is preconfigured so no need to be saved
	static kad_err_t	dynfile_save(kad_peer_t *kad_peer)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_BSTRAP_FILE_HPP__  */



