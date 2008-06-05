/*! \file
    \brief Header of the \ref bt_lnk2mfile_profile_t

*/


#ifndef __NEOIP_BT_LNK2MFILE_PROFILE_HPP__ 
#define __NEOIP_BT_LNK2MFILE_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the fields
#include "neoip_file_path.hpp"
#include "neoip_file_size.hpp"
#include "neoip_http_uri.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief to handle the profile for get_t
 */
class bt_lnk2mfile_profile_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the infohash prefix when the content type is 'static'
	static const std::string	STATIC_INFOHASH_PREFIX;
	//! the piecelen to use when the content is a 'static' one
	static const size_t		STATIC_PIECELEN;
	//! the announce_uri to use when the content type is 'static'
	static const http_uri_t		STATIC_ANNOUNCE_URI;
	//! the file length in byte when the content type is 'static'
	static const file_size_t	STATIC_FILELEN;
	//! the piecelen to use when the content type is 'metalink'
	static const size_t		MLINK_PIECELEN;
	//! the destination directory for the bt_mfile_subfile_t::local_path
	static const std::string	DEST_LOCAL_DIR;
public:
	/*************** ctor/dtor	***************************************/
	bt_lnk2mfile_profile_t()		throw();
	~bt_lnk2mfile_profile_t()	throw();

	/*************** validity function	*******************************/
	bt_err_t	check()		const throw();

	/*************** #define to ease the declaration	***************/
#	define PROFILE_VAR_PLAIN(var_type, var_name)								\
	private:var_type	var_name##_val;								\
	public:	const var_type &	var_name()	const throw()	{ return var_name ## _val; }	\
		bt_lnk2mfile_profile_t &	var_name(const var_type &value)	throw()			\
						{ var_name ## _val = value; return *this;	}
#	define PROFILE_VAR_SPROF(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
		PROFILE_VAR_PLAIN(var_type, var_name);

	/*************** declaration of profile fields	***********************/
	PROFILE_VAR_PLAIN( std::string	, static_infohash_prefix);
	PROFILE_VAR_PLAIN( size_t	, static_piecelen);
	PROFILE_VAR_PLAIN( http_uri_t	, static_announce_uri);
	PROFILE_VAR_PLAIN( file_size_t	, static_filelen);
	PROFILE_VAR_PLAIN( size_t	, mlink_piecelen);
	PROFILE_VAR_PLAIN( std::string	, dest_local_dir);

	/*************** #undef to ease the declaration	***********************/
#	undef PROFILE_VAR_PLAIN
#	undef PROFILE_VAR_SPROF
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_LNK2MFILE_PROFILE_HPP__  */



