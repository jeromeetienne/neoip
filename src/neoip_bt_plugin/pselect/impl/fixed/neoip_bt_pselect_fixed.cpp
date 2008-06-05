/*! \file
    \brief Definition of the \ref bt_pselect_fixed_t

\par Brief Description
bt_pselect_fixed_t is a bt_pselect_vapi_t implementation optimized for the
case of fixed bt_pieceprec_t
- this is typically used to deliver the data from bt to files
- bt_pieceprec_arr_t is not supposed to change frequently
  - changing it is supported but cause the full recomputation over all pieces
    so may cost significant cpu depending on the number of piece.
- it support the piece frequency. so for a given bt_pieceprec_t, the rarest piece 
  will be selected
- it support randomness among pieceidx which have the same bt_pieceprec and goub.


\par Note about core structure
- m_remote_pfreq keeps the frequency of each pieceidx within the remote peers
  - it is updated via the inc/dec_remote_pfreq method
- m_remote_pwish keeps the wish of each pieceidx within the remote peers
  - it is updated via the inc/dec_remote_pfreq method
- m_local_pdling keeps the state of the local pieces currently downloading
- m_local_pavail keeps the state of the local pieces currently available
  - NOTE: a piece MUST NOT be in m_local_pdling and m_local_pavail at the 
    same time.
- m_pieceprec_arr is the precedence for each pieceidx
  - it may be set at any moment, and is used by the bt_swarm_t caller to 
    set custom piece selection. 
  - if the bt_swarm_t caller whish not to download some piece, it set the 
    pieceidx to bt_pieceprec_t::NOTNEEDED

\par Note about precomputation to speed up piece selection
- selectable_db is a map of precedence to a goub_db
- goub_db is a map of wish to a piece_db
- piece_db is a set of pieceidx
- thus to select a piece, require to walk the selectable_db in 'order' thru those
  3 structures because it is put the highest precedence first, and among the 
  pieceidx with the same precedence, put the most goub first, then one can choose
  any piece among the piece_db
  - in order to ensure a good spreading of the piece among the swarm, the pieceidx
    is chosen randomly within the piece_db
*/

/* system include */
/* local include */
#include "neoip_bt_pselect_fixed.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Build a bt_pselect_fixed_t by a nb_piece
 * 
 * - NOTE: the precedence_db_t is equal for each piece
 */
bt_pselect_fixed_t::bt_pselect_fixed_t(const size_t &nb_piece)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// init some field
	m_remote_pfreq	= bitcount_t(nb_piece);
	m_remote_pwish	= bitcount_t(nb_piece);
	m_local_pdling	= bitfield_t(nb_piece);
	m_local_pavail	= bt_pieceavail_t(nb_piece);
	// set the default precedence to all pieces - it may be changed later via pieceprec_arr(arr)
	pieceprec_arr(bt_pieceprec_arr_t().assign(nb_piece, bt_pieceprec_t::DEFAULT));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set a new bt_pieceprec_arr_t
 */
