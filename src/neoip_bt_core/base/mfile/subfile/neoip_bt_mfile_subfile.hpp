/*! \file
    \brief Header of the \ref bt_mfile_subfile_t

*/


#ifndef __NEOIP_BT_MFILE_SUBFILE_HPP__ 
#define __NEOIP_BT_MFILE_SUBFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_http_uri_arr.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_range.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the file in bt_mfile_t
 */
class bt_mfile_subfile_t : NEOIP_COPY_CTOR_ALLOW {
private:
	/*************** precomputed value	*******************************/
	file_range_t	totfile_range_precpu;
public:
	/*************** ctor/dtor	***************************************/
	bt_mfile_subfile_t()								throw()	{}
	bt_mfile_subfile_t(const file_path_t &mfile_path, const file_size_t &len)	throw()
					{ this->mfile_path(mfile_path).len(len);	}
	/*************** query function	***************************************/
	bool		is_null()	const throw();
	bool		is_fully_init()	const throw();
	bool		is_sfile_ok()	const throw();
	bool		is_pfile_ok()	const throw();

	/*************** query the precomputed values	***********************/
	const file_range_t &	totfile_range()	const throw()	{ DBG_ASSERT(!totfile_range_precpu.is_null());
								  return totfile_range_precpu;		}
	const file_size_t &	totfile_beg()	const throw()	{ return totfile_range().beg();		}
	const file_size_t &	totfile_end()	const throw()	{ return totfile_range().end();		}	

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_mfile_subfile_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);
	
	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( file_path_t	, mfile_path);	//!< the file_path_t contains in the bt_mfile_t
	RES_VAR_DIRECT( file_size_t	, len);		//!< the length of this file
	RES_VAR_DIRECT( file_path_t	, local_path);	//!< the local file_path_t of this subfile
	RES_VAR_STRUCT( http_uri_arr_t	, uri_arr);	//!< the list of uri thru which this file is avail

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const bt_mfile_subfile_t &mfile_subfile ) throw()
						{ return os << mfile_subfile.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bt_mfile_subfile_t &mfile_subfile)throw();
	friend	serial_t& operator >> (serial_t & serial, bt_mfile_subfile_t &mfile_subfile) 	throw(serial_except_t);	
	
	/************** List of friend class	*******************************/
	friend class	bt_mfile_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_MFILE_SUBFILE_HPP__  */



