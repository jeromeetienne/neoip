/*! \file
    \brief Header of the bt_io_cache_block_t
    
*/


#ifndef __NEOIP_BT_IO_CACHE_BLOCK_HPP__ 
#define __NEOIP_BT_IO_CACHE_BLOCK_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_io_cache_block_wikidbg.hpp"
#include "neoip_bt_io_cache_state.hpp"
#include "neoip_datum.hpp"
#include "neoip_file_range.hpp"
#include "neoip_mlist.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_io_cache_t;

/** \brief class definition for bt_io_cache_block_t
 */
class bt_io_cache_block_t : NEOIP_COPY_CTOR_DENY 
		, private wikidbg_obj_t<bt_io_cache_block_t, bt_io_cache_block_wikidbg_init> {
private:
	bt_io_cache_t *		m_io_cache;	//!< backpointer to bt_io_cache_t
	file_range_t		m_range;	//!< the totfile offset of this block
	datum_t			m_datum;	//!< the datum_t containing the data 
	bt_io_cache_state_t	m_state;	//!< the current bt_io_cache_state_t of this block
	
	mlist_item_t<bt_io_cache_block_t>	m_lru_item;	//!< to get fast lru in bt_io_cache_pool_t
	
	/*************** Internal function	*******************************/
	void			ctor_postprocess()		throw();

public:
	/*************** ctor/dtor	***************************************/
	bt_io_cache_block_t(bt_io_cache_t * p_io_cache, const file_range_t &p_range
							, const datum_t &p_datum)	throw();
	bt_io_cache_block_t(bt_io_cache_t * p_io_cache, const file_range_t &p_range)	throw();
	~bt_io_cache_block_t()								throw();
	
	/*************** Query function	***************************************/
	bt_io_cache_t *		io_cache()	const throw()	{ return m_io_cache;	}
	const file_range_t &	range()		const throw()	{ return m_range;	}
	const datum_t &		datum()		const throw()	{ return m_datum;	}
	const bt_io_cache_state_t &state()	const throw()	{ return m_state;	}
	
	file_range_t		common_range(const file_range_t &other)		const throw();
	size_t			length()	const throw()	{ return range().length().to_size_t();	}
	const file_size_t &	beg()		const throw()	{ return range().beg();	}
	const file_size_t &	end()		const throw()	{ return range().end();	}
	bool			contain(const file_size_t &off)		const throw()	{ return range().contain(off);		}
	bool			dont_contain(const file_size_t &off)	const throw()	{ return !contain(off);			}
	bool			fully_after(const file_range_t &other)	const throw()	{ return range().fully_after(other);	}

	mlist_item_t<bt_io_cache_block_t> &	lru_item()	throw()	{ return m_lru_item;	}

	/*************** Action function	*******************************/
	void			notify_read()		throw();
	void			notify_write()		throw();
	void			notify_cleaning()	throw();
	void			notify_cleaned()	throw();

	/*************** List of friend class	*******************************/
	friend class	bt_io_cache_block_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_CACHE_BLOCK_HPP__  */



