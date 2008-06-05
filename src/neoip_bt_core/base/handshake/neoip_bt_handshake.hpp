/*! \file
    \brief Header of the \ref bt_handshake_t

*/


#ifndef __NEOIP_BT_HANDSHAKE_HPP__ 
#define __NEOIP_BT_HANDSHAKE_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_bt_id.hpp"
#include "neoip_bt_protoflag.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the parameters replied by a tracker
 */
class bt_handshake_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** query function	***************************************/
	bool		is_null()		const throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_handshake_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( std::string	, protocolid);
	RES_VAR_DIRECT( bt_protoflag_t	, protoflag);
	RES_VAR_DIRECT( bt_id_t		, infohash);
	RES_VAR_DIRECT( bt_id_t		, peerid);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_handshake_t &bt_handshake) throw()
						{ return oss << bt_handshake.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bt_handshake_t &handshake)	throw();
	friend	serial_t& operator >> (serial_t & serial, bt_handshake_t &handshake) 	throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HANDSHAKE_HPP__  */



