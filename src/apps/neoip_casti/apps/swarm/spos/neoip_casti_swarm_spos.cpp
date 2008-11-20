/*! \file
    \brief Definition of the \ref casti_swarm_spos_t

\par Brief Description
casti_swarm_spos_t handle all the start position stuff for the casti_swarm_t.
- this information is obtained by the bt_scasti_mod_vapi_t

*/

/* system include */
/* local include */
#include "neoip_casti_swarm_spos.hpp"
#include "neoip_casti_swarm.hpp"

#include "neoip_bt_cast_pidx.hpp"

#include "neoip_bt_scasti_vapi.hpp"
#include "neoip_bt_scasti_mod_vapi.hpp"

#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_unit.hpp"

#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
casti_swarm_spos_t::casti_swarm_spos_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_casti_swarm	= NULL;

}

/** \brief Destructor
 */
casti_swarm_spos_t::~casti_swarm_spos_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t casti_swarm_spos_t::start(casti_swarm_t *casti_swarm)		throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameter
	this->m_casti_swarm	= casti_swarm;

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
bt_ezswarm_t *	casti_swarm_spos_t::bt_ezswarm()	const throw()
{
	return m_casti_swarm->bt_ezswarm();
}

/** \brief Return a pointer on the bt_scasti_vapi_t of casti_swarm_t
 */
bt_scasti_vapi_t *	casti_swarm_spos_t::scasti_vapi()	const throw()
{
	return m_casti_swarm->scasti_vapi();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			notify_pieceq_changed
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief update the bt_cast_spos_arr_t when the pieceq boundary are changed
 *
 * - TODO put this function in the bt_cast_spos_arr_t
 * - remove all obsoletes bt_cast_spos_t (aka the ones *before* AND *nomore* in the pieceq)
 * - NOTE: as cast_spos_arr may contains byte_offset which are after the pieceq
 *   it removes only the bt_cast_spos_t which are before and nomore in pieceq
 *   but not the ones after pieceq
 * - NOTE: as the pieceq is only increasing, obsolete bt_cast_spos_t are only at the begining
 */
void	casti_swarm_spos_t::notify_pieceq_changed()		throw()
{
	bt_swarm_t *		bt_swarm	= bt_ezswarm()->share()->bt_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	bt_cast_pidx_t		cast_pidx	= bt_cast_pidx_t().modulo(bt_mfile.nb_piece());
	size_t			pieceq_beg	= m_casti_swarm->pieceq_beg;
	size_t			pieceq_end	= m_casti_swarm->pieceq_end;
	// if cast_spos_arr is empty, do nothing
	if( cast_spos_arr().empty() )	return;
	// start the loop - the exit condition is inside the loop
	while( true ){
		// get the oldest byte_offset in the m_cast_spos_arr
		const file_size_t & byte_offset	= m_cast_spos_arr[0].byte_offset();
		// determine the pieceidx of this byte_offset
		size_t	pieceidx	= bt_unit_t::totfile_to_pieceidx(byte_offset, bt_mfile);
		// if this byte_offset pieceidx IS in pieceq, leave the loop
		if( cast_pidx.index(pieceidx).is_in(pieceq_beg, pieceq_end) )	break;
		// log to debug
		KLOG_DBG("remove byte_offset=" << byte_offset << " from the bt_cast_spos_arr_t");
		// remove this bt_cast_spos_t from the bt_cast_spos_arr_t
		m_cast_spos_arr.remove(0);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			cast_spos_arr_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief gather the new bt_cast_spos_t from the bt_scasti_mod_vapi_t
 *
 * - NOTE: this will appends the bt_cast_spos_t of pieces not yet fully available
 *   - aka bt_cast_spos_arr_t may contains bt_cast_spos_t of pieces not yet fully availlable
 */
void	casti_swarm_spos_t::gather()		throw()
{
	bt_swarm_t *		bt_swarm	= bt_ezswarm()->share()->bt_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	bt_scasti_mod_vapi_t * 	mod_vapi	= scasti_vapi()->mod_vapi();
	// Append all new bt_scasti_mod_vapi_t::cast_spos to the cast_spos_arr
	// - NOTE: this will appends the bt_cast_spos_t of pieces not yet fully available
	while( 1 ){
		// try to pop the available keyframe byteoffset
		bt_cast_spos_t	cast_spos	= mod_vapi->cast_spos_pop();
		// if there are no more bt_cast_spos_t to dequeue, leave the loop
		if( cast_spos.is_null() )	break;
		// log to debug
		KLOG_DBG("add cast_spos=" << cast_spos << " to the cast_spos_arr");
		// handle the warparound the bt_mfile.totfile_size()
		// - thus m_cast_pos_arr contains only wrapped up byte_offset
		cast_spos.byte_offset	( cast_spos.byte_offset() % bt_mfile.totfile_size() );
		// append the cast_spos to the cast_spos_arr
		m_cast_spos_arr	+= cast_spos;
	}

	// NOTE: no need to delete obsolete bt_cast_spos_t as new one are not
	//       obsolete but definition.

	// if it is the first bt_cast_spos_t, start publishing
	if( !cast_spos_arr().empty() && !m_casti_swarm->is_published() )
		m_casti_swarm->start_publishing();
}

NEOIP_NAMESPACE_END;




