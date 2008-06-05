/*! \file
    \brief Header of the \ref bt_pieceavail_t
    
*/


#ifndef __NEOIP_BT_PIECEAVAIL_HPP__ 
#define __NEOIP_BT_PIECEAVAIL_HPP__ 
/* system include */
/* local include */
#include "neoip_bitfield.hpp"
#include "neoip_file_size.hpp"
#include "neoip_math_shortcut.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_mfile_t;
class	pkt_t;

/** \brief class definition for bt_pieceavail_t
 */
class	bt_pieceavail_t : public bitfield_t  {
public:	
	/*************** ctor/dtor	***************************************/
	bt_pieceavail_t(const size_t &nb_piece = 0)		throw() : bitfield_t(nb_piece)	{}
	explicit bt_pieceavail_t(const bitfield_t &bitfield)	throw() : bitfield_t(bitfield)	{}

	/*************** query function	***************************************/
	size_t		nb_piece()		const throw()	{ return size();		}
	bool		is_avail(size_t idx)	const throw()	{ return get(idx);		}
	bool		is_unavail(size_t idx)	const throw()	{ return !is_avail(idx);	}
	bool		is_fully_avail()	const throw()	{ return is_all_set();		}

	/*************** Action function	*******************************/
	void		mark_isavail(size_t idx)	throw()	{ set(idx);	}
	void		mark_unavail(size_t idx)	throw()	{ clear(idx);	}

	/*************** btformat serialization	*******************************/
	// TODO those function belong to bitfield_t 
	// - there is already a halfbacked attempt in bifield_rawserial_t
	size_t			nb_byte_needed()	const throw()	{ return ceil_div(size(), size_t(8));	}
	pkt_t &			btformat_to_pkt(pkt_t &pkt)			const throw();
	static bt_pieceavail_t	btformat_from_pkt(pkt_t &pkt, size_t nb_piece)	throw(serial_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PIECEAVAIL_HPP__  */










