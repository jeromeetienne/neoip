/*! \file
    \brief Header of the bt_io_cache_pool_t
    
*/


#ifndef __NEOIP_BT_IO_CACHE_POOL_HPP__ 
#define __NEOIP_BT_IO_CACHE_POOL_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_io_cache_pool_profile.hpp"
#include "neoip_bt_io_cache_pool_wikidbg.hpp"
#include "neoip_mlist.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN
class	bt_io_cache_t;
class	bt_io_cache_block_t;

/** \brief class definition for bt_io_cache_pool_t
 */
class bt_io_cache_pool_t : NEOIP_COPY_CTOR_DENY
		, private wikidbg_obj_t<bt_io_cache_pool_t, bt_io_cache_pool_wikidbg_init> {
private:
	bt_io_cache_pool_profile_t	m_profile;	//!< the profile for this object
	file_size_t			m_pool_curlen;	//!< the sum of all block_lru block_len

	/*************** Internal function	*******************************/
	void		shrink_if_needed()	throw();
	bool		shrink_one_block()	throw();
	
	/*************** store the bt_io_cache_t	***********************/
	std::list<bt_io_cache_t *>	cache_db;
	void cache_dolink(bt_io_cache_t *cache) throw()	{ cache_db.push_back(cache);	}
	void cache_unlink(bt_io_cache_t *cache)	throw()	{ cache_db.remove(cache);	}

	/*************** bt_io_block_block_t lru	***********************/
	mlist_head_t<bt_io_cache_block_t>	block_lru;
	void block_lru_dolink(bt_io_cache_block_t *block)	throw();
	void block_lru_unlink(bt_io_cache_block_t *block)	throw();
	void block_lru_update(bt_io_cache_block_t *block)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_io_cache_pool_t()			throw();
	~bt_io_cache_pool_t()			throw();

	/************** Setup function	***************************************/
	bt_io_cache_pool_t &	profile(const bt_io_cache_pool_profile_t &profile)	throw();
	bt_err_t		start()		throw();

	/*************** query function	***************************************/
	const bt_io_cache_pool_profile_t & profile()	const throw()	{ return m_profile;	}
	const file_size_t &	pool_curlen()		const throw()	{ return m_pool_curlen;	}
	
	/*************** Action function	*******************************/
	void		notify_blkwr_completion()	throw();
	
	/*************** List of friend class	*******************************/
	friend class	bt_io_cache_pool_wikidbg_t;
	friend class	bt_io_cache_t;
	friend class	bt_io_cache_block_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_CACHE_POOL_HPP__  */



