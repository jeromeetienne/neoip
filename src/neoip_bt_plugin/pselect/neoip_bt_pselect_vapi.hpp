/*! \file
    \brief Declaration of the bt_pselect_vapi_t

- TODO what about doing this bt_swarm_t::notify_pieceprec_change automatically in 
  the bt_pselect_vapi_t
  - this would implies a link between bt_pselect_vapi_t and the bt_swarm
  - this would avoid to manage the call in the upper code
  - i just spent quite a lot of time finding out that i didnt call 
    bt_swarm_t::notify_pieceprec_change when deleting a slide
  - this create race condition
    
  
*/


#ifndef __NEOIP_BT_PSELECT_VAPI_HPP__ 
#define __NEOIP_BT_PSELECT_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_bt_pieceprec_arr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_pieceavail_t;
class	bitfield_t;
class	bitcount_t;

/** \brief Virtual API for the bt_pselect_fixed_t and bt_pselect_slide_t
 */
class bt_pselect_vapi_t {
public:
	/*************** piece selection itself	*******************************/
	virtual	size_t	next_piece_to_select(const bt_pieceavail_t &remote_pieceavail
					, size_t pieceidx_hint)	const throw() = 0;
	virtual bool	may_select(size_t pieceidx)	const throw() = 0;
	virtual bool	has_selectable()		const throw() = 0;

	/*************** query bt_pieceprec_arr_t	***********************/
	virtual bt_pieceprec_arr_t	pieceprec_arr()			const throw() = 0;
	virtual bt_pieceprec_t		pieceprec(size_t piece_idx)	const throw() = 0;

	/*************** maintain the remote_pfreq	***********************/
	virtual void	inc_remote_pfreq(size_t pieceidx)		throw()	= 0; 
	virtual void	dec_remote_pfreq(size_t pieceidx)		throw()	= 0;
	virtual void	inc_remote_pfreq(const bitfield_t &bitfield)	throw()	= 0; 
	virtual void	dec_remote_pfreq(const bitfield_t &bitfield)	throw()	= 0;
	virtual size_t	remote_pfreq(size_t pieceidx)			const throw() = 0; 
	virtual const bitcount_t &remote_pfreq()			const throw() = 0;

	/*************** maintain the remote_pwish	***********************/
	virtual void	inc_remote_pwish(size_t pieceidx)		throw()	= 0; 
	virtual void	dec_remote_pwish(size_t pieceidx)		throw()	= 0;
	virtual void	inc_remote_pwish(const bitfield_t &bitfield)	throw()	= 0; 
	virtual void	dec_remote_pwish(const bitfield_t &bitfield)	throw()	= 0;
	virtual size_t	remote_pwish(size_t pieceidx)			const throw() = 0;
	virtual const bitcount_t &remote_pwish()			const throw() = 0;
	
	/*************** Piece which are currently downloading	***************/
	virtual void	enter_dling(size_t pieceidx)			throw() = 0;
	virtual void	leave_dling(size_t pieceidx)			throw() = 0;
	virtual bool	is_dling(size_t pieceidx)			const throw() = 0;
	virtual const bitfield_t &local_pdling()			const throw() = 0;

	/*************** Piece locally available	***********************/
	virtual void	mark_isavail(size_t pieceidx)			throw()	= 0;
	virtual void	mark_isavail(const bt_pieceavail_t &pieceavail)	throw()	= 0;
	virtual void	mark_unavail(size_t pieceidx)			throw()	= 0;
	virtual bool	isavail(size_t pieceidx)			const throw() = 0;
	virtual bool	unavail(size_t pieceidx)			const throw() = 0;
	virtual const bt_pieceavail_t &local_pavail()			const throw() = 0;

	/*************** display function	*******************************/
	virtual std::string	to_string()					const throw() = 0;
	friend	std::ostream &	operator << (std::ostream & os, const bt_pselect_vapi_t &pselect_vapi)	throw()
				{ return os << pselect_vapi.to_string();	}
	//! virtual destructor
	virtual ~bt_pselect_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PSELECT_VAPI_HPP__  */



