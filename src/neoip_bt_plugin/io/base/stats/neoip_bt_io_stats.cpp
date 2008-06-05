/*! \file
    \brief Definition of the \ref bt_io_stats_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_io_stats.hpp"
#include "neoip_file_range.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_io_stats_t::bt_io_stats_t()	throw()
{
	// set the initial value
	totlen_read	( 0 );
	totlen_write	( 0 );
	nb_read		( 0 );
	nb_write	( 0 );

	nb_read_beg_align16k	( 0 );
	nb_read_len_align16k	( 0 );
	nb_write_beg_align16k	( 0 );
	nb_write_len_align16k	( 0 );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief update bt_io_stats_t for read this file_range_t
 */
bt_io_stats_t &	bt_io_stats_t::update_for_read(const file_range_t &file_range)	throw()
{
	// update the object
	nb_read		( nb_read() + 1 );
	totlen_read	( totlen_read() + file_range.length() );

	if( file_range.beg() % (16*1024) == 0 )
		nb_read_beg_align16k( nb_read_beg_align16k() + 1 );
	if( file_range.length() % (16*1024) == 0 )
		nb_read_len_align16k( nb_read_len_align16k() + 1 );
	// return the object itself
	return *this;
}

/** \brief update bt_io_stats_t for read this file_range_t
 */
bt_io_stats_t &	bt_io_stats_t::update_for_write(const file_range_t &file_range)	throw()
{
	// update the object
	nb_write	( nb_write() + 1 );
	totlen_write	( totlen_write() + file_range.length() );

	if( file_range.beg() % (16*1024) == 0 )
		nb_write_beg_align16k( nb_write_beg_align16k() + 1 );
	if( file_range.length() % (16*1024) == 0 )
		nb_write_len_align16k( nb_write_len_align16k() + 1 );
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        display function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	bt_io_stats_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// build the string to return
	oss << "[" << "totlen_read="	<< totlen_read();
	oss << " " << "totlen_write="	<< totlen_write();
	oss << " " << "nb_read="	<< nb_read();
	oss << " " << "nb_write="	<< nb_write();
	oss << " " << "nb_read_beg_align16k="	<< nb_read_beg_align16k();
	oss << " " << "nb_read_len_align16k="	<< nb_read_len_align16k();
	oss << " " << "nb_write_beg_align16k="	<< nb_write_beg_align16k();
	oss << " " << "nb_write_len_align16k="	<< nb_write_len_align16k();
	oss << "]";
	
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END

