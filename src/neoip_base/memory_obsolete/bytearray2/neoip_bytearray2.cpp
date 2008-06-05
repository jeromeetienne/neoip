/*! \file
    \brief Definition of \ref bytearray2_t

- the constructor are poorly degisnged
- issue with the is_null()
  - unable to get bytearray2_t with no reserved
  - so unable to get is_null() being true
  - is that a big problem ? when do i need to recreate a null bytearray2_t ?
- issue with the nocopy ctor from datum_t
  - 2 possibility: by ctor or by custom function ::work_on_datum_nocopy()
    - the second is more explicit but more dirty
    - the first change the semantic of a previously existing function
  - if any conversion from the datum_t via constructor is done via nocopy, i change
    the semantic as it was doing the copy before
  - how frequently i use this bytearray_t(datum_t) ?
    - maybe i could monitor this
    - i dont think it is frequently used
    - if so, maybe i could check if it is ok
  - let it with the work_on_datum_nocopy() first, then do the transition ?
  - is it even clear that doing bytearray2_t(datum2_t) doing nocopy is a good thing ?
    - yep it seems cleaner but it does a dangerous operation in a unexplicit way
    - the nocopy is unusual and hard to debug in case of error
    - being implicit on this is dangerous
- write a nunit very strong for bytearray2_t

\par About transition
- ifdef USE_OBSOLETE_BYTEARRAY then use the obsolete one
  else use the current one
- keep the 2 in the filename
- remove the two in the object code
- same for datum2_t serial2_t bytearray2_t
- thus allow a smooth transition

*/

/* system include */
/* local include */
#include "neoip_bytearray2.hpp"
#include "neoip_math_shortcut.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_swarm_profile_t constant
#if 0	// value for production 
	// - TODO what are the good values ? impossible to determine as it depends a lot of
	//   the usage pattern of the caller
	const size_t	bytearray2_t::FULL_LENGTH_DFL		= 1024;
	const size_t	bytearray2_t::HEAD_FREELEN_DFL		= 0;
	const size_t	bytearray2_t::GROW_CHUNKLEN_DFL		= 256;
#else	// value for debug
	const size_t	bytearray2_t::FULL_LENGTH_DFL		= 1024;
	const size_t	bytearray2_t::HEAD_FREELEN_DFL		= 0;
	const size_t	bytearray2_t::GROW_CHUNKLEN_DFL		= 256;
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
void bytearray2_t::allocate_ctor(size_t new_full_length, size_t new_head_freelen
						, size_t new_grow_chunklen)	throw()
{
	// sanity check - the new_full_length MUST NOT be 0
	DBG_ASSERT( new_full_length > 0 );
	// sanity check - the new_head_freelen MUST be less than new_full_length
	DBG_ASSERT( new_head_freelen < new_full_length );
	// sanity check - the grow_chunklen MUST NOT be 0
	DBG_ASSERT( new_grow_chunklen > 0 );
	// init the internal data
	full_data	= datum2_t(new_full_length);
	used_ptr	= full_data.char_ptr() + new_head_freelen;
	used_len	= 0;
	grow_chunklen	= new_grow_chunklen;
}

/** \brief Constructor to allocate the bytearray2_t with no data
 */
bytearray2_t::bytearray2_t(size_t new_full_length, size_t new_head_freelen, size_t new_grow_chunklen)throw()
{
	allocate_ctor(new_full_length, new_head_freelen, new_grow_chunklen);
}

/** \brief Constructor from a datum_t WITH copy
 * 
 * - aka this will apply the bytearray2_t directly on the datum2_t 
 */
bytearray2_t::bytearray2_t(const datum2_t &datum)				throw()
{
	// allocate the object
	allocate_ctor(datum.length(), 0, GROW_CHUNKLEN_DFL);
	// copy the data from the datum2_t to this one
	tail_add( datum );
}

/** \brief Constructor from a datum_t WITHOUT copy
 * 
 * - aka this will apply the bytearray2_t directly on the datum2_t 
 * - this is typically usefull when parsing received data
 *   - as the bytearray2_t growing in only to build data
 */
void	bytearray2_t::work_on_datum_nocopy(const datum2_t &datum)		throw()
{
	// init the internal data
	full_data	= datum2_t(datum.void_ptr(), datum.length(), datum2_t::NOCOPY);
	used_ptr	= full_data.char_ptr();
	used_len	= datum.length();
	grow_chunklen	= GROW_CHUNKLEN_DFL;
}

/** \brief Copy constructor
 */
bytearray2_t::bytearray2_t(const bytearray2_t &other)			throw()
{
	// allocate the object
	allocate_ctor(other.full_length(), other.head_freelen(), other.get_grow_chunklen());
	// copy the data from the other bytearray2_t to this one
	tail_add( other.void_ptr(), other.length() );
}