void	bt_pselect_fixed_t::pieceprec_arr(const bt_pieceprec_arr_t &m_pieceprec_arr)	throw()
{
	// sanity check - the pieceprec_arr MUST have the same size as pignored
	DBG_ASSERT( m_pieceprec_arr.size() == nb_piece() );
	// copy the new pieceprec_arr
	this->m_pieceprec_arr	= m_pieceprec_arr;

	// reset selectable_db
	selectable_db	= selectable_db_t();
	// rebuild the selectable_db with the new precedence_db
	for(size_t pieceidx = 0; pieceidx < nb_piece(); pieceidx++){
		size_t	piece_prec	= m_pieceprec_arr[pieceidx].value();
		size_t	piece_goub	= remote_pgoub(pieceidx);
		// if this pieceidx cant be selected, skip it
		if( cant_select(pieceidx) )			continue;
		// if this piece_prec is NOTNEEDED, skip it
		if( piece_prec == bt_pieceprec_t::NOTNEEDED )	continue;
		// insert this pieceidx in the selectable_db at its own precedence and wish
		selectable_db[piece_prec][piece_goub].insert(pieceidx);
	}	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      piece selection
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the next pieceidx to select
 * 
 * @return std::numeric_limits<size_t>::max() if none pieceidx is present in both
 */
size_t	bt_pselect_fixed_t::next_piece_to_select(const bt_pieceavail_t &remote_pieceavail
						, size_t pieceidx_hint)	const throw()
{
	// if whished_pieceidx is null, return directly the result of next_piece_to_select_nohint 
	if( pieceidx_hint == std::numeric_limits<size_t>::max() )
		return next_piece_to_select_nohint(remote_pieceavail);

	// sanity check - the pieceidx_hint MUST be a valid pieceidx
	DBG_ASSERT( pieceidx_hint < nb_piece() );

	// get the 'normal_pieceidx' - aka the piece which would be selected in absence of pieceidx_hint
	size_t	normal_pieceidx = next_piece_to_select_nohint(remote_pieceavail);
	// if there are no normal_pieceidx, return a null pieceidx now
	if( normal_pieceidx == std::numeric_limits<size_t>::max())
		return normal_pieceidx;
		
	// NOTE: from here check if the pieceidx_hint is an acceptable alternative to normal_pieceidx
		
	// if pieceidx_hint cant be selected, select normal_pieceidx
	if( cant_select(pieceidx_hint) )				return normal_pieceidx;
	// if pieceidx_hint is not remotly available, select normal_pieceidx
	if( !remote_pieceavail.is_avail(pieceidx_hint) )		return normal_pieceidx;
	// if the bt_pieceprec_t of the pieceidx_hint is less than the normal_pieceidx, select the normal_pieceidx
	if( pieceprec(pieceidx_hint) < pieceprec(normal_pieceidx) )	return normal_pieceidx;
#if 0
	// if pieceidx_hint is more remotely frequent than normal_pieceidx, select normal_pieceidx
	if(remote_pfreq(pieceidx_hint)>remote_pfreq(normal_pieceidx))	return normal_pieceidx;
#else
	// if pieceidx_hint is less wished than normal_pieceidx, select normal_pieceidx
	if(remote_pwish(pieceidx_hint)<remote_pwish(normal_pieceidx))	return normal_pieceidx;
#endif
	// else select the pieceidx_hint
	return pieceidx_hint;
}


/** \brief Return the next pieceidx to select with no pieceidx_hint
 * 
 * - return std::numeric_limits<size_t>::max() if none pieceidx is present in both
 * - among all the ones which have the same remote_pfreq, one is picked at random
 *   - this is actually important to ensure a good distribution of the data withing the swarm
 *   - for example when several nonedata peer connect a single fulldata peer, if all the nonedata
 *     download the pieces in sequence, they will have little to exchange between themselves.
 *     - but if they download random piece, once they each other got a single piece, they can start
 *       exchange.
 * - moreover it is similar to the "random first piece" policy of the bt paper but it is 
 *   more generic as it is applied during the whole download and not only for the first piece.
 * @param remote_pieceavail	a bt_pieceavail_t of the piece remotly available
 * @param pieceidx_hint	the pieceidx to prefere if possible (if == max size_t, means to ignore)
 */
size_t	bt_pselect_fixed_t::next_piece_to_select_nohint(const bt_pieceavail_t &remote_pieceavail)const throw()
{
	selectable_db_t::const_reverse_iterator	selectable_iter;
	// sanity check - remote_pieceavail MUST be of the same size 
	DBG_ASSERT( remote_pieceavail.nb_piece() == nb_piece() );	
	// go the thru selectable_db from the highest precendence to the lowest
	for(selectable_iter = selectable_db.rbegin(); selectable_iter != selectable_db.rend(); selectable_iter++){
		const goub_db_t &		goub_db		= selectable_iter->second;
#if 0
		// go the thru goub_db from the less frequent to the most
		goub_db_t::const_iterator	goub_iter;
		for(goub_iter = goub_db.begin(); goub_iter != goub_db.end(); goub_iter++){
#else
		// go the thru goub_db from the less frequent to the most
		goub_db_t::const_reverse_iterator	goub_iter;
		for(goub_iter = goub_db.rbegin(); goub_iter != goub_db.rend(); goub_iter++){
#endif
			const piece_db_t & 		piece_db	= goub_iter->second;
			piece_db_t::const_iterator	iter_origin	= piece_db.begin();
			piece_db_t::const_iterator	piece_iter;
			// start at a random place withing the piece_db
			std::advance(iter_origin, rand() % piece_db.size());
			// loop over the whole piece_db
			piece_iter	= iter_origin;
			do {
				size_t	pieceidx	= *piece_iter;
				// if this piece is present in the pieceavail, return it
				if( remote_pieceavail[pieceidx] )	return pieceidx;
				// go to the next item
				piece_iter++;
				// wrap around if the end of piece_db is reached
				if( piece_iter == piece_db.end() )	piece_iter = piece_db.begin();
			} while( piece_iter != iter_origin );
		}
	}
	// if no piece match, return size_t max
	return std::numeric_limits<size_t>::max();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                selectable_db maintenance
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief insert this pieceidx from the selectable_db
 */
void	bt_pselect_fixed_t::selectable_db_insert(size_t pieceidx)	throw()
{
	// sanity check - the pieceidx MUST less than nb_piece()
	DBG_ASSERT( pieceidx < nb_piece() );
	// sanity check - the pieceidx MUST be selectable
	DBG_ASSERT( may_select(pieceidx) );

	// if this piece_prec is NOTNEEDED, do nothing
	if( m_pieceprec_arr[pieceidx].is_notneeded() )	return;
	// get the precedence for this pieceidx
	size_t	piece_prec	= m_pieceprec_arr[pieceidx].value();
	// get the wish for this pieceidx
	size_t	piece_goub	= remote_pgoub(pieceidx);

	// get the piece_db_t for all the pieces of the same precedence and wish
	// - the std::vector create an empty element if it did not exist before
	piece_db_t & piece_db	= selectable_db[piece_prec][piece_goub];
	// insert the pieceidx in the piece_db
	bool	succeed		= piece_db.insert(pieceidx).second;
	DBG_ASSERT( succeed );
}

/** \brief delete this pieceidx from the goub_db
 */
void	bt_pselect_fixed_t::selectable_db_delete(size_t pieceidx)	throw()
{
	// sanity check - the pieceidx MUST less than nb_piece
	DBG_ASSERT( pieceidx < nb_piece() );
	// sanity check - the pieceidx MUST be selectable
	DBG_ASSERT( may_select(pieceidx) );

	// if this piece_prec is NOTNEEDED, do nothing
	if( m_pieceprec_arr[pieceidx].is_notneeded() )	return;

	// get the precedence for this pieceidx
	size_t	piece_prec	= m_pieceprec_arr[pieceidx].value();
	// get the selectable_db element for this piece_prec
	selectable_db_t::iterator	selectable_iter	= selectable_db.find(piece_prec);	
	// sanity check - this precedence MUST be present in selectable_db
	DBG_ASSERT( selectable_iter != selectable_db.end() );
	
	// get the wish for this pieceidx
	size_t	piece_goub	= remote_pgoub(pieceidx);
	// get the goub_db element for this piece_goub
	goub_db_t &		goub_db		= selectable_iter->second;
	goub_db_t::iterator	goub_iter	= goub_db.find(piece_goub);
	// sanity check - this frequency MUST be present in goub_db
	DBG_ASSERT( goub_iter != goub_db.end() );

	// get the piece_db_t for all the pieces of the same frequency
	piece_db_t & piece_db	= goub_iter->second;
	// sanity check - the pieceidx MUST be present in the piece_db
	DBG_ASSERT( piece_db.find(pieceidx) != piece_db.end() );

	// remove this pieceidx from the piece_db
	piece_db.erase(pieceidx);

	// if the piece_db is now empty, remove this frequency from the goub_db
	if( piece_db.empty() )	goub_db.erase( goub_iter );
	// if the goub_db is now empty, remove this precedence from the selectable_db
	if( goub_db.empty() )	selectable_db.erase( selectable_iter );
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//		update bitfield_t/bitcount_t which affect selectable_db
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief update the selectable status of this pieceidx to value and update
 *         the bitfield
 */
void	bt_pselect_fixed_t::upd_selectable(bitfield_t &bitfield, size_t pieceidx
							, bool value)	throw()
{
	// delete it from the selectable_db_t if needed
	if( may_select(pieceidx) )	selectable_db_delete( pieceidx );
	// update the bitfield_t
	bitfield.set(pieceidx, value);
	// insert it in the selectable_db_t if needed
	if( may_select(pieceidx) )	selectable_db_insert( pieceidx );

	// sanity check - m_local_pavail and m_local_pdling MUST NOT be one at the same time
	DBG_ASSERT( !(m_local_pavail[pieceidx] && m_local_pdling[pieceidx]) );
}

/** \brief update the selectable status for each piece set in the extfield
 *         and the intfield to the value
 * 
 * - it wont anymore be taken in account piece selection
 */
void	bt_pselect_fixed_t::upd_selectable(bitfield_t &intfield
					, const bitfield_t &extfield, bool value)	throw()
{
	// sanity check - external bitfield MUST be have the proper number of piece 
	DBG_ASSERT( extfield.size() == nb_piece() );
	// go thru the whole extfield bitfield_t
	for(size_t pieceidx = 0; pieceidx < nb_piece(); pieceidx++ ){
		// if this pieceidx is not set, skip it
		if( extfield[pieceidx] == false )	continue;
		// if this pieceidx is set, call upd_selectable for this pieceidx
		upd_selectable(intfield, pieceidx, value);
	}
}

/** \brief update the selectable status of this pieceidx to value and update
 *         the bitcount
 */
void	bt_pselect_fixed_t::upd_selectable(bitcount_t &bitcount, size_t pieceidx
							, bool value)	throw()
{
	// delete it from the selectable_db_t if needed
	if( may_select(pieceidx) )	selectable_db_delete( pieceidx );
	// update the bitcount
	if( value )	bitcount.inc(pieceidx);
	else		bitcount.dec(pieceidx);
	// insert it in the selectable_db_t if needed
	if( may_select(pieceidx) )	selectable_db_insert( pieceidx );
}

/** \brief update the selectable status for each piece set in the extcount
 *         and the intcount to the value
 * 
 * - it wont anymore be taken in account piece selection
 */
void	bt_pselect_fixed_t::upd_selectable(bitcount_t &intcount
					, const bitfield_t &extfield, bool value)	throw()
{
	// sanity check - external bitfield_t MUST be have the proper number of piece 
	DBG_ASSERT( extfield.size() == nb_piece() );
	// go thru the whole extfield bitfield_t
	for(size_t pieceidx = 0; pieceidx < nb_piece(); pieceidx++ ){
		// if this pieceidx is not set, skip it
		if( extfield[pieceidx] == false )	continue;
		// if this pieceidx is set, call upd_selectable for this pieceidx
		upd_selectable(intcount, pieceidx, value);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   to_string() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bt_pselect_fixed_t::to_string()			const throw()
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
	oss << "local_pavail="	<< local_pavail();
	oss << " ";
	
	// go the thru all the piece in bt_pselect_fixed_t
	selectable_db_t::const_reverse_iterator	selectable_iter;
	for(selectable_iter = selectable_db.rbegin(); selectable_iter != selectable_db.rend(); selectable_iter++){
		size_t				piece_prec	= selectable_iter->first;
		const goub_db_t &		goub_db		= selectable_iter->second;
		goub_db_t::const_iterator	goub_iter;
		oss << "[precedence=" << piece_prec;
		// go the thru all the piece in bt_pselect_fixed_t
		for(goub_iter = goub_db.begin(); goub_iter != goub_db.end(); goub_iter++){
			size_t				piece_goub	= goub_iter->first;
			const piece_db_t & 		piece_db	= goub_iter->second;
			piece_db_t::const_iterator	piece_iter;
			oss << "[goub=" << piece_goub;
			for(piece_iter = piece_db.begin(); piece_iter != piece_db.end(); piece_iter++)
				oss << "[" << *piece_iter << "]";
			oss << "]";
		}
		oss << "]";
	}
	oss << "]";	
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






