/*! \file
    \brief Header of the \ref slidwin_t
    
*/


#ifndef __NEOIP_SLIDWIN_HPP__ 
#define __NEOIP_SLIDWIN_HPP__ 
/* system include */
#include <iostream>
#include <sstream>
#include <vector>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"
#include "neoip_assert.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \ref class to handle a slidding window
 * 
 * - T_ELEM is the type of data stored in the slidding window
 * - T_INDEX is the type of the index
 *   - this may be a integer, e.g. uint32_t for the antireplay window ala IPsec
 *     or a wrap around integer, e.g. wai16_t
 */
template <typename T_ELEM, typename T_INDEX> class slidwin_t : NEOIP_COPY_CTOR_ALLOW {
private:
	T_INDEX			index_begin;	//!< the index of the first element of the window
	T_ELEM			dfl_value;	//!< the default value put in newly created elements
	size_t			win_size;	//!< the width of the window
	T_INDEX			win_offset;	//!< as the window is handled as a rotating buffer,
						//!< it show the physical index of the index_begin
						//!< in the window
	std::vector<T_ELEM>	window;		//!< the window which actually contains the value

	void	remove_elem(const T_INDEX &elem_index)	const throw() {
		// TODO
		// - it can be interesting to have a fast sum of the whole window
		//   - for example to get a good rate estimator
	}

	/** \brief If necessary, make the window slide to make index exists
	 */
	void	slide_if_needed( const T_INDEX &dst_index )	throw() {
		// sanity check - the destination index MUST be >= index_begin
		DBG_ASSERT( dst_index >= index_begin );
		// if the destination index is already in the window, dont slide
		if( dst_index <= get_index_last() )	return;

		// compute the new index begin
		T_INDEX	new_index_begin	= dst_index - T_INDEX(win_size - 1);

		// compute the number of elements to slide
		T_INDEX	slide_len	= dst_index - get_index_last();

		// if the window is fully wiped out, fill it with default value
		if( slide_len >= T_INDEX(win_size) ){
			// remove obsolete element
			for( T_INDEX i = index_begin; i <= get_index_last(); i++ )
				remove_elem( i );
			// setup the new index_begin
			index_begin = new_index_begin;
			// fill the whole window with the default value
			fill(dfl_value);
			return;
		}
		// NOTE: here the window is only partially slidded
		// remove the obsolete elements
		for( T_INDEX i = index_begin; i < new_index_begin; i++ ){
			remove_elem( i );
			window[cpu_idx(i)]	= dfl_value;
		}

		// setup the new win_offset
		win_offset	= (win_offset + slide_len) % win_size;
		// setup the new index_begin
		index_begin	= new_index_begin;

		std::stringstream	sstream;
		for( size_t i = 0; i < win_size; i++ )	sstream << " " << window[i];
		//KLOG_ERR( "Post " << sstream.str() );	
		//KLOG_ERR( "win_offset=" << win_offset << " slide_len=" << slide_len << "index_begin=" << index_begin );
	}
	size_t cpu_idx(const T_INDEX &index) 	const throw() {
		return (size_t)((win_offset + (index - index_begin)) % win_size);
	}
public:
	/** \brief Constructor
	 */
	slidwin_t(const size_t &win_size = 0, const T_ELEM &dfl_value = 0, T_INDEX index_begin = 0) throw() 
					: index_begin(index_begin), dfl_value(dfl_value)
					, win_size(win_size), window(win_size) {
		// set up the win_offset
		win_offset = 0;
		// init the window values
		fill( dfl_value );
	}
	/** \brief Destructor
	 */
	~slidwin_t()				throw()	{}

	//! return the number of element of the sliding window
	size_t	size()			const throw() { return win_size;		}
	//! return the index of the first element of the sliding window
	T_INDEX	get_index_first()	const throw() {	return index_begin;		}
	//! return the index of the last element of the sliding window
	T_INDEX	get_index_last()	const throw() {	return index_begin + T_INDEX(win_size-1);	}
	

	/** \brief fill the whole sliding window with a specific value
	 */
	void	fill( T_ELEM value )		throw() {
		for( size_t i = 0; i < win_size; i++ ){
			window[i] = value;
		}
	}
	
	/** \brief return true if a given index exists in the sliding window
	 */
	bool	exist( const T_INDEX &index )	const throw() {
		if( index < get_index_first() )	return false;
		if( index > get_index_last() )	return false;
		return true;
	}

	/** \brief return the value of the element at the given index (NOTE: the element MUST exists)
	 */
	T_ELEM	get( const T_INDEX &index )	const throw() {
		// sanity check - the index MUST exist
		DBG_ASSERT( exist(index) );
		// return the element
		return window[cpu_idx(index)];
	}

	/** \brief assign a given value to the element at a given index. if the element
	 *         doesnt exists, the window is slid to make it exist.
	 * 
	 * @param index the index of the element to set. it MUST be >= \ref slidwin_t::get_index_first()
	 */
	void	set( const T_INDEX &index, const T_ELEM &value )	throw() {
		// make the window slide if necessary
		slide_if_needed( index );
		// sanity check - the index MUST exist
		DBG_ASSERT( exist(index) );
		//KLOG_ERR("setting index=" << index << " value=" << value<< " cpu_idx=" << cpu_idx(index));
		// set the element
		window[cpu_idx(index)]	= value;
	}
	
	// TODO to support the [] operator to get/set 


	/** \brief return the value of the element at the given index. if the element doesnt
	 *         exists, it return the default value.
	 */
	T_ELEM	get_default( const T_INDEX &index, const T_ELEM &dfl_value )	const throw() {
		// if the element doesnt exist return the default value
		if( exist(index) == false )	return dfl_value;
		// else return the actual value
		return get(index);
	}
};

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_SLIDWIN_HPP__  */










