/*! \file
    \brief Definition of the \ref casti_swarm_scasti_t

\par Brief Description
casti_swarm_scasti_t handle all the httpi stuff for the casti_swarm_t

*/

/* system include */
/* local include */
#include "neoip_casti_swarm_scasti.hpp"
#include "neoip_casti_swarm.hpp"
#include "neoip_casti_swarm_profile.hpp"
#include "neoip_casti_swarm_spos.hpp"
#include "neoip_casti_swarm_udata.hpp"

#include "neoip_bt_cast_pidx.hpp"

#include "neoip_bt_httpi.hpp"
#include "neoip_bt_scasti_event.hpp"
#include "neoip_bt_scasti_mod_vapi.hpp"


#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_helper.hpp"
#include "neoip_bt_unit.hpp"

#include "neoip_bt_peerpick_casti.hpp"

#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
casti_swarm_scasti_t::casti_swarm_scasti_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_casti_swarm	= NULL;
	m_bt_httpi	= NULL;

}

/** \brief Destructor
 */
casti_swarm_scasti_t::~casti_swarm_scasti_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the bt_httpi_t
	nipmem_zdelete	m_bt_httpi;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
casti_swarm_scasti_t &	casti_swarm_scasti_t::profile(const casti_swarm_scasti_profile_t &m_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( m_profile.check() == bt_err_t::OK );
	// copy the parameter
	this->m_profile	= m_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t casti_swarm_scasti_t::start(casti_swarm_t *m_casti_swarm)		throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameter
	this->m_casti_swarm	= m_casti_swarm;

	// init the bt_httpi_t recv_rate estimator
	m_rate_estim	= rate_estim_t<size_t>(profile().rate_estim_arg());

	// start the bt_httpi_t
	bt_err_t	bt_err;
	m_bt_httpi	= nipmem_new bt_httpi_t();
	bt_err		= bt_httpi()->start(casti_swarm()->httpi_uri(), bt_ezswarm()->io_vapi()
						, casti_swarm()->scasti_mod(), this, NULL);
	if( bt_err.failed() )	return bt_err;
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Alias to ease readability
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on the bt_ezswarm_t of casti_swarm_t
 */
bt_ezswarm_t *	casti_swarm_scasti_t::bt_ezswarm()	const throw()
{
	return m_casti_swarm->bt_ezswarm();
}

/** \brief Return a pointer on the casti_swarm_udata_t of casti_swarm_t
 */
casti_swarm_udata_t *	casti_swarm_scasti_t::swarm_udata()	const throw()
{
	return m_casti_swarm->swarm_udata();
}

/** \brief Return a pointer on the casti_swarm_spos_t of casti_swarm_t
 */
casti_swarm_spos_t*	casti_swarm_scasti_t::swarm_spos()	const throw()
{
	return m_casti_swarm->swarm_spos();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_httpi_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_httpi_t to provide event
 */
bool	casti_swarm_scasti_t::neoip_bt_scasti_cb(void *cb_userptr, bt_scasti_vapi_t &cb_scasti_vapi
				, const bt_scasti_event_t &scasti_event)	throw()
{
	// sanity check - bt_ezswarm MUST be non null and in_share
	DBG_ASSERT( bt_ezswarm() );
	DBG_ASSERT( bt_ezswarm()->in_share() );

	// log to debug
	KLOG_DBG("enter bt_scasti_event_t=" << scasti_event);

	// in case of error, start a gracefull shutdown of the casti_swarm_t
	if( scasti_event.is_fatal() ){
		// delete bt_httpi_t now
		// - in case casti_swarm_t->gracefull_shutdown() dont delete this object
		// - NOTE: based on the regularity rules "if an obj notifies error, delete it"
		nipmem_zdelete	m_bt_httpi;
		// do casti_swarm()->gracefull_shutdown()
		casti_swarm()->gracefull_shutdown(scasti_event.to_string());
		// return dontkeep
		return false;
	}

	// handle the bt_ezswarm_event_t depending of its type
	switch(scasti_event.get_value()){
	case bt_scasti_event_t::CHUNK_AVAIL:	return handle_chunk_avail(scasti_event.get_chunk_avail());
	case bt_scasti_event_t::MOD_UPDATED:	return handle_mod_updated();
	default:	break;
	}

	// return tokeep
	return true;
}

/** \brief Handle a bt_scasti_event_t::CHUNK_AVAIL
 *
 * @return a tokeep for the bt_httpi_t
 */
bool	casti_swarm_scasti_t::handle_chunk_avail(const file_size_t &chunk_len)		throw()
{
	bt_swarm_t *	bt_swarm	= bt_ezswarm()->share()->bt_swarm();
	const bt_mfile_t &bt_mfile	= bt_swarm->get_mfile();

	// update the httpi_recv_rate
	m_rate_estim.update(chunk_len.to_size_t());

	// update the bt_peerpick_casti_t cnx_minrate with the current httpi_rate_estim
	// - NOTE: this bt_peerpick_vapi_t ensures that every connected peers is allocated
	//   at least the bandwidth to load at the bt_httpi_t throughput.
	// - this prevents to have multiple peer to cancel each other and none getting the
	//   data at the proper rates.
	bt_peerpick_vapi_t *    peerpick_vapi   = bt_swarm_helper_t::peerpick_vapi(bt_swarm);
	bt_peerpick_casti_t *   peerpick_casti  = dynamic_cast<bt_peerpick_casti_t *>(peerpick_vapi);
	DBG_ASSERT( peerpick_casti );
	peerpick_casti->cnx_minrate( rate_average() );


	// compute the old_httpi_pidx - i.e. the piece_idx BEFORE this bt_io_write_t
	// TODO BUG BUG here the totfile position MAY be outside the bt_mfile!!!!
	size_t		old_httpi_pidx	= bt_unit_t::totfile_to_pieceidx(bt_httpi()->cur_offset(), bt_mfile);
	// compute the new_httpi_pidx - i.e. the piece_idx AFTER this bt_io_write_t
	size_t		new_httpi_pidx	= bt_unit_t::totfile_to_pieceidx(bt_httpi()->cur_offset() + chunk_len, bt_mfile);

// TODO pass under a specific function
// - use the bt_cast_pidx_t in it to simplify it
	// notify all the pieceidx newly available after this bt_scasti_event_t::CHUNK_AVAIL
	for(size_t pieceidx = old_httpi_pidx; pieceidx < new_httpi_pidx; pieceidx++){
		// TODO this breaks the "no nested notification" rules
		// - this will make a notification to bt_ezswarm_cb_t
		// - what about having a zerotimer to deliver this event ? would avoid the
		//   nested notification
		//   - zerotimer_t here or in bt_swarm_t itself
		//   - within bt_swarm_t seems cleaner as it is more central
		bt_swarm->declare_piece_newly_avail(pieceidx % bt_mfile.nb_piece());
	}

	// update the pieceq_beg from the new_httpi_pidx
	// - TODO BUG httpi_pidx points on partial piece, and pieceq_end MUST be
	//   available so fully available
	casti_swarm()->pieceq_end	= new_httpi_pidx % bt_mfile.nb_piece();
	// sanity check - the new pieceq_end MUST be available
	// - TODO there is a bug here i think... i think the pieceq_end contains piece
	//   which are not yet fully avail
	// - aka this assert it triggered
	//DBG_ASSERT( bt_swarm->local_pavail().is_avail(pieceq_end));

// TODO pass under a specific function
// - use the bt_cast_pidx_t in it to simplify it
	// delete the piece which is no more in the queue
	for(size_t pieceidx = old_httpi_pidx; pieceidx < new_httpi_pidx; pieceidx++){
		size_t	pieceq_maxlen	= casti_swarm()->profile().pieceq_maxlen();
		// compute the pieceidx at the begining of the queue
		// - TODO not sure how it is coded
		// - ugly for sure. this kind of computation should be externalized
		size_t	pieceidx_2del	= (pieceidx + bt_mfile.nb_piece()
					- pieceq_maxlen) % bt_mfile.nb_piece();
		// if this piece is not available, goto the next
		// - this may happen when the piece_queue is not yet full
		//   aka at the begining of the bt_httpi_t
		if( bt_swarm->local_pavail().is_unavail(pieceidx_2del) )	continue;
		// notify the bt_swarm_t that this piece is no more available
		bt_swarm->declare_piece_nomore_avail(pieceidx_2del);
		// update the pieceq_beg if it just got declared nomore-avail
		if( pieceidx_2del == casti_swarm()->pieceq_beg ){
			// increment the pieceq_beg
			casti_swarm()->pieceq_beg	+= 1;
			casti_swarm()->pieceq_beg	%= bt_mfile.nb_piece();
			// sanity check - the new pieceq_beg MUST be available
			DBG_ASSERT( bt_swarm->local_pavail().is_avail(casti_swarm()->pieceq_beg));
		}
	}

	// notify bt_swarm_spos_t that pieceq has been changed
	swarm_spos()->notify_pieceq_changed();

	// notify casti_swarm_udata_t that pieceq has changed
	swarm_udata()->notify_pieceq_changed();

	// return tokeep
	return true;
}

/** \brief Handle a bt_scasti_event_t::CHUNK_AVAIL
 *
 * @return a tokeep for the bt_httpi_t
 */
bool	casti_swarm_scasti_t::handle_mod_updated()				throw()
{
	// gather all the new bt_cast_spos_t from the bt_scasti_mod_vapi_t
	swarm_spos()->gather();
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END;




