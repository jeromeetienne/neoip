/*! \file
    \brief Header of the \ref bt_io_stats_t

*/


#ifndef __NEOIP_BT_IO_STATS_HPP__ 
#define __NEOIP_BT_IO_STATS_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_bt_io_stats_wikidbg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_file_size.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	file_range_t;

/** \brief to store the statistic about a bt_io_pfile_t
 * 
 * - NOTE: this is only for diagnostic purpose. this MUST NOT be used in 'core' parts
 *   - thus it may be avoided during compilation to save memory and cpu
 */
class bt_io_stats_t : NEOIP_COPY_CTOR_ALLOW
		, private wikidbg_obj_t<bt_io_stats_t, bt_io_stats_wikidbg_init> {
private:
public:
	/*************** ctor/dtor	***************************************/
	bt_io_stats_t()	throw();

	/*************** Action function	*******************************/
	bt_io_stats_t &	update_for_read(const file_range_t &file_range)		throw();
	bt_io_stats_t &	update_for_write(const file_range_t &file_range)	throw();

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_io_stats_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( file_size_t	, totlen_read);
	RES_VAR_DIRECT( file_size_t	, totlen_write);
	RES_VAR_DIRECT( size_t		, nb_read);
	RES_VAR_DIRECT( size_t		, nb_write);

	RES_VAR_DIRECT( size_t		, nb_read_beg_align16k);
	RES_VAR_DIRECT( size_t		, nb_read_len_align16k);
	RES_VAR_DIRECT( size_t		, nb_write_beg_align16k);
	RES_VAR_DIRECT( size_t		, nb_write_len_align16k);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const bt_io_stats_t &stats) throw()
						{ return os << stats.to_string();	}

	/*************** List of friend class	*******************************/
	friend class	bt_io_stats_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_STATS_HPP__  */