/** \brief overload the = operator
 */
bytearray2_t &	bytearray2_t::operator=(const bytearray2_t & other)	throw()
{
	// if auto-assignment, do nothing
	if( this == &other )	return *this;

	// NOTE: no need to destroy this object before as it doesnt have explicit memory allocation
	
	// allocate the object
	allocate_ctor(other.full_length(), other.head_freelen(), other.get_grow_chunklen());
	// copy the used data from the other bytearray2_t to this one
	tail_add( other.void_ptr(), other.length() );

	// return the object itself
	return *this;
}

/** \brief reallocate this bytearray2_t adding space at the head and tail
 * 
 * - some funky stuff about transfert ownership with datum2_t::NOCOPY 
 *   and datum2_t::swap() in order to copy the used_data only once.
 */
void	bytearray2_t::reallocate(size_t head_inclen, size_t tail_inclen)	throw()
{
	size_t		new_head_freelen= head_freelen() + head_inclen;
	size_t		new_full_length	= full_length() + head_inclen + tail_inclen;
	// keep the old_used_data to recopy it on the reallocated bytearray2_t
	datum2_t	old_used_data	= to_datum(datum2_t::NOCOPY);
	// transfert the ownership of full_data to a var in stack
	// - it allow not to free the data during the allocate_ctor
	datum2_t	old_full_data;
	old_full_data.swap(full_data);
	// allocate the object
	allocate_ctor(new_full_length, new_head_freelen, grow_chunklen);
	// copy the used data from the other bytearray2_t to this one
	tail_add( old_used_data );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			allocation at the head/tail
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief ensure there is at least nbyte available at the head and return a pointer to its begining
 * 
 * - NOTE: it change the used_ptr/used_len
 */
char *	bytearray2_t::head_alloc(size_t nbyte)		throw()
{
	// if the freelen at the head is not enought for the allocation, reallocate the bytearray2_t
	if( head_freelen() < nbyte ){
		size_t	missing_len	= nbyte - head_freelen();
		size_t	len_to_alloc	= grow_chunklen * ceil_div(missing_len, grow_chunklen);
		reallocate( len_to_alloc, 0 );
	}
	// update the used_ptr
	used_ptr	-= nbyte;
	// update the used_len
	used_len	+= nbyte;	
	// return a pointer on the begining of the allocated zone
	return used_ptr;
}

/** \brief ensure there is at least nbyte available at the tail and return a pointer to its begining
 * 
 * - NOTE: it change the used_ptr/used_len
 */
char *	bytearray2_t::tail_alloc(size_t nbyte)		throw()
{
	// if the freelen at the tail is not enought for the allocation, reallocate the bytearray2_t
	if( tail_freelen() < nbyte ){
		size_t	missing_len	= nbyte - tail_freelen();
		size_t	len_to_alloc	= grow_chunklen * ceil_div(missing_len, grow_chunklen);
		reallocate( 0, len_to_alloc );
	}
	
	// update the used_len
	used_len	+= nbyte;
	// return a pointer on the begining of the allocated zone
	return used_ptr + used_len - nbyte;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			free at the head/tail
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Free nbyte at the head
 */
void	bytearray2_t::head_free(size_t nbyte)		throw()
{
	// sanity check - the nbyte MUST NOT be greater that the used_length()
	DBG_ASSERT( nbyte <= used_length() );
	// update the used_ptr
	used_ptr	+= nbyte;
	// update the used_len
	used_len	-= nbyte;
}

/** \brief Free nbyte at the tail
 */
void	bytearray2_t::tail_free(size_t nbyte)		throw()
{
	// sanity check - the nbyte MUST NOT be greater that the used_length()
	DBG_ASSERT( nbyte <= used_length() );
	// update the used_len
	used_len	-= nbyte;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//             add data at the head/tail
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add data at the head
 */
bytearray2_t &	bytearray2_t::head_add(const void *data, size_t len)		throw()
{
	// allocate the required space
	char *	ptr	= head_alloc(len);
	// copy the data to it
	memcpy(ptr, data, len);
	// return the object itself
	return *this;
}

/** \brief Add data at the tail
 */
bytearray2_t &	bytearray2_t::tail_add(const void *data, size_t len)		throw()
{
	// allocate the required space
	char *	ptr	= tail_alloc(len);
	// copy the data to it
	memcpy(ptr, data, len);
	// return the object itself
	return *this;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 \ref bytearray2_t
 */
int	bytearray2_t::compare(const bytearray2_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
		
	// compare the used_datum
	return to_datum(datum2_t::NOCOPY).compare(other.to_datum(datum2_t::NOCOPY));
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bytearray2_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the output string
	oss << to_datum();
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END





