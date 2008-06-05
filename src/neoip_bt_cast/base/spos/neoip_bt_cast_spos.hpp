/*! \file
    \brief Header of the \ref bt_cast_spos_t

*/


#ifndef __NEOIP_BT_CAST_SPOS_HPP__ 
#define __NEOIP_BT_CAST_SPOS_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_file_size.hpp"
#include "neoip_date.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the meta data for a bt cast
 */
class bt_cast_spos_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	bt_cast_spos_t()		throw()	{}
	bt_cast_spos_t(const file_size_t &p_byte_offset, const date_t &p_casti_date)	throw()
					{ this->byte_offset(p_byte_offset).casti_date(p_casti_date);	}
	
	/*************** query function	***************************************/
	bool		is_null()	const throw();

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_cast_spos_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( file_size_t	, byte_offset);
	RES_VAR_DIRECT( date_t		, casti_date);
		
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_cast_spos_t &cast_spos) throw()
					{ return oss << cast_spos.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bt_cast_spos_t &cast_spos)	throw();
	friend	serial_t& operator >> (serial_t & serial, bt_cast_spos_t &cast_spos) 		throw(serial_except_t);	

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const bt_cast_spos_t &cast_spos)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, bt_cast_spos_t &cast_spos)		throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_SPOS_HPP__  */



