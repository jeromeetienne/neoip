/*! \file
    \brief Class to handle the bt_utmsg_piecewish_t

\par Brief Description
\rev bt_utmsg_piecewish_t is an extension which report the piece which are 
wished to be downloaded. 

\par Motivation
- At the very core of bt scheduling, there is a 'download the rarest piece first'
  principle. This is the main principle used to optimize the trading between peers
- by the 'rarest piece', in fact, it means the 'most wished' because the plain
  bittorrent protocol assumes that all peers wish to download all pieces.
- This assumption lacks flexibility even in the plain usage
  - e.g. a torrent contains 10 files and the users wishes to download only one. the 
    estimation of the rarest_piece will declare the piece of the 9 other files
    as 'rare' so as 'wished'. This is untrue.
  - In this example, this is not very important because most users downloads all 
    the files, so the error using the 'rarest_piece' as the 'most wished' is
    only slightly untrue.
- But it becomes completly untrue if most users wish to download only a small
  part of a torrent.
  - e.g. when delivering the torrent thru a http connection (e.g. neoip-oload or 
    neoip-casto) only a very small part of the torrent is wished to be downloaded
    at any given time (the part just after the current position of the http connection
    position within the torrent)
- So bt_utmsgtype_t::PIECEWISH uses a protocol to explicitly declare which
  piece are wished or not. instead of assuming all pieces are wished.

\par Relation with the piece selection
- in the piece selection, the principle 'select rarest piece first' is no more valid.
  It is directly replace by the 'select most wished piece first'
  - this is implemented somewhere in bt_swarm_t

\par Definition
A piece wished by a peer is a piece with a bt_pieceprec_t::is_needed
and which is not currently available.

\par Protocol description
It support 3 differents commands, all sent in bencoded format as usual for the 
bt_utmsg_vapi_t
- "df": this stands for DOWISH_FIELD
  - this is sent only once at the connection establishement
  - it contains a string of the binary format of the bitfield_t with 1 for the
    pieces wished, and 0 for the once unwished
  - similar to the plain bittorrent protocol with the command bt_cmdtype_t::PIECE_FIELD
- "di": this stands for DOWISH_INDEX
  - this is sent everytime pieces becomes wished and was unwished before
  - it contains a array of integer, with each integer being the piece_idx
  - similar to the plain bittorrent protocol with the command bt_cmdtype_t::PIECE_ISAVAIL
- "ni": this stands for NOWISH_INDEX
  - this is sent everytime pieces becomes unwished and was wished before
  - it contains a array of integer, with each integer being the piece_idx
  - similar to the bt_utmsg_punavail_t extension

\par Implementation Notes
- commands are sent to the remote peer to notify change in the local status of 'wish'
  - this is done via the following functions, which are forwarded by the bt_swarm_t
    - notify_pieceprec_change()
    - declare_piece_newly_avail()
    - declare_piece_nomore_avail()
- bt_utmsg_piecewish_t maintains a state of the currently wished pieces for the 
  local peer.
- bt_utmsg_piecewish_cnx_t maintains a states of the currently wished pieces
  for the remote peers.

\par Interaction with peers not supporting this extension
- the core principle is "if remote peer does not support this extension, it
  is assumed to wish all pieces which are not available"

*/

/* system include */
/* local include */
#include "neoip_bt_utmsg_piecewish.hpp"
#include "neoip_bt_utmsg_piecewish_cnx.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_utmsg_event.hpp"
#include "neoip_bt_cmd.hpp"
#include "neoip_bencode.hpp"
#include "neoip_dvar.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_utmsg_piecewish_t::bt_utmsg_piecewish_t()		throw()
{
	// zero some fields
	bt_swarm	= NULL;
	utmsg_cb	= NULL;
}

/** \brief Destructor
 */
