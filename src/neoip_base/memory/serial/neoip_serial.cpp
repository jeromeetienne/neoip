/*! \file
    \brief Definition of \ref serial_t

\par About is_null()
- a serial_t may be null and so is_null() return true
- if is_null(), length() == 0 and all pointer == NULL
- the serial_t grow_length is equal to serial_t::GROW_LENGTH_DFL  

\par Typical Usage and performance advice
- building a packet - aka serializing several stuff before sending it
  - several tail_add() to build and then send the used_data
  - if the serial_t is preallocated to be large enougth during the ctor, no realloc happen
- parsing a packet - aka unserializing several stuff
  - several range() to get the data and tail_free() to remove them
  - in this case, no reallocate happen
  - it may use work_on_data_nocopy() to avoid copy the original data
- having a serial_t to parse a bytestream protocol - e.g. bittorrent in bt_swarm_full_t
  - in this case a single serial_t continuously receiving append() from the socket
    and then parsing is done over it.
  - if the serial_t is preallocated to be large enougth during the ctor, no realloc happen
    but it is not that important in this case as the serial_t is long lived
  - due to the memshift, a lot of reallocation is avoided.
  - NOTE: in this case, if the serial_t is already empty when a new packet is 
    received, to parse directly from the packet and then copy the remaining
    to the serial_t would unnessary copying

\par About Statistics in serial_stat_t
- it is only for tuning/debugging and is compilable on option
- the purpose of serial_stat_t is to do statistic on serial_t in order to properly
  tune the serial_t usage.
  - e.g. determine the preallocation size for the 'building a packet' usage
  - e.g. determine how many copy of the data are made in order to detect if
    some may be avoided
- ultimatly one want to avoid any reallocation, memshift and useless serial_t object copy
- TODO currently which statistic to keep are unclear so it is more a stub
  - for example: nb_objcopy, nb_realloc, nb_memmove
    - stuff done on this instance
    - keep the value local to this instance and a sum for each copy from another object
  - in order to ease the modification of serial_stat_t object declaration, it has been
    coded such as modifying serial_stat_t dont trigger a recompilation of all serial_t 
    caller.

\par big picture
- DONE handle the serial_stat_
- DONE no bug remains as far as i know
  - the 'always forward' bug is fixed by the 'memshift'
  - the 'no null' bug is fixed as is_null is possible now
- comment the stuff a lot
- limit the public API of serial_t to the minimum
  - determine which API return an exception on error, and which does DBG_ASSERT
- find proper names for the API
  - make it more consistant
- make a better nunit - more consistant as well
- put the unserial_peek outside the class
  - make a unit test for it
- then try to include it

\par Note
- limit the reallocate()
  - do a tail_grow()
    - what do i do about the head_freelen in the new allocation ? how much
  - do a head_grow()
    - what do i do about the tail_freelen in the new allocation ? how much
  - for tail_alloc()
    - if the free room is available at head, just memmove
    - yep but always the issue of how much head_freelen in the new allocation ?



*/

/* system include */
/* local include */
#include "neoip_serial.hpp"
#include "neoip_math_shortcut.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_serial_stat.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// define the #define to optionally compile serial_stat_t code
#if 0
#	define SERIAL_STAT_CODE(stat_code)	stat_code
#else
#	define SERIAL_STAT_CODE(stat_code)	
#endif

// definition of \ref bt_swarm_profile_t constant
#if 0	// value for production 
	// - TODO what are the good values ? impossible to determine as it depends a lot of
	//   the usage pattern of the caller
	const size_t	serial_t::INIT_LENGTH_DFL		= 1024;
	const size_t	serial_t::HEAD_LENGTH_DFL		= 0;
	const size_t	serial_t::GROW_LENGTH_DFL		= 256;
#else	// value for debug
	const size_t	serial_t::INIT_LENGTH_DFL		= 256;
	const size_t	serial_t::HEAD_LENGTH_DFL		= 0;
	const size_t	serial_t::GROW_LENGTH_DFL		= 256;
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
serial_t::serial_t()	throw()
{
	// zero some field
	used_ptr	= NULL;
	used_len	= 0;
	grow_length	= GROW_LENGTH_DFL;
	// allocate the serial_stat_t
	SERIAL_STAT_CODE	(serial_stat	= nipmem_new serial_stat_t();	)
	// sanity check - after modifying the object, it MUST always be sane	
	DBG_ASSERT( is_sane() );
}

/** \brief Default constructor
 */
