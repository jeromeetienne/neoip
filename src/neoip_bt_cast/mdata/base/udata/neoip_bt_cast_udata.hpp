/*! \file
    \brief Header of the \ref bt_cast_udata_t

*/


#ifndef __NEOIP_BT_CAST_UDATA_HPP__ 
#define __NEOIP_BT_CAST_UDATA_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_date.hpp"
#include "neoip_bt_cast_spos_arr.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the parameters replied by a tracker
 */
class bt_cast_udata_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	bt_cast_udata_t()		throw();
	
	/*************** query function	***************************************/
	bool		is_null()	const throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_cast_udata_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( size_t		, boot_nonce);
	RES_VAR_DIRECT( size_t		, pieceq_beg);
	RES_VAR_DIRECT( size_t		, pieceq_end);
	RES_VAR_DIRECT( date_t		, casti_date);
	RES_VAR_STRUCT( bt_cast_spos_arr_t, cast_spos_arr);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_cast_udata_t &cast_udata) throw()
					{ return oss << cast_udata.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bt_cast_udata_t &cast_udata)	throw();
	friend	serial_t& operator >> (serial_t & serial, bt_cast_udata_t &cast_udata) 		throw(serial_except_t);	

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const bt_cast_udata_t &cast_udata)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, bt_cast_udata_t &cast_udata)		throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_UDATA_HPP__  */



