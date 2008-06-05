/*! \file
    \brief Header of the bt_swarm_helper_t class
    
*/


#ifndef __NEOIP_BT_SWARM_HELPER_HPP__ 
#define __NEOIP_BT_SWARM_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_sched.hpp"
#include "neoip_bt_swarm_sched_full.hpp"
#include "neoip_bt_pselect_vapi.hpp"
#include "neoip_bt_pselect_fixed.hpp"
#include "neoip_bt_pselect_slide.hpp"
#include "neoip_bt_io_vapi.hpp"
#include "neoip_bt_io_sfile.hpp"
#include "neoip_bt_io_pfile.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration

/** \brief static helpers to manipulate swarm_helper in neoip_bt
 * 
 * - TODO why all those functions are inline ?
 * - TODO wow all those get_* and set_* are really noisy. remove them
 */
class bt_swarm_helper_t {
public:

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_pieceprec_arr_t functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	/** \brief Set a new bt_pieceprec_arr_t used by the bt_pselect_fixed_t
	 * 
	 * - irrelevant for the bt_pselect_slide_t as it has sliding cursor
	 */
	static	void	set_pselect_fixed_pieceprec_arr(bt_swarm_t *bt_swarm, const bt_pieceprec_arr_t &pieceprec_arr)	throw()
	{
		bt_pselect_fixed_t *	pselect_fixed	= get_pselect_fixed(bt_swarm);
		// set the new bt_pieceprec_arr_t;
		pselect_fixed->pieceprec_arr(pieceprec_arr);
		// do the notify_pieceprec_change() to warn the scheduler in bt_swarm_t
		// - TODO i do not like this notification
		//   - why ? because it is not automatic ? not sure
		bt_swarm->notify_pieceprec_change();
	}
	

	static void	pieceprec_arr_del_notneeded(bt_swarm_t *bt_swarm
					, const bt_pieceprec_arr_t &pieceprec_arr)	throw();
	
	/** \brief Return the bt_pieceprec_arr_t used by the bt_pselect_t
	 * 
	 * - works for bt_pselect_fixed_t AND bt_pselect_slide_t
	 * - WARNING: this function is rather slow
	 *   - it does a copy of the bt_pieceprec_arr_t
	 *   - it build the bt_pieceprec_arr_t dynamically in case of bt_pselect_slide_t
	 */
	static	bt_pieceprec_arr_t	get_pieceprec_arr(const bt_swarm_t *bt_swarm)	throw()
	{
		bt_pselect_vapi_t *	pselect_vapi	= bt_swarm->pselect_vapi();
		// return the bt_pieceprec_arr_t
		return pselect_vapi->pieceprec_arr();
	}	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_pselect_vapi_t functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	/** \brief Return true if the bt_pselect_vapi_t of bt_swarm_t is a bt_pselect_slide_t
	 */
	static bool	use_pselect_slide(const bt_swarm_t *bt_swarm)	throw()
	{
		bt_pselect_vapi_t *	pselect_vapi	= bt_swarm->pselect_vapi();
		return	dynamic_cast<bt_pselect_slide_t *>(pselect_vapi) != NULL;
	}

	/** \brief Return true if the bt_pselect_vapi_t of bt_swarm_t is a bt_pselect_fixed_t
	 */
	static bool	use_pselect_fixed(const bt_swarm_t *bt_swarm)	throw()
	{
		bt_pselect_vapi_t *	pselect_vapi	= bt_swarm->pselect_vapi();
		return	dynamic_cast<bt_pselect_fixed_t *>(pselect_vapi) != NULL;
	}

	/** \brief return the bt_pselect_slide_t used by this bt_swarm_t
	 * 
	 * - it MUST be a bt_pselect_slide_t
	 */
	static bt_pselect_slide_t *	get_pselect_slide(const bt_swarm_t *bt_swarm)	throw()
	{
		// sanity check - the bt_pselect_vapi_t MUST be a bt_pselect_slide_t
		DBG_ASSERT( use_pselect_slide(bt_swarm) );
		// return the value itself
		return	dynamic_cast<bt_pselect_slide_t *>(bt_swarm->pselect_vapi());
	}

