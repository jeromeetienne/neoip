/*! \file
    \brief Class to handle the bt_relay_t

\par Brief Description
\ref bt_relay_t gather a bunch of bt_swarm_t and make them act as relay.
aka they try to give as much as possible and take as few as possible out of the 
swarm.

\par issue about performance
- ISSUE: after a while, the upload rate is not maximise
- diagnostic: not enougth remote peer wish to download piece from the local peer
  - it may be observed in bt_swarm_wikidbg with the 'locavail_remwish_avg'
- possible reason: the value of a piece is not properly estimated
  - especially the giveexpect which assume that each of the local piece got
    a probability to be downloaded from the local peer calculated by
    remote_pwish / remote_pavail
  - the issue comes from the estimation of the probability that remote peer
    download a given piece from the local peer as opposed as from any other
    remote peer which has it available too.
  - i 'feel' that this is wrong... not sure why
    - likely because remote peer which have the piece available likely have other
      pieces too. while the local peer got only a small amount of piece
      so the probability is higher for local peer
- apparently to have a pool of 10mbyte decrease significantly the performance too
  - at least with a pool of 30mbyte, it is able to give more
- the goal is to saturate the upload all the time

\par Implementation notes
- the integration of bt_relay_t to external tools is unclear
  - currently there is a dedicated apps called neoip-btrelay which does only the relay


\par Old notes about connection establishement (unclear)
- LATER: to massively delete bt_swarm_full_t changes dramatically the 
  estimated value of the piece and thus trigger new download. discarding previously
  downloaded data for 'nothing'
- TODO if no piece is needed, dont establish bt_swarm_full_t ?
  - Thus if the bt_swarm_t is deemed not to be relayed, it wont waste resource
    for handling bt_swarm_full_t which not gonna be used
    - in relation with 'determine from the tracker seed/leech if to be relayed or not'
  - no piece needed, (aka all pieceprec_arr_t == NOTNEEDED)
  - modify bt_swarm_t for this
  - in the notify_pieceprec_change, if it is the case, close all bt_swarm_full_t
  - and in the bt_swarm_full_t creation, in peersrc or in the bt_swarm_full_t
    limiter 
- TODO a bt_relay_t should connect more non seed
  - because it is not possible to give to a seed
  - i dunno how to convert this principle into an algorithm tho :)
  - NOTE: this is very important, i experimented a bit about this with ubuntu 7.04 torrent
    - if the number of remote peer willing to download from the local peer is
      too low, the amount of data given to the swarm do not saturate the upload capacity
    - likely because ubuntu torrent are very well seeded
    - it the number of allowed bt_swarm_full_t is kept at 60, there are around 4-5
      leech connected. this is not saturating the upload capacity of the local peer
    - if the number of bt_swarm_full_t is increased to 500 (instead of the default 60)
      it start to have a regular number of 20-30 leech connected and they produces
      an steady upload on the local peer 
  
*/

/* system include */
/* local include */
#include "neoip_bt_relay.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_helper.hpp"
#include "neoip_bt_unit.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_relay_t::bt_relay_t()		throw()
{
	// zero some field
}

/** \brief Destructor
 */
