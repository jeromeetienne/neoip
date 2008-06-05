/*! \file
    \brief Header of the \ref bt_prange_t
    
*/


#ifndef __NEOIP_BT_PRANGE_HPP__ 
#define __NEOIP_BT_PRANGE_HPP__ 
/* system include */
/* local include */
#include "neoip_file_path.hpp"
#include "neoip_file_size.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_mfile_t;
class	file_range_t;

/** \brief handle a range of data for a file
 * 
 * - it is handled via a 'half-open' range aka [begin,end[
 *   - end is always excluded from the range
 *   - it allow to avoid a special case for the 0byte long range
 */
class bt_prange_t : NEOIP_COPY_CTOR_ALLOW {
private:
	size_t		m_piece_idx;
	size_t		m_offset;
	size_t		m_length;
public:
	/*************** ctor/dtor	***************************************/
	bt_prange_t()		throw() : m_offset(std::numeric_limits<size_t>::max()) {}
	bt_prange_t(size_t m_piece_idx, size_t m_offset, size_t m_length)	throw()
			: m_piece_idx(m_piece_idx), m_offset(m_offset), m_length(m_length) {}
	
	/*************** query function		*******************************/	
	bool		is_null()		const throw()	{ return m_offset == std::numeric_limits<size_t>::max();	}
	size_t		piece_idx()		const throw()	{ return m_piece_idx;		}
	size_t		offset()		const throw()	{ return m_offset;		}
	size_t		length()		const throw()	{ return m_length;		}
	file_size_t	totfile_beg(const bt_mfile_t &bt_mfile)	const throw();
	file_size_t	totfile_end(const bt_mfile_t &bt_mfile)	const throw();

	/*************** action function	*******************************/
	bt_prange_t &	piece_idx(size_t value)	throw()		{ m_piece_idx	= value; return *this;	}
	bt_prange_t &	offset(size_t value)	throw()		{ m_offset	= value; return *this;	}
	bt_prange_t &	length(size_t value)	throw()		{ m_length	= value; return *this;	}

	/*************** convertion operator	*******************************/
	file_range_t		to_totfile_range(const bt_mfile_t &bt_mfile)	const throw();
	static bt_prange_t	from_totfile_range(const file_range_t &totfile_range
						, const bt_mfile_t &bt_mfile)	throw();

	/*************** comparison operator	*******************************/
	int	compare(const bt_prange_t &other)	const throw();
	bool 	operator == (const bt_prange_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const bt_prange_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const bt_prange_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const bt_prange_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const bt_prange_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const bt_prange_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const bt_prange_t &bt_prange) throw()
				{ return os << bt_prange.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PRANGE_HPP__  */










