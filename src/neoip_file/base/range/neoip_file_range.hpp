/*! \file
    \brief Header of the \ref file_range_t
    
*/


#ifndef __NEOIP_FILE_RANGE_HPP__ 
#define __NEOIP_FILE_RANGE_HPP__ 
/* system include */
/* local include */
#include "neoip_file_path.hpp"
#include "neoip_file_size.hpp"
#include "neoip_file_size_inval.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief handle a range of data for a file
 * 
 * - it does NOT allow 0length range
 *   - it does allow null object tho
 * - beg() is the first byte of the range and is included in it
 * - end() is the last byte of the range and is included in it
 */
class file_range_t : NEOIP_COPY_CTOR_ALLOW {
private:
	file_size_t	m_beg;
	file_size_t	m_end;
	/*************** Internal function	*******************************/
	bool		is_sane()			const throw();
public:
	/*************** ctor/dtor	***************************************/
	file_range_t()								throw() {}
	file_range_t(const file_size_t &m_beg, const file_size_t &m_end)	throw()
				: m_beg(m_beg), m_end(m_end)	{ DBG_ASSERT(is_sane());	}
	static file_range_t	from_offlen(const file_size_t &off, const file_size_t &length)	throw()
					{ DBG_ASSERT( length > 0 );
					  return file_range_t(off, off + length - 1);	}

	/*************** query function		*******************************/
	bool		is_null()	const throw()	{ return m_beg.is_null() || m_end.is_null(); }
	const file_size_t &beg()	const throw()	{ return m_beg;			}
	const file_size_t &end()	const throw()	{ return m_end;			}
	file_size_t	len()		const throw()	{ return end() - beg() + 1;	}
	bool		is_distinct(const file_range_t &other)		const throw();
	bool		is_contiguous(const file_range_t &other)	const throw();
	bool		is_not_contiguous(const file_range_t &other)	const throw()	{ return !is_contiguous(other);	}
	bool		fully_include(const file_range_t &other)	const throw();
	bool		fully_included_in(const file_range_t &other)	const throw()
							{ return other.fully_include(*this);	}
	bool		contain(const file_size_t &other)		const throw()
							{ return beg() <= other && other <= end();	}
	bool		fully_after(const file_range_t &other)		const throw()
							{ return other.end() < beg();	}
	bool		fully_before(const file_range_t &other)		const throw()
							{ return end() < other.beg();	}
	
	/*************** Alias more readable but less aligned	***************/
	const file_size_t &first()	const throw()	{ return beg();			}
	const file_size_t &last()	const throw()	{ return end();			}
	file_size_t	length()	const throw()	{ return len();			}
	file_range_t &	first(const file_size_t &value)	throw()	{ return beg(value);	}
	file_range_t &	last(const file_size_t &value)	throw()	{ return end(value);	}
		
	/*************** file_size_inval_item_t convertion	***************/
	static file_range_t	from_inval_item(const file_size_inval_item_t &inval_item)throw()
				{ return file_range_t(inval_item.min_value(), inval_item.max_value());	}
	file_size_inval_item_t	to_inval_item()						const throw()
				{ return file_size_inval_item_t(beg(), end());	}
	
	/*************** action function	*******************************/
	file_range_t &	beg(const file_size_t &value)		throw()	{ m_beg = value; DBG_ASSERT(is_sane());
									  return *this;		}
	file_range_t &	end(const file_size_t &value)		throw()	{ m_end = value; DBG_ASSERT(is_sane());
									  return *this;		}
	file_range_t &	clamped_by(const file_range_t &other)	throw();

	/*************** arithmetic operator	*******************************/
	file_range_t &	operator +=(const file_size_t &other)	throw()	{ m_beg += other; m_end += other; return *this;	}
	file_range_t &	operator -=(const file_size_t &other)	throw()	{ m_beg -= other; m_end -= other; return *this;	}
	file_range_t	operator + (const file_size_t &other)	const throw()
						{ file_range_t tmp(*this); tmp += other; return tmp;	}
	file_range_t	operator - (const file_size_t &other)	const throw()
						{ file_range_t tmp(*this); tmp -= other; return tmp;	}	


	/*************** comparison operator	*******************************/
	int	compare(const file_range_t &other)	const throw();
	bool 	operator == (const file_range_t & other)const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const file_range_t & other)const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const file_range_t & other)const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const file_range_t & other)const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const file_range_t & other)const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const file_range_t & other)const throw()	{ return compare(other) >= 0;	}
	
	/*************** display function	*******************************/	
	std::string		to_string()		const throw();
	std::string		to_human_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const file_range_t &file_range)	throw()
				{ return os << file_range.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_RANGE_HPP__  */










