/*! \file
    \brief Definition of the \ref bt_pselect_slide_t

\par Brief Description
bt_pselect_slide_t is a bt_pselect_vapi_t implementation optimized for the
case of severals slide moves on the data with a their bt_pieceprec_t.
- this is typically used to deliver the data from bt to a http connection
  - as the data are written in the http connection, the bt_pselect_slide_curs_t
    progress, aka change its own offset
- doing such rapid change on a bt_pselect_static_t will create a lot of 
  computation overhead each time the slide position change.
- it support the remote_pwish. so for a given bt_pieceprec_t, the most 
  remotly wished piece will be selected.
- it support randomness among pieceidx which have the same bt_pieceprec 
  and remote_pwish.

\par Implementation Notes
- in this code, curs_idx is the index within bt_pselect_slide_curs_t::pieceprec_arr
  while piece_idx is the index within the bt_mfile_t.
  - so curs_idx may be seen as relative to bt_pselect_slide_curs_t
  - while piece_idx may be seen as absolute
- bt_pselect_slide_curs_t may or may not have has_circularidx
  - if they do the bt_pieceprec_t will warparound when reaching the last pieces
  - this is usefull when delivering an endless streaming to http as in neoip-casti

*/

/* system include */
/* local include */
#include "neoip_bt_pselect_slide.hpp"
#include "neoip_bt_pselect_slide_curs.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a bt_pselect_slide_t by a nb_piece
 * 
 * - NOTE: the precedence_db_t is equal for each piece
 */
bt_pselect_slide_t::bt_pselect_slide_t(const size_t &nb_piece)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// init some field
	m_remote_pfreq	= bitcount_t(nb_piece);
	m_remote_pwish	= bitcount_t(nb_piece);
	m_local_pdling	= bitfield_t(nb_piece);
	m_local_pavail	= bt_pieceavail_t(nb_piece);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief DoLink a bt_pselect_slide_curs_t 
 */
void	bt_pselect_slide_t::curs_dolink(bt_pselect_slide_curs_t *slide_curs)	throw()
{
	// sanity check - the pieceprec_arr.size() MUST NOT be > than nb_piece()
	DBG_ASSERT( slide_curs->pieceprec_arr().size() <= nb_piece() );
	// insert this bt_pselect_slide_curs_t into the curs_db
	curs_db.push_back(slide_curs);
	// regenerate the prec_db
	generate_prec_db();
}

/** \brief UnLink a bt_pselect_slide_curs_t 
 */
void	bt_pselect_slide_t::curs_unlink(bt_pselect_slide_curs_t *slide_curs)	throw()
{
	// sanity check - the pieceprec_arr.size() MUST NOT be > than nb_piece()
	DBG_ASSERT( slide_curs->pieceprec_arr().size() <= nb_piece() );
	// remove this bt_pselect_slide_curs_t from the curs_db
	curs_db.remove(slide_curs);
	// regenerate the prec_db
	generate_prec_db();
}
	
/** \brief Generate the prec_db_t 
 */
void	bt_pselect_slide_t::generate_prec_db()		throw()
{
	std::list<bt_pselect_slide_curs_t *>::iterator	iter;
	// reset the prec_db
	prec_db	= prec_db_t();
	// go thru the whole curs_db
	for(iter = curs_db.begin(); iter != curs_db.end(); iter++){
		bt_pselect_slide_curs_t *	slide_curs	= *iter;
		const bt_pieceprec_arr_t &	pieceprec_arr	= slide_curs->pieceprec_arr();
		// go thru the whole pieceprec_arr
		for(size_t curs_idx = 0; curs_idx < pieceprec_arr.size(); curs_idx++){
			const bt_pieceprec_t &	pieceprec	= pieceprec_arr[curs_idx];
			// if this bt_pieceprec_t is notneeded, skip it
			if( pieceprec.is_notneeded() )	continue;
			// insert this (slide_curs, piece_idx) in the prec_db at its own precedence
			prec_db[pieceprec.value()].push_back(std::make_pair(slide_curs, curs_idx));
		}
	}
}


