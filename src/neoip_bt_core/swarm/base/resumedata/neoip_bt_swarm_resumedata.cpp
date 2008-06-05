/*! \file
    \brief Definition of the \ref bt_swarm_resumedata_t

\par Brief Description
bt_swarm_resumedata_t contains a snapshot of the current state of the 
bt_swarm_t. 
- This may be used to save the current state in order to resume a bt_swarm_t later.
- or this may be used to setup the initial-boot of a bt_swarm_t. 
  - in this case, the bt_swarm_resumedata_t is built by bt_swarm_resumedata_t::from_mfile() 
  - in this case, the name 'resumedata' is poorly choosen as it implies it 
    resume something. TODO find a better name 
    - before it was called bt_swarm_curstate_t... i dunno how good this name is


*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_swarm_resumedata.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_bt_mfile.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
bt_swarm_resumedata_t::bt_swarm_resumedata_t()		throw()
{
	// set the default value
	mfile_allocated	( false );
}

/** \brief Build a bt_swarm_resumedata_t out of a bt_mfile
 */
bt_swarm_resumedata_t	bt_swarm_resumedata_t::from_mfile(const bt_mfile_t &bt_mfile)	throw()
{
	bt_swarm_resumedata_t	swarm_resumedata;
	// set the only required field
	swarm_resumedata.bt_mfile	( bt_mfile );
	// sanity check - the result MUST be check().succeed()
	DBG_ASSERT( swarm_resumedata.check().succeed() );
	// return the just build bt_swarm_resumedata_t
	return swarm_resumedata;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the object is to be considered null, false otherwise
 */
bool	bt_swarm_resumedata_t::is_null()	const throw()
{
	// test if all the mandatory fields have acceptable values
	if( !bt_mfile().is_fully_init() )	return true;
	// if this point is reached, the object is NOT null
	return false;
}


/** \brief Return a bt_err_t if the bt_swarm_resumedata_t is not valid for this bt_mfile
 */
bt_err_t	bt_swarm_resumedata_t::check()		const throw()
{
	// if the bt_swarm_resumedata_t is null, return an error
	if( is_null() )							return bt_err_t::ERROR;
	// if the bt_mfile_t is not fully init, return an error
	if( bt_mfile().is_fully_init() == false )			return bt_err_t::ERROR;
	// if the bt_pieceavail_t size doesnt matched the needed one, return an error
	if( !pieceavail_local().is_null() && pieceavail_local().nb_piece() != bt_mfile().nb_piece() )
		return bt_err_t::ERROR;
	// if the bt_pieceprec_arr_t is non null, it MUST have the same size as bt_mfile_t::nb_piece()
	if( !pieceprec_arr().is_null() && pieceprec_arr().size() != bt_mfile().nb_piece() )
		return bt_err_t::ERROR;
	// return no error
	return bt_err_t::OK;
}
	
/** \brief Return a bitfield_t where only the partial piece are set
 */
bitfield_t	bt_swarm_resumedata_t::get_partavail_piece_bitfield()	const throw()
{
	file_size_inval_t	current_inval	= partavail_piece_inval();
	file_size_inval_t	piece_isavail_inval;
	bitfield_t		partavail_piece_bitfield;
	// sanity check - the bt_mfile field MUST NOT be null
	DBG_ASSERT( !bt_mfile().is_null() );
	// init the bitfield_tocheck
	partavail_piece_bitfield= bitfield_t(bt_mfile().nb_piece());
	// loop until the current_inval is empty
	while( !current_inval.empty() ){
		// compute the piece_idx for the first item available
		file_size_t	first_offset	= current_inval[0].min_value();
		size_t	    	piece_idx	= bt_unit_t::totfile_to_pieceidx(first_offset, bt_mfile());
		// compute the totfile_range for this piece_idx
		file_range_t	piece_range	= bt_unit_t::pieceidx_to_totfile_range(piece_idx, bt_mfile());
		// compute the intersection between the piece_range and the current_inval
		piece_isavail_inval = file_size_inval_t(piece_range.to_inval_item()) - current_inval;
		// if this piece is not in the partavail_piece_inval, goto the next
		if( piece_isavail_inval.empty() )		continue;
		// remove piece_isavail_inval from the current_inval
		current_inval	-= piece_isavail_inval;		
		// as this piece_idx is partially available, set the piece_idx to true
		partavail_piece_bitfield.set(piece_idx, true);
	}
	// sanity check - intersection MUST NOT exists betweel pieceavail_local and partavail_piece_bitfield
	DBG_ASSERT( (pieceavail_local() & partavail_piece_bitfield).is_none_set() );
	// return the just built bitfield_t
	return partavail_piece_bitfield;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	bt_swarm_resumedata_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string to return
	oss        << "bt_mfile="		<< bt_mfile();
	oss << " " << "pieceavail_local="	<< pieceavail_local();
	oss << " " << "pieceprec_arr="		<< pieceprec_arr();
	oss << " " << "peersrc_peer_arr="	<< peersrc_peer_arr();
	oss << " " << "partavail_piece_inval="	<< partavail_piece_inval();
	oss << " " << "swarm_stats="		<< swarm_stats();
	oss << " " << "mfile_allocated="	<< mfile_allocated();
	
	// return the just built string
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          serialization
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief serialize a bt_swarm_resumedata_t
 * 
 * - support null bt_swarm_resumedata_t
 */
serial_t& operator << (serial_t& serial, const bt_swarm_resumedata_t &swarm_resumedata)		throw()
{
	// serialize each field of the object
	serial << swarm_resumedata.bt_mfile();
	serial << uint32_t(swarm_resumedata.pieceavail_local().nb_piece());
	serial << swarm_resumedata.pieceavail_local();
	serial << swarm_resumedata.pieceprec_arr();
	serial << swarm_resumedata.peersrc_peer_arr();
	serial << swarm_resumedata.partavail_piece_inval();
	serial << swarm_resumedata.swarm_stats();
	serial << swarm_resumedata.mfile_allocated();
	// return serial
	return serial;
}

/** \brief unserialze a bt_swarm_resumedata_t
 * 
 * - support null bt_swarm_resumedata_t
 */
serial_t& operator >> (serial_t & serial, bt_swarm_resumedata_t &swarm_resumedata)		throw(serial_except_t)
{
	bt_mfile_t		bt_mfile;
	size_t			nb_piece;
	bt_pieceavail_t		pieceavail_local;
	bt_pieceprec_arr_t	pieceprec_arr;
	bt_peersrc_peer_arr_t	peersrc_peer_arr;
	file_size_inval_t	partavail_piece_inval;
	bt_swarm_stats_t	swarm_stats;
	bool			mfile_allocated;
	// reset the destination variable
	swarm_resumedata	= bt_swarm_resumedata_t();

	// unserialize the data
	serial >> bt_mfile;
	serial >> nb_piece;
	pieceavail_local	= bt_pieceavail_t(nb_piece);
	serial >> pieceavail_local;
	serial >> pieceprec_arr;
	serial >> peersrc_peer_arr;
	serial >> partavail_piece_inval;
	serial >> swarm_stats;
	serial >> mfile_allocated;

	// set the returned variable
	swarm_resumedata	.bt_mfile(bt_mfile)
				.pieceavail_local(pieceavail_local)
				.pieceprec_arr(pieceprec_arr)
				.peersrc_peer_arr(peersrc_peer_arr)
				.partavail_piece_inval(partavail_piece_inval)
				.swarm_stats(swarm_stats)
				.mfile_allocated(mfile_allocated);
	// return serial
	return serial;
}
NEOIP_NAMESPACE_END

