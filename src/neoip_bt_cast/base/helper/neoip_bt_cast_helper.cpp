/*! \file
    \brief Definition of the \ref bt_cast_helper_t class
    
*/

/* system include */
/* local include */
#include "neoip_bt_cast_helper.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_bt_cast_pidx.hpp"
#include "neoip_bt_cast_id.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			TODO to command
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a bt_mfile_t for bt_swarm_t on a libneoip_cast swarm
 * 
 * - TODO currently just a stub, as i dont know the real requirement for it
 *   - to make it more tunnable is one of those requirement
 *     - piecelen
 *     - the subfile name and main name
 *     - the subfiel size... in relation with the piece_idx warp around 
 * - TODO document the assumption here
 *   - e.g. no local path as it is bt_io_pfile_t
 * 
 */
bt_mfile_t	bt_cast_helper_t::build_mfile(const bt_cast_id_t &cast_id
					, const std::string &cast_name
					, const http_uri_t &http_peersrc_uri)	throw()
{
	file_size_t		totfile_size	= 10*1024*1024;
	bt_mfile_t		bt_mfile;
#if 1	// make the totfile_size shorter to warparound more frequently
	// - thus test more the warparound
	totfile_size	= 4*1024*1024;
#endif
	// create the bt_mfile
	if( !http_peersrc_uri.is_null() )	bt_mfile.announce_uri( http_peersrc_uri );
	bt_mfile.name		( cast_name );
	bt_mfile.infohash	( cast_id.to_canonical_string() );	

	// NOTE: bt_mfile.piecelen() is directly related to the latency 
	//       between reality and neoip-casto
	// - this is due to the fact that a piece is declared available only when
	//   fully downloaded. 
#if 0
	bt_mfile.piecelen	( 32*1024 );
#else
	bt_mfile.piecelen	( 16*1024 );
#endif
	bt_mfile.do_piecehash	( false );
	bt_mfile.subfile_arr()	+= bt_mfile_subfile_t("streamdata", totfile_size);
	bt_mfile.complete_init();
	// return the just built bt_mfile
	return bt_mfile;
}

/** \brief Build a bt_mfile_t for bt_swarm_t on a bt_cast_mdata_t
 * 
 * - just an helper on the more 'raw' version
 */
bt_mfile_t	bt_cast_helper_t::build_mfile(const bt_cast_mdata_t &cast_mdata)	throw()
{
	return build_mfile(cast_mdata.cast_id(), cast_mdata.cast_name(), cast_mdata.http_peersrc_uri());
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			remove_piece_outside_pieceq
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Declare all the piece outside pieceq as nomore_avail
 */
void	bt_cast_helper_t::remove_piece_outside_pieceq(bt_swarm_t *bt_swarm
					, size_t pieceq_beg, size_t pieceq_end)	throw()
{
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	bt_cast_pidx_t		cast_pidx	= bt_cast_pidx_t().modulo(bt_mfile.nb_piece());
	const bt_pieceavail_t &	local_pavail	= bt_swarm->local_pavail();

	// go thru each pieceidx
	for(size_t pieceidx = 0; pieceidx < bt_mfile.nb_piece(); pieceidx++ ){
		// if this pieceidx is in the pieceq, goto the next
		if( cast_pidx.index(pieceidx).is_in(pieceq_beg,pieceq_end) )	continue;
		// if this pieceidx is unavail, goto the next
		if( local_pavail.is_unavail(pieceidx) )				continue;
		// declare this pieceidx as nomore avail
		bt_swarm->declare_piece_nomore_avail(pieceidx);
	}
}

NEOIP_NAMESPACE_END


