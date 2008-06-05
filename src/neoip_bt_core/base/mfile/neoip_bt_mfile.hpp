/*! \file
    \brief Header of the \ref bt_mfile_t
    
*/


#ifndef __NEOIP_BT_MFILE_HPP__ 
#define __NEOIP_BT_MFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_mfile_wikidbg.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_id_arr.hpp"
#include "neoip_bt_mfile_subfile_arr.hpp"
#include "neoip_bt_iov_arr.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"
NEOIP_NAMESPACE_BEGIN

/** \brief class definition for bt_mfile_t
 */
class bt_mfile_t : NEOIP_COPY_CTOR_ALLOW , private wikidbg_obj_t<bt_mfile_t, bt_mfile_wikidbg_init> {
private:
	/*************** precomputed value	*******************************/
	file_size_t			totfile_size_precpu;
	size_t				nb_piece_precpu;
	std::map<file_size_t, size_t>	subfile_db;	//!< all the subfile idx with non 0 length
							//!< indexed by their totfile_beg
public:
	/*************** ctor/dtor	***************************************/
	bt_mfile_t()		throw();
	~bt_mfile_t()		throw();
	
	/*************** Convertion function	*******************************/
	static bt_mfile_t	from_bencode(const datum_t &bencoded_mfile)	throw();
	datum_t			to_bencode()					const throw();
	
	/*************** Setup function	***************************************/
	bt_mfile_t &	complete_init()						throw();
	bt_mfile_t &	set_file_local_dir(const file_path_t &dirname)		throw();
	
	/*************** query function		*******************************/
	bool		is_null()			const throw();	
	bool		is_fully_init()			const throw();
	bool		is_sfile_ok()			const throw();
	bool		is_pfile_ok()			const throw();
	bt_iov_arr_t	get_iov_arr(const file_range_t &totfile_range)	const throw();

	/*************** query the precomputed values	***********************/
	const file_size_t &totfile_size() const throw()	{ DBG_ASSERT(!totfile_size_precpu.is_null());
						  	  return totfile_size_precpu;			}
	const size_t &	nb_piece()	const throw()	{ DBG_ASSERT(nb_piece_precpu != std::numeric_limits<size_t>::max());
							  return nb_piece_precpu;			}
	file_range_t	totfile_range()	const throw()	{ return file_range_t(0, totfile_size()-1);	}

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_mfile_t &	var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of non precomputed value	***************/
	RES_VAR_DIRECT( bt_id_t			, infohash);
	RES_VAR_DIRECT( size_t			, piecelen);
	RES_VAR_STRUCT( bt_mfile_subfile_arr_t	, subfile_arr);
	RES_VAR_DIRECT( http_uri_t		, announce_uri);
	RES_VAR_DIRECT( file_path_t		, name);
	RES_VAR_STRUCT( bt_id_arr_t		, piecehash_arr);
	RES_VAR_DIRECT( bool			, no_external_peersrc);
	RES_VAR_DIRECT( bool			, do_piecehash);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const bt_mfile_t &bt_mfile)	throw()
						{ return os << bt_mfile.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bt_mfile_t &bt_mfile)	throw();
	friend	serial_t& operator >> (serial_t & serial, bt_mfile_t &bt_mfile) 	throw(serial_except_t);

	/*************** List of friend class	*******************************/
	friend class	bt_mfile_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_MFILE_HPP__  */










