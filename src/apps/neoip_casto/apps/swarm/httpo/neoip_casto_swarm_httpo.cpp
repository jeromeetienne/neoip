/*! \file
    \brief Definition of the \ref casto_swarm_httpo_t

- TODO reorganize the functions order, this is a mess

\par Brief Description
casto_swarm_httpo_t handles all the bt_httpo_full_t attached to the casto_swarm_t
    
*/

/* system include */
/* local include */
#include "neoip_casto_swarm_httpo.hpp"
#include "neoip_casto_swarm.hpp"
#include "neoip_casto_apps.hpp"

#include "neoip_bt_cast_pidx.hpp"

#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_resumedata_helper.hpp"
#include "neoip_bt_unit.hpp"

#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_event.hpp"

#include "neoip_mimediag.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
casto_swarm_httpo_t::casto_swarm_httpo_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_casto_swarm	= NULL;
}

/** \brief Destructor
 */
casto_swarm_httpo_t::~casto_swarm_httpo_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete httpo_full_db if needed
	httpo_full_db_dtor("Service Unavailable");
}


/** \brief autodelete this object, and return false for tokeep convenience
 */
bool	casto_swarm_httpo_t::autodelete(const std::string &reason)			throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR(reason);
	// delete httpo_full_db if needed
	httpo_full_db_dtor(reason);
	// autodelete the casto_swarm - which will delete the casto_swarm_httpo_t
	// TODO to change. simply make casto_swarm_t to go thru stopping
	// - similar to casti_swarm_t. aka if another casto_swarm_t is needed
	//   while a previous one is in stopping, remvoe the previous one
	//   and have the new one to take precedence
	nipmem_delete	m_casto_swarm;
	// return false
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t casto_swarm_httpo_t::start(casto_swarm_t *p_casto_swarm)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->m_casto_swarm	= p_casto_swarm;
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			TODO to comment
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the bt_cast_mdata_t from casto_swarm_t
 */
