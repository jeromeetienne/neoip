/*! \file
    \brief Header of the bt_io_cache_t
    
*/


#ifndef __NEOIP_BT_IO_CACHE_HPP__ 
#define __NEOIP_BT_IO_CACHE_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_io_cache_wikidbg.hpp"
#include "neoip_bt_io_cache_profile.hpp"
#include "neoip_bt_io_cache_blkwr_cb.hpp"
#include "neoip_bt_io_vapi.hpp"
#include "neoip_bt_io_mode.hpp"
#include "neoip_bt_io_stats.hpp"
#include "neoip_timeout.hpp"
#include "neoip_file_size.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declarations
class	bt_io_cache_pool_t;
class	bt_io_cache_block_t;
class	bt_io_cache_stopping_t;

/** \brief class definition for bt_io_cache_t
 */
class bt_io_cache_t : NEOIP_COPY_CTOR_DENY, public bt_io_vapi_t
			, private timeout_cb_t, private bt_io_cache_blkwr_cb_t
			, private wikidbg_obj_t<bt_io_cache_t, bt_io_cache_wikidbg_init> {
private:
	bt_io_cache_pool_t *	m_cache_pool;	//!< backpointer to the bt_io_cache_pool_t
	bt_io_cache_profile_t	m_profile;	//!< the profile for this object
	bt_io_mode_t		m_io_mode;	//!< the bt_io_mode_t of this bt_io_vapi_t
	bt_io_vapi_t *		m_subio_vapi;	//!< the sub bt_io_vapi_t
	bt_err_t		m_delayed_write_err;	//!< the bt_err_t for delayed_write

	/*************** bt_io_stats_t	***************************************/
	bt_io_stats_t		m_io_stats;
	bt_io_stats_t &		stats()		throw()	{ return m_io_stats;		}

	/*************** store the bt_io_cache_block_t	***********************/
	typedef std::map<file_size_t, bt_io_cache_block_t *>	block_db_t;
	block_db_t	block_db;
	void 		block_dolink(bt_io_cache_block_t *cache)		throw();
	void 		block_unlink(bt_io_cache_block_t *cache)		throw();
	bt_io_cache_block_t * block_find(const file_size_t &offset)		const throw();
	void		block_remove_included_in(const file_range_t &range)	throw();

	/*************** cleaning stuff	***************************************/
	timeout_t	cleaning_timeout;
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** blkwr_db	***************************************/
	std::list<bt_io_cache_blkwr_t *>	blkwr_db;
	bool		neoip_bt_io_cache_blkwr_cb(void *cb_userptr, bt_io_cache_blkwr_t &cb_cache_blkwr
						, const bt_err_t &bt_err)	throw();

	/*************** store the bt_io_read_t	*******************************/
	std::list<bt_io_read_t *>	read_db;
	void	read_dolink(bt_io_read_t *read)		throw()	{ read_db.push_back(read);	}
	void	read_unlink(bt_io_read_t *read)		throw()	{ read_db.remove(read);		}

	/*************** store the bt_io_write_t	*******************************/
	std::list<bt_io_write_t *>	write_db;
	void	write_dolink(bt_io_write_t *write)	throw()	{ write_db.push_back(write);	}
	void	write_unlink(bt_io_write_t *write)	throw()	{ write_db.remove(write);	}

	/*************** store bt_io_stopping_t	*******************************/
	// NOTE: there is stopping_db for regularity, but it never contains more than 1 element
	std::list<bt_io_cache_stopping_t *>	stopping_db;
	void	stopping_dolink(bt_io_cache_stopping_t *stopping)	throw()	{ stopping_db.push_back(stopping);	}
	void	stopping_unlink(bt_io_cache_stopping_t *stopping)	throw()	{ stopping_db.remove(stopping);		}
public:
	/*************** ctor/dtor	***************************************/
	bt_io_cache_t()				throw();
	~bt_io_cache_t()			throw();

	/************** Setup function	***************************************/
	bt_io_cache_t &	profile(const bt_io_cache_profile_t &profile)	throw();
	bt_err_t	start(bt_io_cache_pool_t *m_cache_pool, bt_io_vapi_t *m_subio_vapi)throw();

	/*************** query function	***************************************/
	const bt_io_stats_t &	stats()		const throw()	{ return m_io_stats;	}
	bt_io_vapi_t *		subio_vapi()	const throw()	{ return m_subio_vapi;	}
	bt_io_cache_pool_t *	cache_pool()	const throw()	{ return m_cache_pool;	}
	const bt_io_cache_profile_t & profile()	const throw()	{ return m_profile;	}
	file_range_t		block_range_for(const file_size_t &offset)	const throw();
	std::list<bt_io_cache_block_t *> block_dirty_contiguous(const file_size_t &offset)
										const throw();
	const bt_err_t &	delayed_write_err()const throw(){ return m_delayed_write_err;	}
	std::list<bt_io_cache_block_t *> 
				block_fully_included_in(const file_range_t &range)throw();
				
	/*************** bt_io_cache_blkwr_t starting	***********************/
	void			start_blkwr(const file_size_t &asked_off)	throw();
	void			start_blkwr_all()				throw();

	/*************** bt_io_vapi_t function	*******************************/
	const bt_mfile_t &	bt_mfile()	const throw();	
	const bt_io_mode_t &	mode()		const throw()	{ return m_io_mode;	}
	bt_io_read_t *		read_ctor(const file_range_t &totfile_range
					, bt_io_read_cb_t *callback, void *userptr)	throw();
	bt_io_write_t *		write_ctor(const file_range_t &totfile_range, const datum_t &data2write
					, bt_io_write_cb_t *callback, void *userptr)	throw();
	bt_err_t		remove(const file_range_t &totfile_range)		throw();
	bool			need_stopping()		const throw()	{ return true;	}
	bt_io_stopping_t *	stopping_ctor(bt_io_stopping_cb_t *callback, void *userptr)throw();

	/*************** List of friend class	*******************************/
	friend class	bt_io_cache_wikidbg_t;
	friend class	bt_io_cache_read_t;
	friend class	bt_io_cache_write_t;
	friend class	bt_io_cache_block_t;
	friend class	bt_io_cache_stopping_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_CACHE_HPP__  */