serial_t::~serial_t()	throw()
{
	// sanity check - after modifying the object, it MUST always be sane	
	DBG_ASSERT( is_sane() );
	// free the serial_stat_t
	SERIAL_STAT_CODE	(nipmem_delete	serial_stat;			)
}

/** \brief Constructor to preallocate the serial_t with no data
 */
serial_t::serial_t(size_t new_init_length, size_t new_head_length, size_t new_grow_length)	throw()
{
	// allocate the serial_stat_t
	SERIAL_STAT_CODE	(serial_stat	= nipmem_new serial_stat_t();	)
	// allocate the object
	allocate_ctor(new_init_length, new_head_length, new_grow_length);
	// sanity check - after modifying the object, it MUST always be sane	
	DBG_ASSERT( is_sane() );
}

/** \brief Constructor from a buffer (void *, length) WITH copy
 * 
 * - aka this will apply the serial_t directly on the datum_t 
 */
void	serial_t::internal_ctor_docopy(const void *data, size_t len)		throw()
{
	// allocate the serial_stat_t
	SERIAL_STAT_CODE	(serial_stat	= nipmem_new serial_stat_t();	)
#if 1
	// special case if other is empty
	if( len == 0 ){
		full_data	= datum_t();
		used_ptr	= NULL;
		used_len	= 0;
		grow_length	= GROW_LENGTH_DFL;
		// sanity check - after modifying the object, it MUST always be sane	
		DBG_ASSERT( is_sane() );
		return;	
	}
#endif
	// allocate the object
	allocate_ctor(len, 0, GROW_LENGTH_DFL);
	// copy the data from the buffer to this one
	tail_add(data, len);
	// sanity check - after modifying the object, it MUST always be sane	
	DBG_ASSERT( is_sane() );
}

/** \brief Copy constructor
 */
serial_t::serial_t(const serial_t &other)			throw()
{
	// allocate the serial_stat_t
	SERIAL_STAT_CODE	(serial_stat	= nipmem_new serial_stat_t();	)
	// log to debug
	KLOG_DBG("other.used_length()=" << other.length() << " " << other.full_length() 
				<< " " << other.head_freelen() << " " << other.get_grow_length());	
#if 1
	// special case if other is empty
	if( other.empty() ){
		full_data	= datum_t();
		used_ptr	= NULL;
		used_len	= 0;
		grow_length	= GROW_LENGTH_DFL;
		// sanity check - after modifying the object, it MUST always be sane	
		DBG_ASSERT( is_sane() );
		return;	
	}
#endif
	
	// allocate the object
	allocate_ctor(other.full_length(), other.head_freelen(), other.get_grow_length());
	// copy the data from the other serial_t to this one
	tail_add( other.void_ptr(), other.length() );
	// sanity check - after modifying the object, it MUST always be sane	
	DBG_ASSERT( is_sane() );
}


/** \brief overload the = operator
 */
