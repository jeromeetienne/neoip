/*! \file
    \brief Header of the \ref bt_swarm_resumedata_t

*/


#ifndef __NEOIP_BT_SWARM_RESUMEDATA_HPP__ 
#define __NEOIP_BT_SWARM_RESUMEDATA_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internals fields 
#include "neoip_bt_peersrc_peer_arr.hpp"
#include "neoip_bt_pieceavail.hpp"
#include "neoip_bt_pieceprec_arr.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_file_size_inval.hpp"
#include "neoip_bt_swarm_stats.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store the data to preverse from one instance of a bt_swarm_t to another
 */
class bt_swarm_resumedata_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_resumedata_t()		throw();
	static bt_swarm_resumedata_t	from_mfile(const bt_mfile_t &bt_mfile)	throw();

	/*************** query function	***************************************/
	bool		is_null()					const throw();
	bt_err_t	check()						const throw();
	bitfield_t	get_partavail_piece_bitfield()			const throw();
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_swarm_resumedata_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_STRUCT( bt_mfile_t		, bt_mfile);
	RES_VAR_STRUCT( bt_pieceavail_t		, pieceavail_local);
	RES_VAR_STRUCT( bt_pieceprec_arr_t	, pieceprec_arr);
	RES_VAR_STRUCT( bt_peersrc_peer_arr_t	, peersrc_peer_arr);
	RES_VAR_STRUCT( file_size_inval_t	, partavail_piece_inval);
	RES_VAR_STRUCT( bt_swarm_stats_t	, swarm_stats);	
	RES_VAR_DIRECT( bool			, mfile_allocated);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const bt_swarm_resumedata_t &resumedata ) throw()
						{ return os << resumedata.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bt_swarm_resumedata_t &swarm_resumedata)	throw();
	friend	serial_t& operator >> (serial_t & serial, bt_swarm_resumedata_t &swarm_resumedata) 	throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_RESUMEDATA_HPP__  */



