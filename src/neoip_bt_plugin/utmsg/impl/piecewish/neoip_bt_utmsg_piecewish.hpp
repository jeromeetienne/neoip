/*! \file
    \brief Header of the bt_utmsg_piecewish_t

*/


#ifndef __NEOIP_BT_UTMSG_PIECEWISH_HPP__ 
#define __NEOIP_BT_UTMSG_PIECEWISH_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_utmsg_piecewish_wikidbg.hpp"
#include "neoip_bt_utmsg_vapi.hpp"
#include "neoip_bt_utmsg_cb.hpp"
#include "neoip_bt_utmsgtype.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bitfield.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_cmd_t;
class	bt_utmsg_piecewish_cnx_t;

/** \brief class definition for bt_peersrc for http
 * 
 * - it MUST be ctor after bt_swarm_t and dtor before
 */
class bt_utmsg_piecewish_t : NEOIP_COPY_CTOR_DENY, public bt_utmsg_vapi_t
			, private wikidbg_obj_t<bt_utmsg_piecewish_t, bt_utmsg_piecewish_wikidbg_init, bt_utmsg_vapi_t>
			{
private:
	bt_swarm_t *	bt_swarm;	//!< backpointer to the next
	bitfield_t	m_local_pwish;	//!< the local piecewish

	/*************** internal function	*******************************/
	bitfield_t	generate_local_pwish()		throw();
	
	/*************** bt_cmd_t building	*******************************/
	static bt_cmd_t build_cmd_dowish_field(const bitfield_t &wishfield)	throw();
	static bt_cmd_t	build_cmd_delta_bitfield(const bitfield_t &old_piecewish
					, const bitfield_t &new_piecewish)	throw();
	static bt_cmd_t	build_cmd_pieceidx(const std::string &cmx_str, size_t pieceidx)	throw();
	static bt_cmd_t	build_cmd_dowish_pieceidx(size_t pieceidx)		throw();
	static bt_cmd_t	build_cmd_nowish_pieceidx(size_t pieceidx)		throw();

	/*************** bt_utmsg_piecewish_cnx_t	*******************************/
	std::list<bt_utmsg_piecewish_cnx_t *>	cnx_db;
	void cnx_dolink(bt_utmsg_piecewish_cnx_t *cnx) 	throw()	{ cnx_db.push_back(cnx);	}
	void cnx_unlink(bt_utmsg_piecewish_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}
	void		send_cmd_to_all_cnx(const bt_cmd_t &bt_cmd)	throw();
	
	/*************** callback stuff	***************************************/
	bt_utmsg_cb_t *	utmsg_cb;	//!< callback used to notify peersrc result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_utmsg_cb(const bt_utmsg_event_t &event)			throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_utmsg_piecewish_t() 		throw();
	~bt_utmsg_piecewish_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t		start(bt_swarm_t *bt_swarm, bt_utmsg_cb_t *utmsg_cb
							, void *userptr)	throw();

	/*************** Query function	***************************************/
	bt_swarm_t *		get_swarm()	const throw()	{ return bt_swarm;	}
	const bitfield_t &	local_pwish()	const throw()	{ return m_local_pwish;	}
	size_t			nb_piece()	const throw()	{ return local_pwish().size();}
	bool			local_pwish(size_t pieceidx) const throw()	{ return local_pwish()[pieceidx];	}

	/*************** Action function	*******************************/
	void			notify_pieceprec_change()			throw();
	void			declare_piece_newly_avail(size_t piece_idx)	throw();
	void			declare_piece_nomore_avail(size_t piece_idx)	throw();
	
	/*************** bt_utmsg_vapi_t	*******************************/
	bt_utmsgtype_t	utmsgtype()		const throw()	{ return bt_utmsgtype_t::PIECEWISH;	}
	std::string	utmsgstr()		const throw();
	bt_utmsg_cnx_vapi_t *	cnx_ctor(bt_swarm_full_utmsg_t *full_utmsg)		throw();

	/*************** List of friend class	*******************************/
	friend class	bt_utmsg_piecewish_wikidbg_t;
	friend class	bt_utmsg_piecewish_cnx_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_PIECEWISH_HPP__  */



