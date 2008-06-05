/*! \file
    \brief Header of the \ref bt_pselect_fixed_t
    
*/


#ifndef __NEOIP_BT_PSELECT_FIXED_HPP__ 
#define __NEOIP_BT_PSELECT_FIXED_HPP__ 
/* system include */
#include <set>
#include <map>
/* local include */
#include "neoip_bt_pselect_fixed_wikidbg.hpp"
#include "neoip_bt_pselect_vapi.hpp"
#include "neoip_bt_pieceavail.hpp"
#include "neoip_bitcount.hpp"
#include "neoip_bitfield.hpp"
#include "neoip_bt_pieceprec_arr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for bt_pselect_fixed_t
 */
class bt_pselect_fixed_t : NEOIP_COPY_CTOR_ALLOW, public bt_pselect_vapi_t
		, private wikidbg_obj_t<bt_pselect_fixed_t, bt_pselect_fixed_wikidbg_init> {
private:
	bitcount_t	m_remote_pfreq;	//!< the bitcount_t for the remote piece frequency
	bitcount_t	m_remote_pwish;	//!< the bitcount_t for the remote piece wish
	bitfield_t	m_local_pdling;	//!< keep track of the currently downloading piece
	bt_pieceavail_t	m_local_pavail;	//!< keep track of the local piece fully available


	bt_pieceprec_arr_t	m_pieceprec_arr;//!< store the precedence of each piece_idx

	/*************** update selectable_db and bitfield's	***************/
	void	upd_selectable(bitfield_t &bitfield, size_t piece_idx, bool value)	throw();
	void	upd_selectable(bitfield_t &intfield, const bitfield_t &extfield
									, bool value)	throw();
	void	upd_selectable(bitcount_t &bitcount, size_t piece_idx, bool value)	throw();
	void	upd_selectable(bitcount_t &bitcount, const bitfield_t &extfield
									, bool value)	throw();

	/*************** precomputed selectable_db_t	***********************/
	typedef	std::set<size_t>		piece_db_t;	//!< set of piece_idx
	typedef	std::map<size_t, piece_db_t>	goub_db_t;	//!< piece_db_t indexed by their rarity
	typedef	std::map<size_t, goub_db_t>	selectable_db_t;//!< goub_db_t indexed by their precedence
	selectable_db_t				selectable_db;

#if 0	// TODO to remove - replace goub by wish - once pwish is known to work
	size_t	remote_pgoub(size_t pieceidx)	const throw()	{ return remote_pfreq(pieceidx);	}
#else
	size_t	remote_pgoub(size_t pieceidx)	const throw()	{ return remote_pwish(pieceidx);	}
#endif
	/*************** internal function	*******************************/
	void	selectable_db_insert(size_t piece_idx)					throw();
	void	selectable_db_delete(size_t piece_idx)					throw();
	size_t	next_piece_to_select_nohint(const bt_pieceavail_t &remote_pieceavail)	const throw();	
public:
	/*************** ctor/dtor	***************************************/
	bt_pselect_fixed_t(const size_t &nb_piece = 0)		throw();

	/*************** query function	***************************************/
	bool	is_null()		const throw()	{ return m_local_pavail.is_null();	}
	size_t	nb_piece()		const throw()	{ return m_local_pavail.size();	}
	size_t	size()			const throw()	{ return nb_piece();		}
		
	/*************** bt_pselect_vapi_t	*******************************/
	size_t	next_piece_to_select(const bt_pieceavail_t &remote_pieceavail
						, size_t pieceidx_hint)	const throw();
	// TODO why does cant_select do not take pieceprec not needed into account ?
	bool	cant_select(size_t pieceidx)	const throw()	{ return m_local_pdling[pieceidx] || m_local_pavail[pieceidx];	}	
	bool	may_select(size_t pieceidx)	const throw()	{ return !cant_select(pieceidx);}
	bool	has_selectable()		const throw()	{ return !selectable_db.empty();}

	/*************** query bt_pieceprec_arr_t	***********************/
	bt_pieceprec_arr_t	pieceprec_arr()		const throw()	{ return m_pieceprec_arr;	}
	bt_pieceprec_t		pieceprec(size_t idx)	const throw()	{ return m_pieceprec_arr[idx];	}
	/*************** Action function	*******************************/
	void	pieceprec_arr(const bt_pieceprec_arr_t &pieceprec_arr)	throw();

#if 0
	/*************** maintain the remote_pfreq	***********************/
	void	inc_remote_pfreq(size_t pieceidx)		throw()	{ upd_selectable(m_remote_pfreq, pieceidx, true);	} 
	void	dec_remote_pfreq(size_t pieceidx)		throw()	{ upd_selectable(m_remote_pfreq, pieceidx, false);	} 
	void	inc_remote_pfreq(const bitfield_t &bitfield)	throw()	{ upd_selectable(m_remote_pfreq, bitfield, true);	} 
	void	dec_remote_pfreq(const bitfield_t &bitfield)	throw()	{ upd_selectable(m_remote_pfreq, bitfield, false);	}
	size_t	remote_pfreq(size_t pieceidx)		const throw()	{ return m_remote_pfreq[pieceidx];			} 
	const bitcount_t &remote_pfreq()		const throw()	{ return m_remote_pfreq;				} 

	/*************** maintain the remote_pwish	***********************/
	void	inc_remote_pwish(size_t pieceidx)		throw()	{ m_remote_pwish.inc(pieceidx);	} 
	void	dec_remote_pwish(size_t pieceidx)		throw()	{ m_remote_pwish.dec(pieceidx);	}
	void	inc_remote_pwish(const bitfield_t &bitfield)	throw()	{ m_remote_pwish.inc(bitfield);	} 
	void	dec_remote_pwish(const bitfield_t &bitfield)	throw()	{ m_remote_pwish.dec(bitfield);	}
	size_t	remote_pwish(size_t pieceidx)		const throw()	{ return m_remote_pwish[pieceidx];	} 
	const bitcount_t &remote_pwish()		const throw()	{ return m_remote_pwish;		} 
#else
	/*************** maintain the remote_pfreq	***********************/
	void	inc_remote_pfreq(size_t pieceidx)		throw()	{ m_remote_pfreq.inc(pieceidx);	} 
	void	dec_remote_pfreq(size_t pieceidx)		throw()	{ m_remote_pfreq.dec(pieceidx);	}
	void	inc_remote_pfreq(const bitfield_t &bitfield)	throw()	{ m_remote_pfreq.inc(bitfield);	} 
	void	dec_remote_pfreq(const bitfield_t &bitfield)	throw()	{ m_remote_pfreq.dec(bitfield);	}
	size_t	remote_pfreq(size_t pieceidx)		const throw()	{ return m_remote_pfreq[pieceidx];	} 
	const bitcount_t &remote_pfreq()		const throw()	{ return m_remote_pfreq;		} 

	/*************** maintain the remote_pwish	***********************/
	void	inc_remote_pwish(size_t pieceidx)		throw()	{ upd_selectable(m_remote_pwish, pieceidx, true);	} 
	void	dec_remote_pwish(size_t pieceidx)		throw()	{ upd_selectable(m_remote_pwish, pieceidx, false);	} 
	void	inc_remote_pwish(const bitfield_t &bitfield)	throw()	{ upd_selectable(m_remote_pwish, bitfield, true);	} 
	void	dec_remote_pwish(const bitfield_t &bitfield)	throw()	{ upd_selectable(m_remote_pwish, bitfield, false);	}
	size_t	remote_pwish(size_t pieceidx)		const throw()	{ return m_remote_pwish[pieceidx];			} 
	const bitcount_t &remote_pwish()		const throw()	{ return m_remote_pwish;				} 
#endif
	/*************** Piece which are currently downloading	***************/
	void	enter_dling(size_t pieceidx)	throw()		{ upd_selectable(m_local_pdling, pieceidx, true);	}
	void	leave_dling(size_t pieceidx)	throw()		{ upd_selectable(m_local_pdling, pieceidx, false);	}
	bool	is_dling(size_t pieceidx)	const throw()	{ return m_local_pdling[pieceidx];	}
	const bitfield_t &local_pdling()	const throw()	{ return m_local_pdling;				} 

	/*************** Piece locally available	***********************/
	void	mark_isavail(size_t pieceidx)	throw()	{ upd_selectable(m_local_pavail, pieceidx, true);	}
	void	mark_unavail(size_t pieceidx)	throw()	{ upd_selectable(m_local_pavail, pieceidx, false);	}
	void	mark_isavail(const bt_pieceavail_t &ext_pavail)
						throw()	{ upd_selectable(m_local_pavail, ext_pavail, true);	}
	bool	isavail(size_t pieceidx)	const throw()	{ return m_local_pavail[pieceidx];	}
	bool	unavail(size_t pieceidx)	const throw()	{ return !isavail(pieceidx);	}
	const bt_pieceavail_t &local_pavail()	const throw()	{ return m_local_pavail;	} 

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const bt_pselect_fixed_t &bt_pselect)	throw()
				{ return os << bt_pselect.to_string();	}
				
	/*************** List of friend class	*******************************/
	friend class	bt_pselect_fixed_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PSELECT_FIXED_HPP__  */










