/*! \file
    \brief Definition of the \ref bt_lnk2mfile_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_lnk2mfile_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_lnk2mfile_profile_t constant
const std::string	bt_lnk2mfile_profile_t::STATIC_INFOHASH_PREFIX	= "neoip_bt http uri";
const size_t		bt_lnk2mfile_profile_t::STATIC_PIECELEN		= 32*1024;
const http_uri_t	bt_lnk2mfile_profile_t::STATIC_ANNOUNCE_URI	= http_uri_t();
const file_size_t	bt_lnk2mfile_profile_t::STATIC_FILELEN		= file_size_t::NONE_VAL;
const size_t		bt_lnk2mfile_profile_t::MLINK_PIECELEN		= 256*1024;
const std::string	bt_lnk2mfile_profile_t::DEST_LOCAL_DIR		= ".";
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_lnk2mfile_profile_t::bt_lnk2mfile_profile_t()	throw()
{
	static_infohash_prefix	(STATIC_INFOHASH_PREFIX);
	static_piecelen		(STATIC_PIECELEN);
	static_announce_uri	(STATIC_ANNOUNCE_URI);
	static_filelen		(STATIC_FILELEN);
	mlink_piecelen		(MLINK_PIECELEN);
	dest_local_dir		(DEST_LOCAL_DIR);
}

/** \brief Destructor
 */
bt_lnk2mfile_profile_t::~bt_lnk2mfile_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_lnk2mfile_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