/** \brief Functor for sorting piece_db_t by their remote_pwish
 * 
 * - used in next_piece_to_select_nohint()
 */
struct cmp_piece_db_t : public std::binary_function<bt_pselect_slide_t::piece_curs_t
				, bt_pselect_slide_t::piece_curs_t, bool> {
	private:const bt_pselect_slide_t *	pselect_slide;
	public:	cmp_piece_db_t(const bt_pselect_slide_t *pselect_slide)
					: pselect_slide(pselect_slide) {}
	
	bool operator()(const bt_pselect_slide_t::piece_curs_t &item1
				, const bt_pselect_slide_t::piece_curs_t &item2){
		bt_pselect_slide_curs_t*slide_curs1	= item1.first;
		bt_pselect_slide_curs_t*slide_curs2	= item2.first;
		size_t	cursidx1	= item1.second;
		size_t	cursidx2	= item2.second;
		size_t	pieceidx1	= slide_curs1->cursidx_to_pieceidx(cursidx1);
		size_t	pieceidx2	= slide_curs1->cursidx_to_pieceidx(cursidx2);
// TODO to adapt to remote_pwish
#if 0	// this version use remote_pfreq to sort the 2 pieces
		size_t	frequency1	= std::numeric_limits<size_t>::max();
		size_t	frequency2	= std::numeric_limits<size_t>::max();
		// compute the frequency1 - if pieceidx is outofrange, make it the maximum 
		if( pieceidx1 != std::numeric_limits<size_t>::max() )
			frequency1	= slide_curs1->pselect_slide()->remote_pfreq(pieceidx1);
		// compute the frequency2 - if pieceidx is outofrange, make it the maximum 
		if( pieceidx2 != std::numeric_limits<size_t>::max() )
			frequency2	= slide_curs2->pselect_slide()->remote_pfreq(pieceidx2);
		// return the less-than bool - to get piece_db_t in ascending order of frequency
		return frequency1 < frequency2;
#else	// this version use remote_pwish to sort the 2 pieces
		size_t	wish1	= std::numeric_limits<size_t>::min();
		size_t	wish2	= std::numeric_limits<size_t>::min();
		// compute the wish1 - if pieceidx is outofrange, make it the min 
		if( pieceidx1 != std::numeric_limits<size_t>::max() )
			wish1	= slide_curs1->pselect_slide()->remote_pwish(pieceidx1);
		// compute the wish2 - if pieceidx is outofrange, make it the min 
		if( pieceidx2 != std::numeric_limits<size_t>::max() )
			wish2	= slide_curs2->pselect_slide()->remote_pwish(pieceidx2);
		// return the less-than bool - to get piece_db_t in descending order of wish
		return wish1 > wish2;
#endif
	}
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      piece selection
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the next piece_idx to select
 * 
 * @return std::numeric_limits<size_t>::max() if none piece_idx is present in both
 */
size_t	bt_pselect_slide_t::next_piece_to_select(const bt_pieceavail_t &remote_pieceavail
						, size_t pieceidx_hint)	const throw()
{
	// if whished_pieceidx is null, return directly the result of next_piece_to_select_nohint 
	if( pieceidx_hint == std::numeric_limits<size_t>::max() )
		return next_piece_to_select_nohint(remote_pieceavail);

	// sanity check - the pieceidx_hint MUST be a valid piece_idx
	DBG_ASSERT( pieceidx_hint < nb_piece() );

	// get the 'normal_pieceidx' - aka the piece which would be selected in absence of pieceidx_hint
	size_t	normal_pieceidx = next_piece_to_select_nohint(remote_pieceavail);
	// if there are no normal_pieceidx, return a null pieceidx now
	if( normal_pieceidx == std::numeric_limits<size_t>::max())
		return normal_pieceidx;
	
	// sanity check - the normal pieceidx MUST be < nb_piece()
	DBG_ASSERT( normal_pieceidx < nb_piece() );
		
	// NOTE: from here check if the pieceidx_hint is an acceptable alternative to normal_pieceidx
		
	// if pieceidx_hint cant be selected or not remotly available, select normal_pieceidx
	if( cant_select(pieceidx_hint) )				return normal_pieceidx;
	if( remote_pieceavail.is_unavail(pieceidx_hint) )		return normal_pieceidx;
	// if the bt_pieceprec_t of the pieceidx_hint is less than the normal_pieceidx, select the normal_pieceidx
	if( pieceprec(pieceidx_hint) < pieceprec(normal_pieceidx) )	return normal_pieceidx;
#if 0
	// if the remote_pfreq of the pieceidx_hint is > than normal_pieceidx, select normal_pieceidx
	if(remote_pfreq(pieceidx_hint) > remote_pfreq(normal_pieceidx))	return normal_pieceidx;
#else
	// if pieceidx_hint is more wished than normal_pieceidx, select the normal_pieceidx
	if(remote_pwish(pieceidx_hint) < remote_pwish(normal_pieceidx))	return normal_pieceidx;
#endif	
	// else select the pieceidx_hint
	return pieceidx_hint;
}

/** \brief Return the next piece_idx to select without any pieceidx_hint
 * 
 * @return std::numeric_limits<size_t>::max() if none piece_idx is present in both
 */
size_t	bt_pselect_slide_t::next_piece_to_select_nohint(const bt_pieceavail_t &remote_pieceavail)	const throw()
{
	prec_db_t::reverse_iterator	prec_iter;
	// sanity check - pieceavail MUST be of the same size
	DBG_ASSERT( remote_pieceavail.nb_piece() == nb_piece() );

	// go the thru prec_db from the highest precedence to the lowest
	for(prec_iter = prec_db.rbegin(); prec_iter != prec_db.rend(); prec_iter++){
		piece_db_t &	piece_db	= prec_iter->second;
		// shuffle the piece_db to insert some randomness in case of prec equality
		// - this is an important for bt efficiency - if several pieceidx have
		//   equal precedence/wishfactor, it MUST be picked randomly 
		// - this is done before the sorting, in the hope to produce different
		//   order in case of equal bt_pieceprec_t
		std::random_shuffle(piece_db.begin(), piece_db.end());
		// sort the piece_db_t by the frequency of the pieces it contains
		// - NOTE: the sorting is done inplace in the piecedb
		// - TODO be carefull this is not tested - i may take the wrong order :)
		std::sort(piece_db.begin(), piece_db.end(), cmp_piece_db_t(this));
		// loop over the whole piece_db
		piece_db_t::const_iterator	piece_iter;
		for(piece_iter = piece_db.begin(); piece_iter != piece_db.end(); piece_iter++){
			// get info from the piece_iter
			const piece_curs_t &	piece_curs	= *piece_iter;
			bt_pselect_slide_curs_t*slide_curs	= piece_curs.first;
			size_t			curs_idx	= piece_curs.second;
			size_t			piece_idx	= slide_curs->cursidx_to_pieceidx(curs_idx);
			// if curs_idx is out-of-range for this piece_curs, dont select it
			if( piece_idx == std::numeric_limits<size_t>::max() )	continue;
			// if this piece cant be selected, dont select it 
			if( cant_select(piece_idx) )				continue;
			// if this piece_idx ISNOT remotly available, dont select it
			if( !remote_pieceavail.is_avail(piece_idx) )		continue;
			// if all previous tests passed, this piece_idx is selected
			return piece_idx;
		}
	}
	// if no piece match, return size_t max
	return std::numeric_limits<size_t>::max();
}

/** \brief Return true IFF it is still possible to select piece with bt_pselect_slide_t
 * 
 * - in fact it use the normal piece selection function and simulate a remote_pieceavail
 *   with all pieces available
 */
bool	bt_pselect_slide_t::has_selectable()		const throw()
{
	// build a 'fake' bt_pieceavail_t with all pieces available
	bt_pieceavail_t	seed_pieceavail;
	seed_pieceavail.assign(nb_piece(), true);
	// test if this seed_pieceavail would result in a piece selection
	size_t		pieceidx	= next_piece_to_select_nohint(seed_pieceavail);
	// if no piece has been selected, return false
	if( pieceidx == std::numeric_limits<size_t>::max() )	return false;
	// if a piece has been selected, return true
	return true;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query for pieceprec
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a snapshop of the current bt_pieceprec_arr_t
 * 
 * - it will be obsoleted by any move on any bt_pselect_slide_curs_t
 */
bt_pieceprec_arr_t	bt_pselect_slide_t::pieceprec_arr()		const throw()
{
	bt_pieceprec_arr_t	pieceprec_arr;
	// init the bt_pieceprec_arr_t
	pieceprec_arr.assign(size(), bt_pieceprec_t::NOTNEEDED);
	// go thru the whole curs_db
	std::list<bt_pselect_slide_curs_t *>::const_iterator	iter;
	for(iter = curs_db.begin(); iter != curs_db.end(); iter++){
		bt_pselect_slide_curs_t *	slide_curs		= *iter;
		const bt_pieceprec_arr_t &	curs_pieceprec_arr	= slide_curs->pieceprec_arr();
		// go thru the whole curs_pieceprec_arr
		for(size_t curs_idx = 0; curs_idx < curs_pieceprec_arr.size(); curs_idx++){
			const bt_pieceprec_t &	curs_pieceprec	= curs_pieceprec_arr[curs_idx];
			size_t			piece_idx	= slide_curs->cursidx_to_pieceidx(curs_idx);
			// if curs_idx is out-of-range for this piece_curs, goto next piece_curs 
			if( piece_idx == std::numeric_limits<size_t>::max() )	continue;
			// update max_pieceprec if curs_pieceprec is greater
			pieceprec_arr[piece_idx] = std::max(pieceprec_arr[piece_idx], curs_pieceprec);
		}
	}
	// return the just built bt_pieceprec_arr_t
	return pieceprec_arr;
}

/** \brief Return the bt_pieceprec_t for this piece_idx
 * 
 * - aka the highest bt_pieceprec_t of all the attached slide
 */
bt_pieceprec_t		bt_pselect_slide_t::pieceprec(size_t piece_idx)	const throw()
{
	// set the default bt_pieceprec (used if there is no attached bt_pselect_slide_curs_t
	bt_pieceprec_t	max_pieceprec	= bt_pieceprec_t::NOTNEEDED;
	// log to debug
	KLOG_DBG("piece_idx=" << piece_idx);
	// go thru the whole curs_db
	std::list<bt_pselect_slide_curs_t *>::const_iterator	iter;
	for(iter = curs_db.begin(); iter != curs_db.end(); iter++){
		bt_pselect_slide_curs_t*slide_curs	= *iter;
		bt_pieceprec_t		curs_pieceprec	= slide_curs->pieceprec(piece_idx);
		// update max_pieceprec if curs_pieceprec is greater
		max_pieceprec	= std::max(max_pieceprec, curs_pieceprec);
	}
	// log to debug
	KLOG_DBG("max_pieceprec=" << max_pieceprec);
	// return the result
	return max_pieceprec;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bt_pselect_slide_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )		return "null";
	// build the string
	oss << "[";
	oss << "remote_pfreq="	<< remote_pfreq();
	oss << " ";
	oss << "remote_pwish="	<< remote_pwish();
	oss << " ";
	oss << "local_pdling="	<< local_pdling();
	oss << " ";
	oss << "piecedavail="	<< local_pavail();
	oss << " ";
	std::list<bt_pselect_slide_curs_t *>::const_iterator	iter;
	oss << "curs_db=[";
	for(iter = curs_db.begin(); iter != curs_db.end(); iter++)	oss << *iter;
	oss << "]";	
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