bt_utmsg_piecewish_t::~bt_utmsg_piecewish_t()		throw()
{
	// unregister this object
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	if( utmsg_cb ){
		bool	tokeep	= notify_utmsg_cb( bt_utmsg_event_t::build_unregister() );
		DBG_ASSERT( tokeep == true );
	}
	// close all pending bt_utmsg_piecewish_cnx_t
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_utmsg_piecewish_t::start(bt_swarm_t *bt_swarm, bt_utmsg_cb_t *utmsg_cb
							, void *userptr)	throw()
{
	bool	tokeep;
	// copy the parameter
	this->bt_swarm	= bt_swarm;
	this->utmsg_cb	= utmsg_cb;
	this->userptr	= userptr;
	// init the local_pwish
	m_local_pwish	= generate_local_pwish();
	// register this utmsg to the callback
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return dontkeep
	tokeep	= notify_utmsg_cb( bt_utmsg_event_t::build_doregister() );
	DBG_ASSERT( tokeep == true );
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a bitfield_t of all the pieces locally wished
 */
bitfield_t	bt_utmsg_piecewish_t::generate_local_pwish()		throw()
{
	const bt_pieceavail_t &	local_pavail	= bt_swarm->local_pavail();
	bt_pselect_vapi_t *	pselect_vapi	= bt_swarm->pselect_vapi();
	bt_pieceprec_arr_t	pieceprec_arr	= pselect_vapi->pieceprec_arr();
	bitfield_t		piecewish(pieceprec_arr.size());
	// go thru the whole bt_pieceprec_arr_t
	for(size_t pieceidx = 0; pieceidx < pieceprec_arr.size(); pieceidx++){
		// if this pieceidx is not needed, goto the next
		if( pieceprec_arr[pieceidx].is_notneeded() )	continue;
		// if this pieceidx is already available, goto the next
		if( local_pavail[pieceidx] )			continue;
		// set this pieceidx as 'wished' in the piecewish bitfield_t
		piecewish.set(pieceidx);
	}
	// return the resulting bitfield_t
	return piecewish;
}

/** \brief Send a bt_cmd_t all the connections of this extension
 */
void	bt_utmsg_piecewish_t::send_cmd_to_all_cnx(const bt_cmd_t &bt_cmd)	throw()
{
	std::list<bt_utmsg_piecewish_cnx_t *>::iterator	iter;
	// go thru the whole cnx_db
	for(iter = cnx_db.begin(); iter != cnx_db.end(); iter++ ){
		bt_utmsg_piecewish_cnx_t *	piecewish_cnx	= *iter;
		// send the bt_cmd_t to this connection
		piecewish_cnx->send_cmd(bt_cmd);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_cmd_t building
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Generate the bt_cmd_t to declare a full piecewish bitfield_t
 */
bt_cmd_t bt_utmsg_piecewish_t::build_cmd_dowish_field(const bitfield_t &wishfield)	throw()
{
	// build the dvar - "df" for dowish_field
	dvar_t	dvar	= dvar_map_t();
	dvar.map().insert("df", dvar_str_t(wishfield.to_datum().to_stdstring()));
	// generate the payload of the bt_cmd_t
	pkt_t	payload;
	payload << bt_utmsgtype_t(bt_utmsgtype_t::PIECEWISH);
	payload.append(datum_t(bencode_t::from_dvar(dvar)));
	// return the just built bt_cmd_t
	return bt_cmd_t::build_utmsg_payl(payload.to_datum(datum_t::NOCOPY));
}

/** \brief Generate the bt_cmd_t containing dowish_pieceidx/nowish_pieceidx
 *         for each pieceidx different between the old_piecewish and new_piecewish
 */
bt_cmd_t	bt_utmsg_piecewish_t::build_cmd_delta_bitfield(const bitfield_t &old_piecewish
					, const bitfield_t &new_piecewish)	throw()
{
	// sanity check - both bitfield_t MUST have the same size
	DBG_ASSERT( old_piecewish.size() == new_piecewish.size() );
	// compute the difference from the previous and the current piecewish
	bitfield_t	diff_piecewish	= old_piecewish ^ new_piecewish;

	// go thru all the pieceidx which are different
	dvar_arr_t doidx_dvar, noidx_dvar;
	for(size_t idx = diff_piecewish.next_set(0); idx < diff_piecewish.size()
					; idx = diff_piecewish.next_set(idx+1) ){
		size_t	pieceidx	= idx;
		// log to debug
		KLOG_ERR((new_piecewish[pieceidx] ? "DOwish":"nowish") << " pieceidx " << pieceidx);
		// if this pieceidx is now wished, add this pieceidx into the array doidx
		// if this pieceidx is nomore wished, add it into the array noidx
		if( new_piecewish[pieceidx] )	doidx_dvar += dvar_int_t(pieceidx);
		else				noidx_dvar += dvar_int_t(pieceidx);
	}

	// build the dvar map
	dvar_t	dvar	= dvar_map_t();
	// add "di" doidx_dvar if not empty 
	if( !doidx_dvar.empty() )	dvar.map().insert("di", doidx_dvar);
	// add "ni" doidx_dvar if not empty 
	if( !noidx_dvar.empty() )	dvar.map().insert("ni", noidx_dvar);

	// generate the payload of the bt_cmd_t
	pkt_t	payload;
	payload << bt_utmsgtype_t(bt_utmsgtype_t::PIECEWISH);
	payload.append(datum_t(bencode_t::from_dvar(dvar)));
	// return the just built bt_cmd_t
	return bt_cmd_t::build_utmsg_payl(payload.to_datum(datum_t::NOCOPY));
}

/** \brief Generate a no/dowish_pieceidx bt_cmd_t for this pieceidx
 */
bt_cmd_t	bt_utmsg_piecewish_t::build_cmd_pieceidx(const std::string &cmd_str
						, size_t pieceidx)		throw()
{
	// build the dvar map
	dvar_t	dvar	= dvar_map_t();
	// add cmd_str element in the map as an array 
	dvar.map().insert(cmd_str, dvar_arr_t());
	// add the pieceidx in the array as a integer
	dvar.map()[cmd_str].arr() += dvar_int_t(pieceidx);

	// generate the payload of the bt_cmd_t
	pkt_t	payload;
	payload << bt_utmsgtype_t(bt_utmsgtype_t::PIECEWISH);
	payload.append(datum_t(bencode_t::from_dvar(dvar)));
	// return the just built bt_cmd_t
	return bt_cmd_t::build_utmsg_payl(payload.to_datum(datum_t::NOCOPY));
}

/** \brief Build a bt_cmd_t for a dowish_pieceidx
 */
bt_cmd_t	bt_utmsg_piecewish_t::build_cmd_dowish_pieceidx(size_t pieceidx)	throw()
{
	return build_cmd_pieceidx("di", pieceidx);
}

/** \brief Build a bt_cmd_t for a nowish_pieceidx
 */
bt_cmd_t	bt_utmsg_piecewish_t::build_cmd_nowish_pieceidx(size_t pieceidx)	throw()
{
	return build_cmd_pieceidx("ni", pieceidx);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			external notification
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function called by caller when the bt_pieceprec_arr_t change 
 */
void	bt_utmsg_piecewish_t::notify_pieceprec_change()	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// compute the new local_pwish
	bitfield_t	new_local_pwish	= generate_local_pwish();
	// generate the bt_cmd_t to xmit
	bt_cmd_t bt_cmd	= build_cmd_delta_bitfield(local_pwish(), new_local_pwish);
	// update the local_pwish with the new one
	m_local_pwish	= new_local_pwish;
	// send this command thru all the connections
	send_cmd_to_all_cnx(bt_cmd);
}

/** \brief function called by caller when pieceidx is newly available
 * 
 * - TODO extreme stubbing - to review
 * - TODO should it be renamed notify_piece_isavail ? if so rename it everywhere
 */
void	bt_utmsg_piecewish_t::declare_piece_newly_avail(size_t pieceidx)	throw()
{
	// log to debug
	KLOG_DBG("enter pieceidx=" << pieceidx);
	// sanity check - bt_swarm_t MUST declare this pieceidx as available
	DBG_ASSERT( bt_swarm->local_pavail().is_avail(pieceidx) );
	// if this pieceidx is not currently wished, do nothing and return now
	// - this may happen in case the piece is imported from outside bt_swarm_t
	//   - e.g. with neoip-casti
	if( local_pwish(pieceidx) == false )	return;


// TODO wow this is extremly unclear
// - what is the relation with the current status of the piece
// - should i test the local_pavail ? should i test the bt_pieceprec_t ?

	// update the local_pwish
	m_local_pwish.set(pieceidx, false);

	// send a nowish_pieceidx bt_cmd_t thru all the connections
	send_cmd_to_all_cnx( build_cmd_nowish_pieceidx(pieceidx) );
}

/** \brief function called by caller when pieceidx is nomore available
 *  
 * - TODO extreme stubbing - to review
 * - TODO should it be renamed notify_piece_unavail ? 
 */
void	bt_utmsg_piecewish_t::declare_piece_nomore_avail(size_t pieceidx)	throw()
{
	bt_pselect_vapi_t *	pselect_vapi	= bt_swarm->pselect_vapi();
	// log to debug
	KLOG_ERR("enter pieceidx=" << pieceidx);
	// sanity check - bt_swarm_t MUST declare this pieceidx as UNavailable
	DBG_ASSERT( bt_swarm->local_pavail().is_unavail(pieceidx) );
	// sanity check - local_pwish MUST NOT contain this pieceidx
	DBG_ASSERT( local_pwish()[pieceidx] == false );


// TODO wow this is extremly unclear
// - what is the relation with the current status of the piece
// - should i test the local_pavail ? should i test the bt_pieceprec_t ?
	
	// if this pieceidx is not needed, it is not wished so do nothing
	if( pselect_vapi->pieceprec(pieceidx).is_notneeded() )	return;
	// if this pieceidx is available, it is not wished so do nothing
	if( bt_swarm->local_pavail()[pieceidx] )		return;
	
	// update the local_pwish
	m_local_pwish.set(pieceidx, true);

	// send a nowish_pieceidx bt_cmd_t thru all the connections
	send_cmd_to_all_cnx( build_cmd_dowish_pieceidx(pieceidx) );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    bt_utmsg_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the string which identify this bt_utmsg_vapi_t
 */
std::string	bt_utmsg_piecewish_t::utmsgstr()		const throw()
{
#if 0
	return "piecewish";
#else	// 'scramble' the string to make it harder to guess
	return bt_id_t("piecewish").to_canonical_string();
#endif
}

/** \brief Construct a bt_utmsg_piecewish_cnx_t from a bt_swarm_full_utmsg_t
 */
bt_utmsg_cnx_vapi_t * bt_utmsg_piecewish_t::cnx_ctor(bt_swarm_full_utmsg_t *full_utmsg)	throw()
{
	bt_err_t	bt_err;	
	// create a bt_utmsg_piecewish_cnx_t for this bt_swarm_full_utmsg_t
	bt_utmsg_piecewish_cnx_t *piecewish_cnx;
	piecewish_cnx	= nipmem_new bt_utmsg_piecewish_cnx_t();
	bt_err		= piecewish_cnx->start(this, full_utmsg);
	DBG_ASSERT( bt_err.succeed() );
	// return the just build piecewish_cnx
	return piecewish_cnx;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the bt_utmsg_event_t
 */
bool bt_utmsg_piecewish_t::notify_utmsg_cb(const bt_utmsg_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( utmsg_cb );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*utmsg_cb);
	// notify the caller
	bool tokeep = utmsg_cb->neoip_bt_utmsg_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





