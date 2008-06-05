/*! \file
    \brief Definition of the \ref bt_cast_pidx_t

*/


/* system include */
#include <sstream>
/* local include */
#include "neoip_bt_cast_pidx.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
bt_cast_pidx_t::bt_cast_pidx_t()		throw()
{
	// set the fields to default values
	m_modulo	= std::numeric_limits<size_t>::max();
	m_index		= std::numeric_limits<size_t>::max();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	bt_cast_pidx_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( m_modulo == std::numeric_limits<size_t>::max() )	return true;
	if( m_index  == std::numeric_limits<size_t>::max() )	return true;
	// if this point is reached, the object is NOT null
	return false;
}

/** \brief Return true if the object is sane, false otherwise
 * 
 * - an object which is not sane, implies a bug
 */
bool	bt_cast_pidx_t::is_sane()	const throw()
{
	// a null object object is sane
	if( is_null() )			return true;
	// a non null object index() MUST be less than modulo() 
	if( index() >= modulo() )	return false;
	// if all previous tests passed this object is considered sane
	return true;
}

/** \brief Return true if the bt_cast_pidx_t 'is_in' the range_beg/range_end (both included)
 */
bool	bt_cast_pidx_t::is_in(size_t range_beg, size_t range_end)		const throw()
{
	// sanity check - the object MUST be sane and not null
	DBG_ASSERT( is_sane() );
	DBG_ASSERT( !is_null() );
	// sanity check - range_beg/end MUST be < modulo
	DBG_ASSERT( range_beg < modulo() );
	DBG_ASSERT( range_end < modulo() );	

	// if the range does not warp around the modulo
	if( range_beg <= range_end ){
		// if the index is less than the range_beg, it IS NOT in the range
		if( index() < range_beg )	return false;
		// if the index is more than the range_end, it IS NOT in the range
		if( index() > range_end )	return false;
		// else it IS in the range
		return true;
	}
	
	// NOTE: at this point, the range is known to wrap around the modulo
	DBG_ASSERT( range_beg > range_end );
	
	// if the index is >= than range_beg, it IS included in the range
	if( index() >= range_beg )	return true;
	// if the index is <= than range_end, it IS included in the range
	if( index() <= range_end )	return true;
	// else it IS NOT in the range
	return false;
}

/** \brief Return the number of piece between this object and pieceidx_fwd
 *         under the assumption this object is *before* pieceidx_fwd
 */
size_t	bt_cast_pidx_t::is_nb_piece_before(size_t pieceidx_fwd)	const throw()
{
	// sanity check - the object MUST be sane and not null
	DBG_ASSERT( is_sane() );
	DBG_ASSERT( !is_null() );
	// sanity check - pieceidx_fwd MUST be < modulo
	DBG_ASSERT( pieceidx_fwd < modulo() );
	// if pieceidx_fwd is < index(), normalize it 
	// - NOTE: here normalize it means make it as if there was no modulo
	if( pieceidx_fwd < index() )	pieceidx_fwd	+= modulo();
	// return the distrance between the 2
	return pieceidx_fwd - index();
}

/** \brief Return the number of piece between this object and pieceidx_fwd
 *         under the assumption this object is *after* pieceidx_fwd
 */
