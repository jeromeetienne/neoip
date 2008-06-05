/*! \file
    \brief Definition of the \ref bt_io_cache_pool_profile_t

\par About BLOCK_ALIGN_SIZE and BLOCK_MAXLEN
- they are tuned to be aligned with the typical bittorrent way to read/write
  requests on the wire.
  - it is related to bt_swarm_profile_t::xmit_req_maxlen() which is itself 
    related to mainline implementation which disconnect every remote
    peer doing request of more than 16kbyte.
  - see http://wiki.theory.org/BitTorrentSpecification#request:_.3Clen.3D0013.3E.3Cid.3D6.3E.3Cindex.3E.3Cbegin.3E.3Clength.3E
- so all bt_io_cache_block_t::beg() is 16kbyte aligned and is at most 16kbyte long. 

\par About POOL_MAXLEN
- it is set to a null file_size_t by default, thus any user of bt_io_cache_pool_t
  is forced to set it to a value before starting it.
- it is to ensure the caller is fully aware of the memory used by this.

*/


/* system include */
/* local include */
#include "neoip_bt_io_cache_pool_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_io_cache_pool_profile_t constant
const file_size_t	bt_io_cache_pool_profile_t::POOL_MAXLEN		= file_size_t();
const file_size_t	bt_io_cache_pool_profile_t::BLOCK_ALIGN_SIZE	= 16*1024;
const file_size_t	bt_io_cache_pool_profile_t::BLOCK_MAXLEN	= 16*1024;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_io_cache_pool_profile_t::bt_io_cache_pool_profile_t()	throw()
{
	pool_maxlen		(POOL_MAXLEN		);
	block_align_size	(BLOCK_ALIGN_SIZE	);
	block_maxlen		(BLOCK_MAXLEN		);
}

/** \brief Destructor
 */
bt_io_cache_pool_profile_t::~bt_io_cache_pool_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_io_cache_pool_profile_t::check()	const throw()
{
	// check pool_maxlen is non-null and > 0
	if( pool_maxlen().is_null() || pool_maxlen() == 0 )
		return bt_err_t(bt_err_t::ERROR, "bt_io_cache_pool_profile_t::pool_maxlen MUST be set to > 0");
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

