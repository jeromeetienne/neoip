/*! \file
    \brief Header of the \ref bt_pselect_slide_curs_t
    
*/


#ifndef __NEOIP_BT_PSELECT_SLIDE_CURS_HPP__ 
#define __NEOIP_BT_PSELECT_SLIDE_CURS_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_pselect_slide_curs_wikidbg.hpp"
#include "neoip_bt_pselect_slide_curs_arg.hpp"
#include "neoip_bt_pieceprec_arr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_pselect_slide_t;

/** \brief class definition for bt_pselect_slide_curs_t
 */
class bt_pselect_slide_curs_t : NEOIP_COPY_CTOR_ALLOW
			, private wikidbg_obj_t<bt_pselect_slide_curs_t, bt_pselect_slide_curs_wikidbg_init> {
private:
	bt_pselect_slide_t *	m_pselect_slide;	//!< backpointer on the attached bt_pselect_slide_t
	bt_pieceprec_arr_t	m_pieceprec_arr;
	size_t			m_offset;
	bool			m_has_circularidx;
public:
	/*************** ctor/dtor	***************************************/
	bt_pselect_slide_curs_t(bt_pselect_slide_t *pselect_slide
				, const bt_pselect_slide_curs_arg_t &arg)	throw();
	~bt_pselect_slide_curs_t()								throw();

	/*************** query function	***************************************/
	bool			is_null()		const throw()	{ return pieceprec_arr().is_null();	}
	size_t			nb_piece()		const throw()	{ return pieceprec_arr().size();}
	size_t			offset()		const throw()	{ return m_offset;		}
	const bt_pieceprec_arr_t&pieceprec_arr()	const throw()	{ return m_pieceprec_arr;	}
	bool			has_circularidx()	const throw()	{ return m_has_circularidx;	}
	bt_pselect_slide_t *	pselect_slide()		const throw()	{ return m_pselect_slide;	}
	size_t			cursidx_to_pieceidx(size_t curs_idx)	const throw();
	bt_pieceprec_t		pieceprec(size_t pieceidx)		const throw();

	/*************** Action function	*******************************/
	void			offset(size_t new_offset)	throw();

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const bt_pselect_slide_curs_t &bt_pselect)	throw()
				{ return os << bt_pselect.to_string();	}


	/*************** List of friend class	*******************************/
	friend class	bt_pselect_slide_curs_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PSELECT_SLIDE_CURS_HPP__  */










