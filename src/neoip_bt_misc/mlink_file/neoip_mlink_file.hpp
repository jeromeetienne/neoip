/*! \file
    \brief Header of the mlink_file_t
    
*/


#ifndef __NEOIP_MLINK_FILE_HPP__ 
#define __NEOIP_MLINK_FILE_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_mlink_subfile_arr.hpp"
#include "neoip_xml_except.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	datum_t;
class	xml_build_t;
class	xml_parse_t;

/** \brief class definition for mlink_file_t to store a metalink file
 */
class mlink_file_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	mlink_file_t()	throw();
	~mlink_file_t()	throw();
	
	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return head_version().empty();	}

	/*************** Action Function	*******************************/
	static mlink_file_t	from_xml(const datum_t &xml_datum)	throw();
	datum_t			to_xml()				const throw();

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	mlink_file_t &	var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of non precomputed value	***************/
	RES_VAR_DIRECT( std::string		, head_version);
	RES_VAR_DIRECT( std::string		, head_xmlns);
	RES_VAR_DIRECT( std::string		, head_origin);
	RES_VAR_DIRECT( std::string		, head_type);
	RES_VAR_DIRECT( std::string		, head_pubdate);
	RES_VAR_DIRECT( std::string		, head_refreshdate);
	RES_VAR_DIRECT( std::string		, head_generator);
	RES_VAR_DIRECT( std::string		, publisher_name);
	RES_VAR_DIRECT( std::string		, publisher_url);
	RES_VAR_DIRECT( std::string		, license_name);
	RES_VAR_DIRECT( std::string		, license_url);
	RES_VAR_DIRECT( std::string		, description);
	RES_VAR_DIRECT( std::string		, tags);
	RES_VAR_DIRECT( std::string		, identity);
	RES_VAR_DIRECT( std::string		, version);
	RES_VAR_STRUCT( mlink_subfile_arr_t	, subfile_arr);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const mlink_file_t &mlink_file)	throw()
						{ return os << mlink_file.to_string();	}
						
	/*************** xml serialization	*******************************/
	friend	xml_build_t& operator << (xml_build_t& xm_build, const mlink_file_t &mlink_file)throw();	
	friend	xml_parse_t& operator >> (xml_parse_t& xml_parse, mlink_file_t &mlink_file)	throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_MLINK_FILE_HPP__  */



