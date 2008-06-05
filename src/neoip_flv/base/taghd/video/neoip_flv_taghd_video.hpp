/*! \file
    \brief Header of the \ref flv_taghd_video_t

*/


#ifndef __NEOIP_FLV_TAGHD_VIDEO_HPP__ 
#define __NEOIP_FLV_TAGHD_VIDEO_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of header for the internal fields
#include "neoip_flv_codecid.hpp"
#include "neoip_flv_frametype.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the parameters replied by a tracker
 */
class flv_taghd_video_t : NEOIP_COPY_CTOR_ALLOW {
public:	/*************** Constant declaration	*******************************/
	//! the total length of a flv_taghd_video_t - which is constant
	static const size_t	TOTAL_LENGTH;
private:
public:
	/*************** query function	***************************************/
	bool		is_null()		const throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	flv_taghd_video_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( flv_codecid_t	, codecid);
	RES_VAR_DIRECT( flv_frametype_t	, frametype);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const flv_taghd_video_t &value)	throw()
						{ return oss << value.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const flv_taghd_video_t &taghd_video)	throw();
	friend	serial_t& operator >> (serial_t & serial, flv_taghd_video_t &taghd_video) 	throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_TAGHD_VIDEO_HPP__  */



