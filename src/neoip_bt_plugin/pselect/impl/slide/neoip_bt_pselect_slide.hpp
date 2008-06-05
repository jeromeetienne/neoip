/*! \file
    \brief Header of the \ref bt_pselect_slide_t
    
*/


#ifndef __NEOIP_BT_PSELECT_SLIDE_HPP__ 
#define __NEOIP_BT_PSELECT_SLIDE_HPP__ 
/* system include */
#include <list>
#include <vector>
#include <map>
/* local include */
#include "neoip_bt_pselect_slide_wikidbg.hpp"
#include "neoip_bt_pselect_vapi.hpp"
#include "neoip_bt_pieceavail.hpp"
#include "neoip_bitcount.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_pselect_slide_curs_t;

/** \brief class definition for bt_pselect_slide_t
 */
class bt_pselect_slide_t : NEOIP_COPY_CTOR_ALLOW, public bt_pselect_vapi_t
		, private wikidbg_obj_t<bt_pselect_slide_t, bt_pselect_slide_wikidbg_init> {
public:
	typedef std::pair<bt_pselect_slide_curs_t*, size_t> piece_curs_t; //!< the buff_curs and the piece_idx
								//!< within the curs (aka without the offset)
	typedef	std::vector<piece_curs_t>	piece_db_t;	//!< set of piece_idx, curs pointer
	typedef	std::map<size_t, piece_db_t>	prec_db_t;	//!< piece_db_t indexed by their precedence
private:
	bitcount_t	m_remote_pfreq;	//!< the bitcount_t for the remote piece frequency
	bitcount_t	m_remote_pwish;	//!< the bitcount_t for the remote piece wish
	bitfield_t	m_local_pdling;	//!< keep track of the currently downloading piece
	bt_pieceavail_t	m_local_pavail;	//!< keep track of the local piece fully available

	/*************** bt_pselect_slide_curs_t	***********************/
	std::list<bt_pselect_slide_curs_t *>	curs_db;
	void	curs_dolink(bt_pselect_slide_curs_t *curs)	throw();
	void	curs_unlink(bt_pselect_slide_curs_t *curs)	throw();

	/*************** precomputed unavail_db_t	***********************/
	mutable prec_db_t			prec_db;
	/*************** internal function	*******************************/	
	void	generate_prec_db()		throw();
	size_t	next_piece_to_select_nohint(const bt_pieceavail_t &remote_pieceavail)	const throw();	
public:
	/*************** ctor/dtor	***************************************/
	bt_pselect_slide_t(const size_t &nb_piece = 0)		throw();

	/*************** query function	***************************************/
	bool	is_null()		const throw()	{ return m_local_pavail.is_null();	}
	size_t	nb_piece()		const throw()	{ return m_local_pavail.size();	}
	size_t	size()			const throw()	{ return nb_piece();		}

	/*************** piece selection itself	*******************************/
	size_t	next_piece_to_select(const bt_pieceavail_t &remote_pieceavail
						, size_t pieceidx_hint)	const throw();
	bool	cant_select(size_t pieceidx)	const throw()	{ return m_local_pdling[pieceidx] || m_local_pavail[pieceidx];	}	
	bool	may_select(size_t pieceidx)	const throw()	{ return !cant_select(pieceidx);}
	bool	has_selectable()		const throw();

	/*************** query bt_pieceprec_arr_t	***********************/
	bt_pieceprec_arr_t	pieceprec_arr()				const throw();
	bt_pieceprec_t		pieceprec(size_t piece_idx)		const throw();

	/*************** maintain the remote_pfreq	***********************/
	void	inc_remote_pfreq(size_t pieceidx)		throw()	{ m_remote_pfreq.inc(pieceidx);	} 
	void	dec_remote_pfreq(size_t pieceidx)		throw()	{ m_remote_pfreq.dec(pieceidx);	}
	void	inc_remote_pfreq(const bitfield_t &bitfield)	throw()	{ m_remote_pfreq.inc(bitfield);	} 
	void	dec_remote_pfreq(const bitfield_t &bitfield)	throw()	{ m_remote_pfreq.dec(bitfield);	}
	size_t	remote_pfreq(size_t pieceidx)		const throw()	{ return m_remote_pfreq[pieceidx];	} 
	const bitcount_t &remote_pfreq()		const throw()	{ return m_remote_pfreq;		} 

	/*************** maintain the remote_pwish	***********************/
	void	inc_remote_pwish(size_t pieceidx)		throw()	{ m_remote_pwish.inc(pieceidx);	} 
	void	dec_remote_pwish(size_t pieceidx)		throw()	{ m_remote_pwish.dec(pieceidx);	}
	void	inc_remote_pwish(const bitfield_t &bitfield)	throw()	{ m_remote_pwish.inc(bitfield);	} 
	void	dec_remote_pwish(const bitfield_t &bitfield)	throw()	{ m_remote_pwish.dec(bitfield);	}
	size_t	remote_pwish(size_t pieceidx)		const throw()	{ return m_remote_pwish[pieceidx];	} 
	const bitcount_t &remote_pwish()		const throw()	{ return m_remote_pwish;		} 

	/*************** maintain the rarity of piece_idx	***************/
	void	enter_dling(size_t pieceidx)	throw()		{ m_local_pdling.set(pieceidx);		}
	void	leave_dling(size_t pieceidx)	throw()		{ m_local_pdling.clear(pieceidx);	}
	bool	is_dling(size_t pieceidx)	const throw()	{ return m_local_pdling[pieceidx];	}
	const bitfield_t &local_pdling()	const throw()	{ return m_local_pdling;		}
	
	/*************** Piece locally available	***********************/
	void	mark_isavail(size_t pieceidx)	throw()	{ m_local_pavail.set(pieceidx);		}
	void	mark_isavail(const bt_pieceavail_t &ext_pavail)
						throw()	{ m_local_pavail = ext_pavail;		}
	void	mark_unavail(size_t pieceidx)	throw()	{ m_local_pavail.clear(pieceidx);	}
	bool	isavail(size_t pieceidx)	const throw()	{ return local_pavail()[pieceidx];}
	bool	unavail(size_t pieceidx)	const throw()	{ return !isavail(pieceidx);	}
	const bt_pieceavail_t &local_pavail()	const throw()	{ return m_local_pavail;	} 

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const bt_pselect_slide_t &bt_pselect)	throw()
				{ return os << bt_pselect.to_string();	}

	/*************** List of friend class	*******************************/
	friend class	bt_pselect_slide_wikidbg_t;
	friend class	bt_pselect_slide_curs_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PSELECT_SLIDE_HPP__  */