bt_relay_t::~bt_relay_t()		throw()
{
	// sanity check - the swarm_db MUST be empty at this point
	// - it is up to the caller to unlink them *BEFORE* deleting bt_relay_t
	DBG_ASSERT( swarm_db().empty() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
bt_relay_t &bt_relay_t::profile(const bt_relay_profile_t &p_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_relay_t::start()	throw()
{
	// log to debug 
	KLOG_DBG("enter");

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/bt_relay_" + OSTREAMSTR(this));

	// init the eval_delaygen and start the timeout_t
	eval_delaygen	= delaygen_t(profile().eval_delaygen());
	eval_timeout.start(eval_delaygen.current(), this, NULL);
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_swarm_t linking
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Dolink a bt_swarm_t to this bt_relay_t
 */
void	bt_relay_t::swarm_dolink(bt_swarm_t *bt_swarm)	throw()
{
	// sanity check - the bt_swarm_t MUST NOT be already in the database
	DBG_ASSERT( m_swarm_db.find(bt_swarm) == m_swarm_db.end() );
	// Set the bt_pieceprec_arr_t to bt_pieceprec_t::NOTNEEDED
	// - thus all added bt_swarm_t starts with no piece needed and it is decided later
	bt_pieceprec_arr_t	pieceprec_arr;
	pieceprec_arr.assign( bt_swarm->get_mfile().nb_piece(), bt_pieceprec_t::NOTNEEDED );
	bt_swarm_helper_t::set_pselect_fixed_pieceprec_arr(bt_swarm, pieceprec_arr);
	// put this bt_swarm_t in the database
	m_swarm_db.insert(bt_swarm);
}

/** \brief Unlink a bt_swarm_t to this bt_relay_t
 */
void	bt_relay_t::swarm_unlink(bt_swarm_t *bt_swarm)	throw()
{
	// sanity check - the bt_swarm_t MUST be already in the database
	DBG_ASSERT( m_swarm_db.find(bt_swarm) != m_swarm_db.end() );
	// remove this bt_swarm_t from the database
	m_swarm_db.erase(bt_swarm);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool 	bt_relay_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// notify the expiration of the timeout to the delaygen
	eval_delaygen.notify_expiration();

	// do a swarmpiece_db reevaluation
	swarmpiece_db_eval();

	// set the next timer
	eval_timeout.start(eval_delaygen.post_inc(), this, NULL);
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			swarmpiece_db
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void	bt_relay_t::swarmpiece_db_eval()				throw()
{
	swarmpiece_db_t	swarmpiece_db;
	// build the swarmpiece_db
	swarmpiece_db_build(swarmpiece_db);
	// sort the swarmpiece_db
	swarmpiece_db_sort(swarmpiece_db);
	// process the swarmpiece_db
	swarmpiece_db_process(swarmpiece_db);
}

/** \brief Build the swarmpiece_db
 */
void	bt_relay_t::swarmpiece_db_build(swarmpiece_db_t &swarmpiece_db)	throw()
{
	swarm_db_t::iterator	iter;
	// sanity check - the swarmpiece_db MUST be empty 
	DBG_ASSERT( swarmpiece_db.empty() );
	// go thru the whole swarm_db
	for(iter = m_swarm_db.begin(); iter != m_swarm_db.end(); iter++){
		bt_swarm_t *		bt_swarm	= *iter;
		const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
		// TODO maybe some filter not to add from bt_swarm_t
		// - maybe some stuff on the nb_peer from peersrc to guess global wish factor

		// go thru each piece of this bt_swarm_t
		for(size_t pieceidx = 0; pieceidx < bt_mfile.nb_piece(); pieceidx++){
			swarmpiece_t	swarmpiece(bt_swarm, pieceidx, gainexpect(bt_swarm, pieceidx));
			// add this swarmpiece to the swarmpiece_db
			swarmpiece_db.push_back(swarmpiece);
		}
	}
	
	// do a random permutation of the swarmpiece_db
	// - it adds some randomness in the order thus if there are several relays 
	//   they are less likely to download the same pieces
	std::random_shuffle(swarmpiece_db.begin(), swarmpiece_db.end());
}

/** \brief Sort the swarmpiece_db
 */
void	bt_relay_t::swarmpiece_db_sort(swarmpiece_db_t &swarmpiece_db)	throw()
{
	// sort the swarmpiece_db
	// - the sorting criteria is the swarmpiece_t::gainexpect 
	// - it is handled directly by the comparison operator of swarmpiece_t
	std::sort(swarmpiece_db.begin(), swarmpiece_db.end());
}

/** \brief process the swarmpiece_db
 */
void	bt_relay_t::swarmpiece_db_process(swarmpiece_db_t &swarmpiece_db)	throw()
{
	size_t			swarmpiece_idx	= 0;
	file_size_t		cur_cache_size	= 0;
	bt_pieceprec_t		pieceprec_max	= 1000;
	bt_pieceprec_t		pieceprec_min	= bt_pieceprec_t::LOWEST;
	bt_pieceprec_t		pieceprec_delta	= pieceprec_max.value() - pieceprec_min.value();	
	double			gainexpect_max;

	// compute the gainexpect_max
	if( !swarmpiece_db.empty() )	gainexpect_max	= swarmpiece_db[0].gainexpect;
	else				gainexpect_max	= 0;

	// build the pieceprec_arr_db
	std::map<bt_swarm_t *, bt_pieceprec_arr_t>	pieceprec_arr_db;
	swarm_db_t::iterator				iter;
	for(iter = m_swarm_db.begin(); iter != m_swarm_db.end(); iter++){
		bt_swarm_t *		bt_swarm	= *iter;
		const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile(); 
		// create the pieceprec_arr
		bt_pieceprec_arr_t	pieceprec_arr;
		pieceprec_arr.assign(bt_mfile.nb_piece(), bt_pieceprec_t::NOTNEEDED);
		// put it in the pieceprec_arr_db
		pieceprec_arr_db[bt_swarm]	= pieceprec_arr;
	}

	// populate the pieceprec_arr_db
	for(swarmpiece_idx = 0; swarmpiece_idx < swarmpiece_db.size(); swarmpiece_idx++){
		const swarmpiece_t &	swarmpiece	= swarmpiece_db[swarmpiece_idx];
		bt_swarm_t *		bt_swarm	= swarmpiece.bt_swarm;
		
		// update the cur_cache_size
		cur_cache_size	+= piecelen(swarmpiece.bt_swarm, swarmpiece.pieceidx);
		// leave the loop if it cur_cache_size would become greater than the max
		if( cur_cache_size > profile().max_cache_size() )	break;
		
		// sanity check - the bt_swarm_t MUST be in the pieceprec_arr_db
		DBG_ASSERT(pieceprec_arr_db.find(bt_swarm) != pieceprec_arr_db.end());
		// get the pieceprec_arr for this bt_swarm
		bt_pieceprec_arr_t &	pieceprec_arr	= pieceprec_arr_db[bt_swarm];
		// compute the bt_pieceprec_t for this swarmpiece_t
		bt_pieceprec_t	bt_pieceprec;
		bt_pieceprec	= size_t(swarmpiece.gainexpect / gainexpect_max
					* pieceprec_delta.value()) + pieceprec_min.value();
		if( bt_pieceprec.is_notneeded() ){
			KLOG_ERR("gainexpect=" << swarmpiece.gainexpect << " gainexpect_max=" << gainexpect_max);
			KLOG_ERR("pieceprec_delta=" << pieceprec_delta);
			KLOG_ERR("pieceprec_min=" << pieceprec_min);
		}
		// sanity check - bt_pieceprec_t MUST be needed
		DBG_ASSERT( bt_pieceprec.is_needed() );
		// set the bt_pieceprec_t in the pieceprec_arr
		pieceprec_arr[swarmpiece.pieceidx]	= bt_pieceprec;
	}
	
	// Set all the bt_pieceprec_arr_t to their respective bt_swarm_t
	// - this will set the selected pieces precedence
	// - this will delete the unselected pieces
	for(iter = m_swarm_db.begin(); iter != m_swarm_db.end(); iter++){
		bt_swarm_t *		bt_swarm	= *iter;
		bt_pieceprec_arr_t &	pieceprec_arr	= pieceprec_arr_db[bt_swarm];
		// set the bt_pieceprec_arr_t to the bt_swarm_t and delete notneeded piece
		bt_swarm_helper_t::pieceprec_arr_del_notneeded(bt_swarm, pieceprec_arr);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the amount of remote peer which wish to download this pieceidx
 */
size_t	bt_relay_t::remote_pwish(bt_swarm_t *bt_swarm, size_t pieceidx)	throw()
{
	// get the bt_pselect_vapi_t for this bt_swarm_t 
	bt_pselect_vapi_t *pselect_vapi	= bt_swarm->pselect_vapi();
	// return the amount of time this is declared remotly wished
	return pselect_vapi->remote_pwish(pieceidx);
}

/** \brief Return the amount of remote peer for which this pieceidx is already available
 */
size_t	bt_relay_t::remote_pavail(bt_swarm_t *bt_swarm, size_t pieceidx)	throw()
{
	// get the bt_pselect_vapi_t for this bt_swarm_t 
	bt_pselect_vapi_t *pselect_vapi	= bt_swarm->pselect_vapi();
	// return the amount of time this is declared remotly available
	return pselect_vapi->remote_pfreq(pieceidx);
}

/** \brief Return the piecelen for this pieceidx
 */
size_t	bt_relay_t::piecelen(bt_swarm_t *bt_swarm, size_t pieceidx)		throw()
{
	return bt_unit_t::pieceidx_to_piecelen(pieceidx, bt_swarm->get_mfile());
}

/** \brief Return the amount of byte available for this pieceidx
 */
size_t	bt_relay_t::pieceavaillen(bt_swarm_t *bt_swarm, size_t pieceidx)	throw()
{
	return bt_swarm_helper_t::pieceidx_anyavail_len(bt_swarm, pieceidx);
}

/** \brief return the amount of byte which need to be taken from the swarm to 
 *         have this piece locally available.
 */
size_t	bt_relay_t::takecost(bt_swarm_t *bt_swarm, size_t pieceidx)		throw()
{
	return piecelen(bt_swarm, pieceidx) - pieceavaillen(bt_swarm, pieceidx);
}

/** \brief return the expectation of byte given to the swarm for pieceidx IIF locally avail
 * 
 * - aka, ASSUMING the piece is already locally available, how many byte may 
 *   be given for this pieceidx.
 * - proba that a remote peer download this piece from local peer
 *   = how many remote peer want to download this piece / how many peer have it available
 * - giveexpect = piecelen * proba that a remote peer download it from us
 */
double	bt_relay_t::giveexpect(bt_swarm_t *bt_swarm, size_t pieceidx)		throw()
{
	// - the +1 in "remote_pavail(pieceidx) + 1" is to take into account 
	//   that this piece will be available in all those remote peers *AND* in 
	//   the local peer too. so +1 for the local peer.
	// - this formula is directly derived from PieceLen * WishFactor with a
	//   correction to take into account that the local peer will be able 
	//   to give this piece too.
	return piecelen(bt_swarm, pieceidx) * (double)remote_pwish(bt_swarm, pieceidx)
				/ (remote_pavail(bt_swarm, pieceidx) + 1);
}

/** \brief Return the gain expectation for this piece
 * 
 * - aka how many byte may be given for this pieceidx, no matter if this piece is 
 *   locally available or not
 */
double	bt_relay_t::gainexpect(bt_swarm_t *bt_swarm, size_t pieceidx)	throw()
{
	return giveexpect(bt_swarm, pieceidx) - takecost(bt_swarm, pieceidx);
}

NEOIP_NAMESPACE_END