size_t	bt_cast_pidx_t::is_nb_piece_after(size_t pieceidx_bwd)	const throw()
{
	size_t	tmp_idx	= index();
	// sanity check - the object MUST be sane and not null
	DBG_ASSERT( is_sane() );
	DBG_ASSERT( !is_null() );
	// sanity check - pieceidx_bwd MUST be < modulo
	DBG_ASSERT( pieceidx_bwd < modulo() );
	// if tmp_idx is < pieceidx_bwd, normalize it 
	// - NOTE: here normalize it means make it as if there was no modulo
	if( tmp_idx < pieceidx_bwd )	tmp_idx	+= modulo();
	// return the distrance between the 2
	return tmp_idx - pieceidx_bwd;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the new modulo
 */
bt_cast_pidx_t&	bt_cast_pidx_t::modulo(size_t new_val)	throw()
{
	// set the value
	m_modulo	= new_val;
	// return the object itself
	return *this;
}

/** \brief Set the new index
 */
bt_cast_pidx_t&	bt_cast_pidx_t::index(size_t new_val)	throw()
{
	// set the value
	m_index		= new_val;
	// sanity check - this object MUST be sane
	DBGNET_ASSERT( is_sane() ); 
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Arithmetic operator
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Post Incrementation operator
 */
bt_cast_pidx_t&	bt_cast_pidx_t::operator ++ ()			throw()
{
	// sanity check - this object MUST be sane and non null
	DBG_ASSERT( is_sane() );
	DBG_ASSERT( !is_null() );
	// add one to the index - with a modulo handling
	if( index()+1 != modulo() )	m_index++;
	else				m_index	= 0;
	// return the object itself
	return *this;
}

/** \brief Post Decrementation operator
 */
bt_cast_pidx_t&	bt_cast_pidx_t::operator -- ()			throw()
{
	// sanity check - this object MUST be sane and non null
	DBG_ASSERT( is_sane() );
	DBG_ASSERT( !is_null() );
	// sub one to the index - with a modulo handling
	if( index() != 0 )	m_index--;
	else			m_index	= modulo()-1;
	// return the object itself
	return *this;
}

/** \brief Addition and assignement with a size_t
 */
bt_cast_pidx_t&	bt_cast_pidx_t::operator += (size_t &other)	throw()
{
	// sanity check - this object MUST be sane and non null
	DBG_ASSERT( is_sane() );
	DBG_ASSERT( !is_null() );
	// do the addition - with a modulo handling
	m_index	+= other;
	m_index %= modulo();
	// return the object itself
	return *this;
}

/** \brief Subtraction and assignement with a size_t
 */
bt_cast_pidx_t&	bt_cast_pidx_t::operator -= (size_t &other)	throw()
{
	// sanity check - this object MUST be sane and non null
	DBG_ASSERT( is_sane() );
	DBG_ASSERT( !is_null() );
	// do the addition - with a modulo handling
	m_index	+= modulo() - (other % modulo());
	m_index %= modulo();
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	bt_cast_pidx_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss << index() << " mod " << modulo(); 
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bt_cast_pidx_t
 * 
 * - support null bt_cast_pidx_t
 */
serial_t& operator << (serial_t& serial, const bt_cast_pidx_t &cast_pidx)		throw()
{
	// sanity check - the object MUST be sane
	DBG_ASSERT( cast_pidx.is_sane() );
	// serialize each field of the object
	serial << cast_pidx.modulo();
	serial << cast_pidx.index();
	// return serial
	return serial;
}

/** \brief unserialze a bt_cast_pidx_t
 * 
 * - support null bt_cast_pidx_t
 */
serial_t& operator >> (serial_t & serial, bt_cast_pidx_t &cast_pidx)		throw(serial_except_t)
{	
	size_t		modulo;
	size_t		index;
	// reset the destination variable
	cast_pidx	= bt_cast_pidx_t();
	// unserialize the data
	serial >> modulo;
	serial >> index;
	// set the returned variable
	cast_pidx.modulo(modulo).index(index);
	// return serial
	return serial;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc bt_cast_pidx_t
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief doserialize xmlrpc for bt_cast_pidx_t
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const bt_cast_pidx_t &cast_pidx)	throw()
{
	// sanity check - the object MUST be sane
	DBG_ASSERT( cast_pidx.is_sane() );
	// serialize the object
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;	
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG( "modulo" );
	xmlrpc_build		<< cast_pidx.modulo();
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG( "index" );
	xmlrpc_build		<< cast_pidx.index();
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_END;
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;
	// return the object itself
	return xmlrpc_build;
}

/** \brief unserialize xmlrpc for bt_cast_pidx_t
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, bt_cast_pidx_t &cast_pidx)	throw(xml_except_t)
{
	// define the macro
	size_t	modulo;
	size_t	index;
	// unserialize the object
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_BEG;
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG( "modulo" );
	xmlrpc_parse		>> modulo;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse 	>> xmlrpc_parse_t::MEMBER_BEG( "index" );
	xmlrpc_parse		>> index;
	xmlrpc_parse	>> xmlrpc_parse_t::MEMBER_END;
	xmlrpc_parse >> xmlrpc_parse_t::STRUCT_END;

	// set the returned variable
	cast_pidx.modulo(modulo).index(index);

	// return the object itself
	return xmlrpc_parse;
}
NEOIP_NAMESPACE_END

