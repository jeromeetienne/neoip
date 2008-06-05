/*! \file
    \brief Defintion of \ref bytearray_t

\par TODO
- may be ported on top of datum_t for the allocation/free ?
    
*/

/* system include */
/* local include */
#include "neoip_bytearray.hpp"
#include "neoip_datum.hpp"
#include "neoip_base64.hpp"
#include "neoip_assert.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

#if 0
// definition of \ref bt_swarm_profile_t constant
const size_t	bytearray_t::RESERVED_SIZE		= 2048;
const size_t	bytearray_t::START_OFFSET		= 256;
const size_t	bytearray_t::GROW_CHUNK_LEN		= 256;
// end of constants definition


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Internal contructor
 */
void	bytearray_t::internal_ctor(const void *ptr, size_t len)		throw()
{
	size_t		reserved	= std::max(len, bytearray_t::RESERVED_SIZE);
	// init the glib array
	glib_barray	= g_byte_array_sized_new(reserved);
	// put the datum in it
	glib_barray	= g_byte_array_append(glib_barray, (guint8*)ptr, len);
	// sanity check - the glib_barray MUST be non null
	DBG_ASSERT( glib_barray != NULL );
}

/** \brief Internal destructor
 */
void	bytearray_t::internal_dtor()					throw()
{
	// sanity check - the glib_barray MUST be non null
	DBG_ASSERT( glib_barray != NULL );
	// free the glib_barray and the memory zone attached to it
	g_byte_array_free( glib_barray, 1 );
}


/** \brief Constructor from value
 */
bytearray_t::bytearray_t(const size_t reserved_size, const size_t start_offset
					, const size_t grow_chunk_len)	throw()
{
	// sanity check - start_offset MUST be <= reserved_size
	DBG_ASSERT( start_offset <= reserved_size );
	// sanity check - the grow_chunk_len MUST be > 0
	DBG_ASSERT( grow_chunk_len > 0 );
	// init the glib array
	glib_barray	= g_byte_array_sized_new(reserved_size);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     operator = and copy constructor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief assignement operator
 */
bytearray_t &bytearray_t::operator = (const bytearray_t &other)			throw()
{
	// if the assignment over itself, do nothing
	if( this == &other )	return *this;
	// destruct the old value
	internal_dtor();
	// initiator with the new values
	internal_ctor(other.get_ptr(), other.get_len());
	// return a reference over itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 \ref bytearray_t
 */
int	bytearray_t::compare(const bytearray_t &other)	const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
		
	// handle them by the length - if both are not of the same length, the shorter is less than
	if( get_len() < other.get_len() )	return -1;
	if( get_len() > other.get_len() )	return +1;
	// NOTE: here both have the same length

	// if both length are equal, compare the data
	return memcmp( get_data(), other.get_data(), get_len() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a datum containing the data of the packet
 * 
 * @return a datum containing the data of the packet, may be null if the bytearray_t is null
 */
datum_t	bytearray_t::to_datum(datum_flag_t flag)	const throw()
{
	// return null datum_t is the bytearray_t is null
	if( is_null() )	return datum_t();
	// return a datum_t containing the data
	return datum_t(get_data(), get_len(), flag);
}

/** \brief Convert the bytearray_t to a base64 string
 */
std::string	bytearray_t::to_base64()		const throw()
{
	return base64_t::encode(get_data(), get_len());
}

/** \brief Build a bytearray_t from a base64 encoded string
 */
bytearray_t	bytearray_t::from_base64(const std::string &str)	throw()
{
	return bytearray_t( base64_t::decode(str) );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       PEEK functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return the len byte at the head but DONT remove them from the bytearray_t
 */
datum_t	bytearray_t::head_peek(size_t len)		throw(serial_except_t)
{
	// sanity check - the length MUST be >= bytearray_t::get_len()
	if( len > get_len() )	nthrow_serial_plain("ByteArray Data too short.");
	// copy the peekd data
	datum_t	datum( (uint8_t *)get_data(), len );
	// returned the peekd data
	return datum;
}

/** \brief return the len byte at the tail but DONT them from the bytearray_t
 */
datum_t	bytearray_t::tail_peek(size_t len)		throw(serial_except_t)
{
	// sanity check - the length MUST be >= bytearray_t::get_len()
	if( len > get_len() )	nthrow_serial_plain("ByteArray Data too short.");
	// copy the peekd data
	datum_t	datum( (uint8_t *)get_data()+get_len()-len, len );
	// returned the peekd data
	return datum;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       Consume functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return the len byte at the head and remove them from the bytearray_t
 */
datum_t	bytearray_t::head_consume(size_t len)		throw(serial_except_t)
{
	// copy the consumed data
	datum_t	datum	= head_peek(len);
	// remove the consumed data
	head_remove( len );
	// returned the consumed data
	return datum;
}

/** \brief return the len byte at the tail and remove them from the bytearray_t
 */
datum_t	bytearray_t::tail_consume(size_t len)		throw(serial_except_t)
{
	// copy the consumed data
	datum_t	datum	= tail_peek(len);
	// remove the consumed data
	tail_remove( len );
	// returned the consumed data
	return datum;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       add/remove at the tail/head
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief set the size of the bytearray_t
 */
bytearray_t &	bytearray_t::set_size(size_t len)		throw()
{
	// update the glib_barray
	glib_barray	= g_byte_array_set_size(glib_barray, len);	
	// sanity check - now the size() MUST be the len variable
	DBG_ASSERT( size() == len );
	// return the object itself
	return *this;	
}

/** \brief add data at the head
 */
bytearray_t &	bytearray_t::head_add(const void *data, size_t len)	throw()
{
	// add the data
	glib_barray	= g_byte_array_prepend(glib_barray, (guint8 *)data, len);
	// return the object itself
	return *this;
}

/** \brief remove data at the head
 */
bytearray_t &	bytearray_t::head_remove(size_t len)		throw(serial_except_t)
{
	// sanity check - the length MUST be >= bytearray_t::get_len()
	if( len > get_len() )	nthrow_serial_plain("Data too short.");
	// if there is nothing to remove, return now
	if( len == 0 )	return *this;
	// remove the data
	glib_barray	= g_byte_array_remove_range(glib_barray, 0, len);	
	// return the object itself
	return *this;	
}

/** \brief add data at the tail
 */
bytearray_t &	bytearray_t::tail_add(const void *data, size_t len)	throw()
{
	// add the data
	glib_barray	= g_byte_array_append(glib_barray, (guint8 *)data, len);
	// return the object itself
	return *this;
}

/** \brief remove data at the tail
 */
bytearray_t &	bytearray_t::tail_remove(size_t len)		throw(serial_except_t)
{
	// sanity check - the length MUST be >= bytearray_t::get_len()
	if( len > get_len() )	nthrow_serial_plain("Data too short.");
	// if there is nothing to remove, return now
	if( len == 0 )	return *this;
	// remove the data
	glib_barray	= g_byte_array_remove_range(glib_barray, get_len() - len, len);
	// return the object itself
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bytearray_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the output string
	oss << this->to_datum(datum_t::NOCOPY);
	// return the just built string
	return oss.str();
}
#endif

NEOIP_NAMESPACE_END





