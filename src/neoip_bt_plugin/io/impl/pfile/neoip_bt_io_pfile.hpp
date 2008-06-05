/*! \file
    \brief Header of the bt_io_pfile_t
    
*/


#ifndef __NEOIP_BT_IO_PFILE_HPP__ 
#define __NEOIP_BT_IO_PFILE_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_io_pfile_wikidbg.hpp"
#include "neoip_bt_io_pfile_profile.hpp"
#include "neoip_bt_io_vapi.hpp"
#include "neoip_bt_io_mode.hpp"
#include "neoip_bt_io_stats.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bitfield.hpp"
#include "neoip_file_path.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_prange_arr_t;

/** \brief class definition for bt_swarm_sched
 */
class bt_io_pfile_t : NEOIP_COPY_CTOR_DENY, public bt_io_vapi_t
		, private wikidbg_obj_t<bt_io_pfile_t, bt_io_pfile_wikidbg_init> {
private:
	bt_io_mode_t		m_io_mode;
	bt_io_pfile_profile_t	m_profile;
	bt_mfile_t		m_bt_mfile;
	file_path_t		dest_dirname;
	bitfield_t		m_file_exist;	//!< bitfield true for any existing piece_idx file 

	/*************** bt_io_stats_t	***************************************/
	bt_io_stats_t		m_io_stats;
	bt_io_stats_t &		stats()		throw()	{ return m_io_stats;	}

	/*************** internal function	*******************************/
	file_path_t	piece_filepath(size_t piece_idx)			const throw();
	void		remove_temp_files()					const throw();
	bt_prange_arr_t	totfile_to_prange_arr(const file_range_t &totfile_range)const throw();

	/*************** store the bt_io_read_t	*******************************/
	std::list<bt_io_read_t *>	read_db;
	void read_dolink(bt_io_read_t *read)	throw()	{ read_db.push_back(read);	}
	void read_unlink(bt_io_read_t *read)	throw()	{ read_db.remove(read);		}

	/*************** store the bt_io_write_t	*******************************/
	std::list<bt_io_write_t *>	write_db;
	void write_dolink(bt_io_write_t *write) throw()	{ write_db.push_back(write);	}
	void write_unlink(bt_io_write_t *write)	throw()	{ write_db.remove(write);	}
public:
	/*************** ctor/dtor	***************************************/
	bt_io_pfile_t()					throw();
	~bt_io_pfile_t()				throw();
	
	/************** Setup function	***************************************/
	bt_io_pfile_t &	profile(const bt_io_pfile_profile_t &profile)	throw();
	bt_err_t	start(const bt_mfile_t &bt_mfile)		throw();
	
	/*************** query function	***************************************/
	const bt_io_pfile_profile_t &profile()	const throw()	{ return m_profile;	}
	const bt_io_stats_t &	stats()		const throw()	{ return m_io_stats;	}
	
	/*************** bt_io_vapi_t function	*******************************/
	const bt_mfile_t &	bt_mfile()	const throw()	{ return m_bt_mfile;	}
	const bt_io_mode_t &	mode()		const throw()	{ return m_io_mode;	}
	bt_io_read_t *		read_ctor(const file_range_t &totfile_range
					, bt_io_read_cb_t *callback, void *userptr)	throw();
	bt_io_write_t *		write_ctor(const file_range_t &totfile_range, const datum_t &data2write
					, bt_io_write_cb_t *callback, void *userptr)	throw();
	bt_err_t		remove(const file_range_t &totfile_range)		throw();
	bool			need_stopping()		const throw()	{ return false;	}
	bt_io_stopping_t *	stopping_ctor(bt_io_stopping_cb_t *callback
					, void *userptr)	throw()	{ return NULL;	}
	

	/*************** List of friend class	*******************************/
	friend class	bt_io_pfile_wikidbg_t;
	friend class	bt_io_pfile_read_t;
	friend class	bt_io_pfile_write_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_PFILE_HPP__  */



