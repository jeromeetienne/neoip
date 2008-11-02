/*! \file
    \brief Header of the \ref rtmp_header_t

*/


#ifndef __NEOIP_RTMP_HEADER_HPP__
#define __NEOIP_RTMP_HEADER_HPP__
/* system include */
#include <string>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of header for the internal fields
#include "neoip_rtmp_type.hpp"
#include "neoip_delay.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the parameters replied by a tracker
 */
class rtmp_header_t : NEOIP_COPY_CTOR_ALLOW {
public:	/*************** Constant declaration	*******************************/
	//! the total length of a rtmp_header_t - which is constant
	static const size_t	TOTAL_LENGTH;
private:
public:
	/*************** query function	***************************************/
	bool		is_null()		const throw();

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	rtmp_header_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( uint8_t		, channel_id);
	RES_VAR_DIRECT( delay_t		, timestamp);
	RES_VAR_DIRECT( size_t		, body_length);
	RES_VAR_DIRECT( rtmp_type_t	, type);
	RES_VAR_DIRECT( uint32_t	, stream_id);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const rtmp_header_t &rtmp_header) throw()
						{ return oss << rtmp_header.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const rtmp_header_t &rtmp_header)	throw();
	friend	serial_t& operator >> (serial_t & serial, rtmp_header_t &rtmp_header) 		throw(serial_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_HEADER_HPP__  */

