/*! \file
    \brief Header of the mlink_url_t
    
*/


#ifndef __NEOIP_MLINK_URL_HPP__ 
#define __NEOIP_MLINK_URL_HPP__ 
/* system include */
/* local include */
#include "neoip_file_path.hpp"
#include "neoip_file_size.hpp"
#include "neoip_xml_except.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	xml_build_t;
class	xml_parse_t;

/** \brief class definition for mlink_url_t
 */
class mlink_url_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	mlink_url_t()	throw();
	~mlink_url_t()	throw();

	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return type().empty();	}

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	mlink_url_t &	var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of internal field	***********************/
	RES_VAR_DIRECT( std::string	, type);
	RES_VAR_DIRECT( std::string	, location);
	RES_VAR_DIRECT( std::string	, preference);
	RES_VAR_DIRECT( std::string	, url_content);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const mlink_url_t &mlink_url)	throw()
						{ return os << mlink_url.to_string();	}	

	/*************** xml serialization	*******************************/
	friend	xml_build_t& operator << (xml_build_t& xm_build, const mlink_url_t &mlink_url)	throw();	
	friend	xml_parse_t& operator >> (xml_parse_t& xml_parse, mlink_url_t &mlink_url)	throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_MLINK_URL_HPP__  */