const bt_cast_mdata_t &	casto_swarm_httpo_t::cast_mdata()	const throw()
{
	return casto_swarm()->cast_mdata();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			TODO to comment
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle the bt_swarm_event_t::PIECE_NEWLY_AVAIL
 */
bool	casto_swarm_httpo_t::handle_piece_newly_avail(size_t pieceidx)		throw()
{
	// resync all unstarted bt_httpo_full_t before notifying the new pieceidx
	httpo_full_resync_if_needed();
	
	// notify this new data to each bt_httpo_full_t within the httpo_full_db
	std::list<bt_httpo_full_t *>::iterator	iter;
	for(iter = m_httpo_full_db.begin(); iter != m_httpo_full_db.end(); iter++){
		bt_httpo_full_t *httpo_full	= *iter;
		// notify this bt_httpo_full_t of the new data
		httpo_full->notify_newly_avail_piece(pieceidx);
	}

#if 1	// NOTE: only some display to help diagnose/debug
	bt_ezswarm_t *	bt_ezswarm	= casto_swarm()->bt_ezswarm;
	bt_swarm_t * 	bt_swarm	= bt_ezswarm->share()->bt_swarm();
	const bt_mfile_t &bt_mfile	= bt_swarm->get_mfile();
	for(iter = m_httpo_full_db.begin(); iter != m_httpo_full_db.end(); iter++){
		bt_httpo_full_t *httpo_full	= *iter;
		// compute the current pieceidx pointed by this httpo_full
		size_t		cur_pieceidx;
		cur_pieceidx	= bt_unit_t::totfile_to_pieceidx(httpo_full->current_pos(), bt_mfile);
		// create a bt_cast_pidx_t based on it
		bt_cast_pidx_t	cast_pidx;
		cast_pidx	= bt_cast_pidx_t().index(cur_pieceidx).modulo(bt_mfile.nb_piece());
		// compute its relative position within the pieceq
		size_t		npiece_before, npiece_after, npiece_total;
		npiece_before	= cast_pidx.is_nb_piece_after(cast_mdata().pieceq_beg());
		npiece_after	= cast_pidx.is_nb_piece_before(cast_mdata().pieceq_end());
		npiece_total	= npiece_before + npiece_after;
		double ratio_pos= double(npiece_before) / npiece_total;
		// display the relative position
		KLOG_ERR("httpo_full on before=" << npiece_before << " after=" << npiece_after
					<< " total=" << npiece_total
					<< " ratio=" << string_t::ratio_string(ratio_pos));
	}
#endif

	// if httpo_full_db is now empty, autodelete
	if( m_httpo_full_db.empty() )	return autodelete("No more httpo_full_t");
	
	// return tokeep
	return true;
}


/** \brief check all the bt_httpo_full_t are overrun 
 * 
 * - being in overrun meaning "pointing to data no more in casti pieceq"
 * - WARNING: this function may delete some/all bt_httpo_full_t
 */
void	casto_swarm_httpo_t::httpo_full_check_overrun()					throw()
{
	bt_ezswarm_t *	bt_ezswarm	= casto_swarm()->bt_ezswarm;
	bt_swarm_t * 	bt_swarm	= bt_ezswarm->share()->bt_swarm();
	const bt_mfile_t &bt_mfile	= bt_swarm->get_mfile();
	
	// compute the pieceq_end assuming the pieceq is full
	size_t		pieceq_end;
	pieceq_end	= (cast_mdata().pieceq_beg() + cast_mdata().pieceq_maxlen()) % bt_mfile.nb_piece();
	
	// copy of the list, as elements may be deleted during walk
	std::list<bt_httpo_full_t *>	db_copy	= m_httpo_full_db;
	std::list<bt_httpo_full_t *>::iterator	iter;
	// go thru the m_httpo_full_db copy
	for(iter = db_copy.begin(); iter != db_copy.end(); iter++){
		bt_httpo_full_t * httpo_full	= *iter;
		// compute the current pieceidx pointed by this httpo_full
		size_t		cur_pieceidx;
		cur_pieceidx	= bt_unit_t::totfile_to_pieceidx(httpo_full->current_pos(), bt_mfile);
		// create a bt_cast_pidx_t based on it
		bt_cast_pidx_t	cast_pidx;
		cast_pidx	= bt_cast_pidx_t().index(cur_pieceidx).modulo(bt_mfile.nb_piece());
		// IF bt_httpo_full_t current_pos() is inside in the bt_cast_mdata_t pieceq, no overrun
		if( cast_pidx.is_in(cast_mdata().pieceq_beg(), pieceq_end) )
			continue;

		// log the error
		KLOG_ERR("Deleting a bt_httpo_full_t as it is in overrun");
		// delete the bt_httpo_full_t from the 'original' database
		m_httpo_full_db.remove(httpo_full);
		nipmem_zdelete	httpo_full;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_httpo_full_t starting
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief push a new bt_httpo_full_t on this casto_swarm_httpo_t
 * 
 * - NOTE: the bt_httpo_full_t ownership is transfered to this object
 */
void	casto_swarm_httpo_t::httpo_full_push(bt_httpo_full_t *httpo_full)	throw()
{
	bt_ezswarm_t *	bt_ezswarm	= casto_swarm()->bt_ezswarm;
	// log to debug 
	KLOG_ERR("enter");
	// add the new httpo_full to the database
	m_httpo_full_db.push_back(httpo_full);
	// if bt_ezswarm_t is in_share, start it now
	if( bt_ezswarm && bt_ezswarm->in_share() )	httpo_full_start_one(httpo_full);
}

/** \brief start all the bt_httpo_full_t from the m_httpo_full_db
 */
void	casto_swarm_httpo_t::httpo_full_start_all()					throw()
{
	// copy of the list, as elements may be deleted during walk
	std::list<bt_httpo_full_t *>	db_copy	= m_httpo_full_db;
	std::list<bt_httpo_full_t *>::iterator	iter;
	// go thru the m_httpo_full_db copy
	for(iter = db_copy.begin(); iter != db_copy.end(); iter++){
		bt_httpo_full_t*httpo_full = *iter;
		httpo_full_start_one(httpo_full);
	}
}

/** \brief Start one bt_httpo_full_t
 * 
 * - WARNING: this function may delete the bt_httpo_full_t
 */
void	casto_swarm_httpo_t::httpo_full_start_one(bt_httpo_full_t *httpo_full)	throw()
{
	bt_err_t	bt_err;
	// start the bt_httpo_full_t immediatly - as bt_ezswarm->in_share() 
	// - this is used when new connections occurs during the bt_ezswarm_state_t::SHARE
	bt_err		= httpo_full_do_start(httpo_full);
	if( bt_err.failed() ){
		// log the error
		KLOG_ERR("Cant start bt_httpo_full_t due to " << bt_err);
		// delete the bt_httpo_full_t
		m_httpo_full_db.remove(httpo_full);
		nipmem_zdelete	httpo_full;
	}
}

/** \brief Do the start for this bt_httpo_full_t - it doesnt handle the database 
 */
bt_err_t	casto_swarm_httpo_t::httpo_full_do_start(bt_httpo_full_t *httpo_full)	throw()
{
	bt_ezswarm_t *		bt_ezswarm	= casto_swarm()->bt_ezswarm;
	bt_swarm_t *		bt_swarm	= bt_ezswarm->share()->bt_swarm();
	const http_reqhd_t &	http_reqhd	= httpo_full->http_reqhd();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	// log to debug 
	KLOG_ERR("enter");

	// sanity check - the bt_mfile MUST have a single subfile
	DBG_ASSERT( bt_mfile.subfile_arr().size() == 1 );

	// if the http_reqhd_t contains a range rfc2616.14.35, return an error
	// - there is a special case where req_range is (0-max). In this case
	//   the req_range is just ignored.
	//   - this range is sent by vlc to test if the server support range request
	file_range_t	req_range	= http_reqhd.range();
	if( !req_range.is_null() && req_range != file_range_t(0, file_size_t::MAX) )
		return bt_err_t(bt_err_t::ERROR, "Range request are not valid");

	// build the reply header
	http_rephd_t	http_rephd;
	http_rephd.version(http_version_t::V1_1).status_code(200).reason_phrase("OK");
	http_rephd.accept_ranges(false);
	// add the mimetype of the content - rfc2616.14.17
	const bt_mfile_subfile_t&mfile_subfile	= bt_mfile.subfile_arr()[0];
	std::string	mimetype_str	= mimediag_t::from_file_path(mfile_subfile.mfile_path());
	if( !mimetype_str.empty() )	http_rephd.header_db().update("Content-Type", mimetype_str);
	
	// setup "Cache-Control" field to never cache - rfc2616.14.9
	// - "no-cache" - dont cache the data - rfc2616.14.9.1
	// - "no-store" - dont store the data - rfc2616.14.9.2
	http_rephd.header_db().update("Cache-Control", "no-cache; no-store");

	// add 'Connection: close' - rfc2616.14.10
	// HTTP/1.1 applications that do not support persistent connections MUST include the "close" connection option in every message. 
	http_rephd.header_db().update("Connection", "close");

	// log to debug
	KLOG_ERR("http_reqhd=" << http_reqhd << " http_rephd=" << http_rephd);

	// determine the rep_range - the range to reply in bt_httpo_full_t
	// - its end is always file_size_t::MAX - as the stream is endless
	file_size_t  reply_beg	= httpo_full_initial_pos(httpo_full);		
	file_range_t rep_range	= file_range_t(reply_beg, file_size_t::MAX);
	
	// sanity check - cast_mdata slide_curs_nbpiece_hint MUST be <= than bt_mfile.nb_piece()
	DBG_ASSERT( cast_mdata().slide_curs_nbpiece_hint() <= bt_mfile.nb_piece() );
	// compute the current idx_offsetfor the slide_curs
	bt_pselect_slide_curs_arg_t	curs_arg;
	size_t			idx_end	= cast_mdata().slide_curs_nbpiece_hint()-1;
	curs_arg.offset		(bt_unit_t::totfile_to_pieceidx(rep_range.beg(), bt_mfile));
	curs_arg.has_circularidx(true);

// TODO fix this bt_pieceprec_arr_t sutff
// - which one to use
// - why 
// - who decides it ? casto or casti ?
	// set a bt_pieceprec_arr_t
	bt_pieceprec_arr_t	pieceprec_arr;
	pieceprec_arr.assign(idx_end+1, bt_pieceprec_t::NOTNEEDED);
#if 0
	// strictly decreasing precedence
	pieceprec_arr.set_range_decrease(0, idx_end, 1000);
#elif 1
	// strictly decreasing precedence for the first 10 piece, and equal for the rest
	if( idx_end > 10 ){
		pieceprec_arr.set_range(0, 9, 1000, 991);
		pieceprec_arr.set_range(10, idx_end, 990, 990);
	}else{
		pieceprec_arr.set_range_decrease(0, idx_end, 1000);
	}
#else
	// all equal precedence
	pieceprec_arr.set_range_equal(0, idx_end, 1000);
#endif
	curs_arg.pieceprec_arr	(pieceprec_arr);

	// build the bt_httpo_full_arg_t
	bt_httpo_full_arg_t	httpo_full_arg;
	httpo_full_arg.bt_swarm		( bt_swarm	);
	httpo_full_arg.http_rephd	( http_rephd	);
	httpo_full_arg.range_tosend	( rep_range	);
	httpo_full_arg.curs_arg		( curs_arg	);
	httpo_full_arg.header_datum	( cast_mdata().prefix_header()	);

	// start this bt_httpo_full_t
	bt_err_t bt_err	= httpo_full->start(httpo_full_arg, this, NULL);
	if( bt_err.failed() )	return bt_err;
	
	// handle the "httpo_maxrate" from the http_reqhd.uri() variable
	if( http_reqhd.uri().var().contain_key("httpo_maxrate") ){
		const std::string & value_str	= http_reqhd.uri().var().get_first_value("httpo_maxrate");
		size_t		httpo_maxrate	= string_t::to_uint32(value_str);
		// set the bw_limit in the bt_httpo_full_t
		httpo_full->xmit_maxrate(httpo_maxrate);
	}

	// return no error
	return bt_err;
}


/** \brief Delete the whole http_full_db returning a http error if needed
 */
void	casto_swarm_httpo_t::httpo_full_db_dtor(const std::string &reason)	throw()
{
	// delete the httpo_full_db if needed
	while( !m_httpo_full_db.empty() ){
		bt_httpo_full_t * httpo_full	= m_httpo_full_db.front();
		// remote it from the m_httpo_full_db 
		m_httpo_full_db.pop_front();
		// reply the http error 503 "Service Unavailable" - rfc2616.10.5.4
		// - IIF httpo_full is not started, because the error is in the header
		if( !httpo_full->is_started() ){
			// build the http_rephd_t
			http_rephd_t	http_rephd; 
			http_rephd.version(http_version_t::V1_1).status_code(503)
							.reason_phrase(reason);
			// Start the bt_httpo_full_t but in reply error
			httpo_full->start_reply_error( http_rephd );
		}
		// delete the object itself
		nipmem_zdelete httpo_full;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_httpo_full_t position synching
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief check all the bt_httpo_full_t are in sync with the bt_cast_data_t
 * 
 * Description of the 'synchronization' for bt_httpo_full_t:
 * - if the bt_httpo_full_t has not yet sent data, resync its position
 *   to the initial one.
 */
void	casto_swarm_httpo_t::httpo_full_resync_if_needed()			throw()
{
	// go thru the m_httpo_full_db copy
	std::list<bt_httpo_full_t *>::iterator	iter;
	for(iter = m_httpo_full_db.begin(); iter != m_httpo_full_db.end(); iter++){
		bt_httpo_full_t *	httpo_full = *iter;
		// if bt_httpo_full_t HAS already started delivering data, goto the next
		if( httpo_full->sent_length() > 0 )	continue;
		// compute the range_tosend
		file_size_t 	reply_beg	= httpo_full_initial_pos(httpo_full);
		file_range_t	range_tosend	= file_range_t(reply_beg, file_size_t::MAX);
		// notify the new range_tosend to httpo_full
		httpo_full->reinit_range_tosend(range_tosend);
	}
}

/** \brief Compute the initial position of a httpo_full_t according to current bt_cast_mdata_t
 */
file_size_t	casto_swarm_httpo_t::httpo_full_initial_pos(bt_httpo_full_t *httpo_full)	throw()
{
	const bt_cast_spos_arr_t &	cast_spos_arr	= cast_mdata().cast_spos_arr();
	int64_t				casti_dtime	= casto_swarm()->casti_dtime();
	// compute the current date_t::present in neoip-casti
	date_t	casti_present	= delay_t(date_t::present().to_uint64() - casti_dtime);
	// compute the casti_date to obtain
	// - TODO this delay_t::from_sec should not be hardcoded
	//   - should be in the cast_mdata
	// - TODO this hide a lot of issue because what if the delay is too long 
	//   and the stream go so fast that the delta is out of the pieceq... 
	date_t	casti_date	= casti_present - delay_t::from_sec(25);
#if 1	/*************** experiment to get less latency	***********************/
	// - the piece_size vs stream-bitrate is important too, because the 
	//   piece is downloadable IIF fully received
	casti_date	= casti_present - delay_t::from_sec(0);
#endif
	// find the bt_cast_spos_t with the closest_casti_date
	const bt_cast_spos_t &	cast_spos	= cast_spos_arr.closest_casti_date(casti_date);
	// log to debug
	KLOG_ERR("cast_spos=" << cast_spos << " casti_date=" << casti_date);
	// return the cast_spos.byte_offset
	return cast_spos.byte_offset(); 
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     bt_httpo_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback for bt_httpo_full_t
 */
bool	casto_swarm_httpo_t::neoip_bt_httpo_full_cb(void *cb_userptr, bt_httpo_full_t &cb_bt_httpo_full
					, const bt_httpo_event_t &httpo_event)	throw()
{
	bt_httpo_full_t*httpo_full	= &cb_bt_httpo_full;
	// log to debug
	KLOG_WARN("enter httpo_event=" << httpo_event);
	// sanity check - bt_httpo_event_t MUST be is_full_ok()
	DBG_ASSERT( httpo_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( httpo_event.get_value() ){
	case bt_httpo_event_t::CNX_CLOSED:
			// remove it from the httpo_full_db
			m_httpo_full_db.remove(httpo_full);
			// delete the bt_httpo_full_t object itself
			nipmem_zdelete httpo_full;
			// if httpo_full_db is now empty, autodelete
			if( m_httpo_full_db.empty() )	return autodelete("No more httpo_full_t");
			// return dontkeep - as the httpo_full has just been deleted
			return false;
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END;




