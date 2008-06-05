/*! \file
    \brief Header of the \ref bt_iov_t
    
*/


#ifndef __NEOIP_BT_IOV_HPP__ 
#define __NEOIP_BT_IOV_HPP__ 
/* system include */
/* local include */
#include "neoip_file_path.hpp"
#include "neoip_file_range.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_mfile_t;


/** \brief class definition for bt_iov_t
 * 
 * - it defines a bt_swarm_t io from a bt_mfile_t for the given bt_mfile_subfile_t
 */
class bt_iov_t : NEOIP_COPY_CTOR_ALLOW {
private:
	size_t		m_subfile_idx;		//!< index of the file in the bt_mfile_t::subfile_arr()
	file_range_t	m_subfile_range;	//!< range of data within the subfile
public:
	/*************** ctor/dtor	***************************************/
	bt_iov_t()								throw() {}
	bt_iov_t(size_t subfile_idx, const file_range_t &file_range)		throw()
				: m_subfile_idx(subfile_idx), m_subfile_range(file_range) {}
				
	bt_iov_t(size_t subfile_idx, const file_size_t &file_off, const file_size_t &file_len)	throw();

	/*************** comparison operator	*******************************/
	int	compare(const bt_iov_t &other)	const throw();
	bool 	operator == (const bt_iov_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const bt_iov_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const bt_iov_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const bt_iov_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const bt_iov_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const bt_iov_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/*************** query function		*******************************/	
	bool			is_null()	const throw()	{ return m_subfile_range.is_null();	}
	size_t			subfile_idx()	const throw()	{ return m_subfile_idx;			}
	const file_size_t &	subfile_beg()	const throw()	{ return m_subfile_range.beg();		}
	const file_size_t &	subfile_end()	const throw()	{ return m_subfile_range.end();		}
	const file_range_t &	subfile_range()	const throw()	{ return m_subfile_range;		}
	file_size_t		length()	const throw()	{ return subfile_range().length();	}
	file_size_t		totfile_beg(const bt_mfile_t &bt_mfile)		const throw();
	file_size_t		totfile_end(const bt_mfile_t &bt_mfile)		const throw();
	file_range_t		totfile_range(const bt_mfile_t &bt_mfile)	const throw();
	
	/*************** query function for interval	***********************/
	bool			is_distinct(const bt_iov_t &other)	const throw();
	bool			fully_include(const bt_iov_t &other)	const throw();
	bool			is_contiguous(const bt_iov_t &other)	const throw();

	/*************** action function	*******************************/
	bt_iov_t &		subfile_idx(size_t &value)		throw()	{ m_subfile_idx = value;
										  return *this;		}
	bt_iov_t &		subfile_beg(const file_size_t &value)	throw()	{ m_subfile_range.beg(value);
										  return *this;		}
	bt_iov_t &		subfile_end(const file_size_t &value)	throw()	{ m_subfile_range.end(value);
										  return *this;		}
	bt_iov_t &		subfile_range(const file_range_t &value)throw()	{ m_subfile_range = value;
										  return *this;		}

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const bt_iov_t &bt_iov)	throw()
				{ return os << bt_iov.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IOV_HPP__  */