serial_t &	serial_t::operator=(const serial_t & other)	throw()
{
	// if auto-assignment, do nothing
	if( this == &other )	return *this;

	// NOTE: no need to destroy this object before as it doesnt have explicit memory allocation
	// - the memory is allocated via datum_t

#if 0	// TODO im not sure about this one, in fact this cause trouble in the 
	//      preallocation system. e.g. see file_aread_t preallocation

	// special case if other is empty
	if( other.empty() ){
		full_data	= datum_t();
		used_ptr	= NULL;
		used_len	= 0;
		grow_length	= GROW_LENGTH_DFL;
		// sanity check - after modifying the object, it MUST always be sane	
		DBG_ASSERT( is_sane() );
		return *this;
	}
#endif
	
	// allocate the object
	allocate_ctor(other.full_length(), other.head_freelen(), other.get_grow_length());
	// copy the used data from the other serial_t to this one
	tail_add( other.void_ptr(), other.length() );
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
void serial_t::allocate_ctor(size_t new_init_length, size_t new_head_length, size_t new_grow_length)throw()
{
	// log to debug
	KLOG_DBG("new_init_length=" << new_init_length << " new_head_length=" << new_head_length
				<< " new_grow_length=" << new_grow_length);
	// sanity check - the new_init_length MUST NOT be 0
	DBG_ASSERT( new_init_length > 0 );
	// sanity check - the new_head_length MUST be less than new_init_length
	DBG_ASSERT( new_head_length <= new_init_length );
	// sanity check - the grow_length MUST NOT be 0
	DBG_ASSERT( new_grow_length > 0 );
	// init the internal data
#if 0	// TODO this create a useless memcpy i think
	full_data	= datum_t(new_init_length);
#else
	datum_t	tmp(new_init_length);
	full_data.swap(tmp);
#endif	
	used_ptr	= full_data.char_ptr() + new_head_length;
	used_len	= 0;
	grow_length	= new_grow_length;
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
}

/** \brief Constructor from a datum_t WITHOUT copy
 * 
 * - aka this will apply the serial_t directly on the datum_t 
 * - this is typically usefull when parsing received data
 *   - as the serial_t growing in only to build data
 */
void	serial_t::work_on_data_nocopy(const void *data, size_t len)		throw()
{
	// init the internal data
#if 0	// TODO this create a useless memcpy i think
	full_data	= datum_t(data, len, datum_t::NOCOPY);
#else
	datum_t	tmp(data, len, datum_t::NOCOPY);
	full_data.swap(tmp);
#endif	
	used_ptr	= full_data.char_ptr();
	used_len	= len;
	grow_length	= GROW_LENGTH_DFL;
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
}

/** \brief reallocate this serial_t adding space at the head and tail
 * 
 * - some funky stuff about transfert ownership with datum_t::NOCOPY 
 *   and datum_t::swap() in order to copy the used_data only once.
 */
void	serial_t::reallocate(size_t head_inclen, size_t tail_inclen)	throw()
{
	// compute the new used length
	size_t	new_used_length	= used_length() + head_inclen + tail_inclen;

	// compute the new_full_length
	size_t	new_full_length;
	if( new_used_length <= INIT_LENGTH_DFL ){
		// if the new_used_length is <= INIT_LENGTH_DFL, set new_full_length to INIT_LENGTH_DFL
		new_full_length	= INIT_LENGTH_DFL;
	}else{
		// if the new_used_length is > INIT_LENGTH_DFL, set new_full_length to its modulo grow_length
		new_full_length	= grow_length * ceil_div(new_used_length, grow_length);
	}

	// keep the old_used_data to recopy it on the reallocated serial_t
	datum_t	old_used_data	= to_datum(datum_t::NOCOPY);
	// transfert the ownership of full_data to a var in stack
	// - it allow not to free the data during the allocate_ctor()
	datum_t	old_full_data;
	old_full_data.swap(full_data);

	// allocate the object with the used data in the middle of the full_data
	size_t	new_free_length	= new_full_length - new_used_length;
	allocate_ctor(new_full_length, new_free_length/2 + head_inclen, grow_length);
	// copy the used data from the other serial_t to this one
	tail_add( old_used_data );
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
}

/** \brief Return true if the serial_t is sane, false otherwise
 * 
 * - NOTE: it MUST always be sane, if not it is a bug
 */
bool	serial_t::is_sane()							const throw()
{
	// check the is_null() conditions
	if( full_data.is_null() )	DBG_ASSERT( void_ptr() == NULL );
	if( full_data.is_null() )	DBG_ASSERT( length() == 0 );
	// check that the used_ptr is pointing withing the full_data
	DBG_ASSERT( used_ptr >= full_data.char_ptr() );
	DBG_ASSERT( used_ptr + used_len <=  full_data.char_ptr() + full_data.length() );
	// check that the used_len is within the full_data
	DBG_ASSERT( used_len <= full_data.length() );	
	DBG_ASSERT( used_len <= full_data.length() - (used_ptr-full_data.char_ptr()) );
	// check that there is a consistency between the various length functions
	DBG_ASSERT( head_freelen() + used_length() + tail_freelen() == full_length() );
	// if all previous tests passed, it is considered sane
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			allocation at the head/tail
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief ensure there is at least req_len available at the head and return a pointer to its begining
 * 
 * - NOTE: it change the used_ptr/used_len
 */
char *	serial_t::head_alloc(size_t req_len)		throw()
{
	// if the head_freelen() is < the requested length, make room for nbyte
	if( head_freelen() < req_len ){
		// if the total free length is LESS than the requested elgnth, do a rellocation
		if( tail_freelen() + head_freelen() < req_len ){
			reallocate(req_len, 0);
			// update the serial_stat_t
			SERIAL_STAT_CODE	(serial_stat->head_realloc() += 1;)
		}else{	// if total free length is MORE or EQUAL than the requested length, do a memshift
			// - shift the used data to get 2/3 of the full_freelen for the head
			// - based on the assumption that tail_add is called more often than head_add
			size_t	new_free_len	= head_freelen() + tail_freelen() - req_len;
			char *	new_used_ptr	= full_data.char_ptr()  + 2*new_free_len/3;
			DBG_ASSERT(full_data.char_ptr());
			// move the data
			memmove(new_used_ptr, used_ptr, used_len);
			// set the used_ptr to the new value
			used_ptr	= new_used_ptr;
			// update the serial_stat_t
			SERIAL_STAT_CODE	(serial_stat->head_memshift() += 1;)
		}
	}
	// update the used_ptr
	used_ptr	-= req_len;
	// update the used_len
	used_len	+= req_len;
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
	// return a pointer on the begining of the allocated zone
	return used_ptr;
}

/** \brief ensure there is at least req_len available at the tail and return a pointer to its begining
 * 
 * - NOTE: it change the used_ptr/used_len
 */
char *	serial_t::tail_alloc(size_t req_len)		throw()
{
	// if the tail_freelen() is < the requested length, make room for req_len
	if( tail_freelen() < req_len ){
		// if the total free length is LESS than the requested length, do a rellocation
		if( head_freelen() + tail_freelen() < req_len ){
			reallocate(0, req_len);
			// update the serial_stat_t
			SERIAL_STAT_CODE	(serial_stat->tail_realloc() += 1;)			
		}else{	// if total free length is MORE or EQUAL than the requested length, do a memshift
			// - put the data at the begining of the full_data
			// - based on the assumption that tail_add is called more often than head_add
			char *	new_used_ptr	= full_data.char_ptr();
			DBG_ASSERT(full_data.char_ptr());
			// move the data
			memmove(new_used_ptr, used_ptr, used_len);
			// set the used_ptr to the new value
			used_ptr	= new_used_ptr;
			// update the serial_stat_t
			SERIAL_STAT_CODE	(serial_stat->tail_memshift() += 1;)			
		}
	}
	// update the used_len
	used_len	+= req_len;
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
	// return a pointer on the begining of the allocated zone
	return used_ptr + used_len - req_len;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			free at the head/tail
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Free nbyte at the head
 */
void	serial_t::head_free(size_t nbyte)		throw()
{
	// sanity check - the nbyte MUST NOT be greater that the used_length()
	DBG_ASSERT( nbyte <= used_length() );
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
	// log to debug
	KLOG_DBG("used_ptr=" << (void*)used_ptr << " used_len=" << used_len<< " full_data.ptr=" 
			<< full_data.void_ptr() << " full_data.len=" << full_data.length() );
	// update the used_ptr
	used_ptr	+= nbyte;
	// update the used_len
	used_len	-= nbyte;
	// log to debug
	KLOG_DBG("used_ptr=" << (void*)used_ptr << " used_len=" << used_len<< " full_data.ptr=" 
			<< full_data.void_ptr() << " full_data.len=" << full_data.length() );
	KLOG_DBG("full_data.char_ptr() + full_data.length()=" << (void *)(full_data.char_ptr() + full_data.length()));
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
}

/** \brief Free nbyte at the tail
 */
void	serial_t::tail_free(size_t nbyte)		throw()
{
	// sanity check - the nbyte MUST NOT be greater that the used_length()
	DBG_ASSERT( nbyte <= used_length() );
	// update the used_len
	used_len	-= nbyte;
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//             add data at the head/tail
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add data at the head
 */
serial_t &	serial_t::head_add(const void *data, size_t len)		throw()
{
	// allocate the required space
	char *	ptr	= head_alloc(len);
	// copy the data to it
	memcpy(ptr, data, len);
	// update the serial_stat_t
	SERIAL_STAT_CODE	(serial_stat->len_copied() += len;)
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
	// return the object itself
	return *this;
}

/** \brief Add data at the tail
 */
serial_t &	serial_t::tail_add(const void *data, size_t len)		throw()
{
	// allocate the required space
	char *	ptr	= tail_alloc(len);
	// copy the data to it
	memcpy(ptr, data, len);
	// update the serial_stat_t
	SERIAL_STAT_CODE	(serial_stat->len_copied() += len;)
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
	// return the object itself
	return *this;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 \ref serial_t
 */
int	serial_t::compare(const serial_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
		
	// compare the used_datum
	return to_datum(datum_t::NOCOPY).compare(other.to_datum(datum_t::NOCOPY));
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string serial_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// sanity check - after modifying the object, it MUST always be sane
	DBG_ASSERT( is_sane() );
	// handle the null case
	if( is_null() )	return "null";
	// build the output string
	oss << to_datum();
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END