	/** \brief return the bt_pselect_fixed_t used by this bt_swarm_t
	 * 
	 * - it MUST be a bt_pselect_fixed_t
	 */
	static bt_pselect_fixed_t *	get_pselect_fixed(const bt_swarm_t *bt_swarm)	throw()
	{
		// sanity check - the bt_pselect_vapi_t MUST be a bt_pselect_fixed_t
		DBG_ASSERT( use_pselect_fixed(bt_swarm) );
		// return the value itself
		return	dynamic_cast<bt_pselect_fixed_t *>(bt_swarm->pselect_vapi());
	}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_io_vapi_t functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	/** \brief return the bt_io_vapi_t used by this bt_swarm_t
	 */
	static bt_io_vapi_t *	get_io_vapi(const bt_swarm_t *bt_swarm)	throw()
	{
		return bt_swarm->io_vapi();
	}

	/** \brief Return true if the bt_io_vapi_t of bt_swarm_t is a bt_io_pfile_t
	 */
	static bool	use_io_pfile(const bt_swarm_t *bt_swarm)	throw()
	{
		bt_io_vapi_t *	io_vapi	= get_io_vapi(bt_swarm);
		return	dynamic_cast<bt_io_pfile_t *>(io_vapi) != NULL;
	}

	/** \brief Return true if the bt_io_vapi_t of bt_swarm_t is a bt_io_sfile_t
	 */
	static bool	use_io_sfile(const bt_swarm_t *bt_swarm)	throw()
	{
		bt_io_vapi_t *	io_vapi	= get_io_vapi(bt_swarm);
		return	dynamic_cast<bt_io_sfile_t *>(io_vapi) != NULL;
	}

	/** \brief return the bt_io_pfile_t used by this bt_swarm_t
	 * 
	 * - it MUST be a bt_io_pfile_t
	 */
	static bt_io_pfile_t *	get_io_pfile(const bt_swarm_t *bt_swarm)	throw()
	{
		// sanity check - the bt_io_vapi_t MUST be a bt_io_pfile_t
		DBG_ASSERT( use_io_pfile(bt_swarm) );
		// return the value itself
		return	dynamic_cast<bt_io_pfile_t *>(get_io_vapi(bt_swarm));
	}

	/** \brief return the bt_io_sfile_t used by this bt_swarm_t
	 * 
	 * - it MUST be a bt_io_sfile_t
	 */
	static bt_io_sfile_t *	get_io_sfile(const bt_swarm_t *bt_swarm)	throw()
	{
		// sanity check - the bt_io_vapi_t MUST be a bt_io_sfile_t
		DBG_ASSERT( use_io_sfile(bt_swarm) );
		// return the value itself
		return	dynamic_cast<bt_io_sfile_t *>(get_io_vapi(bt_swarm));
	}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_peerpick_vapi_t functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	/** \brief return the bt_peerpick_vapi_t used by this bt_swarm_t
	 */
	static bt_peerpick_vapi_t * peerpick_vapi(const bt_swarm_t *bt_swarm)	throw()
	{
		bt_swarm_sched_t *	swarm_sched	= bt_swarm->swarm_sched();
		bt_swarm_sched_full_t *	sched_full	= swarm_sched->sched_full();
		// return the bt_peerpick_vapi_t
		return sched_full->peerpick_vapi();
	}
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   query nb leecher/seeder
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	static size_t	peersrc_nb_leech(const bt_swarm_t *bt_swarm)		throw();
	static size_t	peersrc_nb_seed(const bt_swarm_t *bt_swarm)		throw();
	static size_t	swarm_nb_leech(const bt_swarm_t *bt_swarm)		throw();
	static size_t	swarm_nb_seed(const bt_swarm_t *bt_swarm)		throw();

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   query nb leecher/seeder
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	static size_t	nb_full_local_doauth_req(const bt_swarm_t *bt_swarm)	throw();
	static size_t	nb_full_local_dowant_req(const bt_swarm_t *bt_swarm)	throw();
	static size_t	nb_full_other_doauth_req(const bt_swarm_t *bt_swarm)	throw();
	static size_t	nb_full_other_dowant_req(const bt_swarm_t *bt_swarm)	throw();

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			misc
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
	static size_t	pieceidx_anyavail_len(const bt_swarm_t *bt_swarm, size_t pieceidx) throw();
	static double	recv_rate_for_reqauth_type(const bt_swarm_t *bt_swarm
					, const bt_reqauth_type_t &reqauth_type)	throw();
	static double	xmit_rate_for_reqauth_type(const bt_swarm_t *bt_swarm
					, const bt_reqauth_type_t &reqauth_type)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_HELPER_HPP__  */



