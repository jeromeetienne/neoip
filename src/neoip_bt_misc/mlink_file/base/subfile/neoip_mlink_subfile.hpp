/*! \file
    \brief Header of the mlink_subfile_t
    
*/


#ifndef __NEOIP_MLINK_SUBFILE_HPP__ 
#define __NEOIP_MLINK_SUBFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_mlink_hash_arr.hpp"
#include "neoip_mlink_url_arr.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_size.hpp"
#include "neoip_xml_except.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	xml_build_t;
class	xml_parse_t;

/** \brief class definition for mlink_subfile_t
 */
class mlink_subfile_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	mlink_subfile_t()	throw();
	~mlink_subfile_t()	throw();
	
	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return name().is_null();	}

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	mlink_subfile_t &	var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of internal field	***********************/
	RES_VAR_DIRECT( file_path_t		, name);
	RES_VAR_DIRECT( std::string		, version);
	RES_VAR_DIRECT( std::string		, os);
	RES_VAR_DIRECT( std::string		, language);
	RES_VAR_DIRECT( file_size_t		, file_size);
	RES_VAR_STRUCT( mlink_hash_arr_t	, hash_arr);
	RES_VAR_STRUCT( mlink_url_arr_t		, url_arr);
	RES_VAR_DIRECT( datum_t			, bencoded_torrent);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const mlink_subfile_t &mlink_subfile)	throw()
						{ return os << mlink_subfile.to_string();	}	

	/*************** xml serialization	*******************************/
	friend	xml_build_t& operator << (xml_build_t& xm_build, const mlink_subfile_t &mlink_subfile)	throw();	
	friend	xml_parse_t& operator >> (xml_parse_t& xml_parse, mlink_subfile_t &mlink_subfile)	throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_MLINK_SUBFILE_HPP__  */



