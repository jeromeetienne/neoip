/*! \file
    \brief Definition of the \ref bt_iov_arr_t
    
*/

/* system include */
/* local include */
#include "neoip_bt_iov_arr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief 'reduce' the bt_iov_arr_t (aka merge the successive bt_iov_t)
 * 
 * - ALGO: if 2 succesives bt_iov_t are on the same index and have contiguous IO
 *   they will be merged into a single one
 * - the bt_iov_arr_t is assumed to be sorted.
 *   - if needed, the item_arr_t::sort() may be used for that
 */
bt_iov_arr_t &	bt_iov_arr_t::reduce()	throw()
{
	size_t i = 0;
	// go thru the whole bt_iov_arr_t (except the last one as it has no 'next')
	// - NOTE: a lot of 'weird stuff' due to 'removal during walk' issue
	while( i + 1 < this->size() ){
		bt_iov_t &	curr = (*this)[i];
		bt_iov_t &	next = (*this)[i+1];
		// if curr and next are not contiguous, goto the next bt_iov_t
		if( !curr.is_contiguous(next) ){
			// increase the index
			i++;
			// goto the next bt_iov_t
			continue;
		}
		// NOTE: here both are contiguous
		// merge the next bt_iov_t into the curr one
		curr	= bt_iov_t(curr.subfile_idx(), file_range_t(curr.subfile_beg(), next.subfile_end()));
		// remove the next bt_iov_t 
		this->remove(i+1);
		// NOTE: dont increment the index on purpose
	}
	// return the object itself
	return *this;
}

NEOIP_NAMESPACE_END;






