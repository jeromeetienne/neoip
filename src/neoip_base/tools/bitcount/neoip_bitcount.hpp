/*! \file
    \brief Header of the \ref bitcount_t
    
*/


#ifndef __NEOIP_BITCOUNT_HPP__ 
#define __NEOIP_BITCOUNT_HPP__ 
/* system include */
#include <vector>
#include <string>
/* local include */
#include "neoip_bitcount_wikidbg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bitfield_t;

/** \brief class definition for bitcount_t
 */
class bitcount_t : NEOIP_COPY_CTOR_ALLOW
		, private wikidbg_obj_t<bitcount_t, bitcount_wikidbg_init> {
private:
	std::vector<size_t>	count_arr;	//!< array with key = idx and value = counter
public:
	/*************** ctor/dtor	***************************************/
	bitcount_t(const size_t &nb_piece = 0)		throw();
 
	/*************** query function		*******************************/
	bool		is_null()		const throw()	{ return count_arr.empty();	}
	size_t		size()			const throw()	{ return count_arr.size();	}
	size_t		nb_bit()		const throw()	{ return size();		}
	size_t		operator[](size_t idx)	const throw()	{ DBG_ASSERT( !is_null() );
								  DBG_ASSERT(idx < count_arr.size());
								  return count_arr[idx];	}
	double		coverage_factor()	const throw();

	/*************** action function		***********************/
	bitcount_t &	inc(size_t idx)			throw();
	bitcount_t &	dec(size_t idx)			throw();
	bitcount_t &	inc(const bitfield_t &bitfield)	throw();
	bitcount_t &	dec(const bitfield_t &bitfield)	throw();

	/*************** arithmetic operator	*******************************/
	bitcount_t &	operator +=(const bitfield_t &other)	throw()	{ return inc(other);	}
	bitcount_t &	operator -=(const bitfield_t &other)	throw()	{ return dec(other);	}
	bitcount_t	operator + (const bitfield_t &other)	const throw()
						{ bitcount_t tmp(*this); tmp += other; return tmp;	}
	bitcount_t	operator - (const bitfield_t &other)	const throw()
						{ bitcount_t tmp(*this); tmp -= other; return tmp;	}
	bitcount_t &	operator +=(const bitcount_t &other)	throw();
	bitcount_t &	operator -=(const bitcount_t &other)	throw();
	bitcount_t	operator + (const bitcount_t &other)	const throw()
						{ bitcount_t tmp(*this); tmp += other; return tmp;	}
	bitcount_t	operator - (const bitcount_t &other)	const throw()
						{ bitcount_t tmp(*this); tmp -= other; return tmp;	}	


	/*************** Comparison function	*******************************/
	int	compare(const bitcount_t &other)	  const throw();
	bool 	operator == (const bitcount_t & other) const throw() { return compare(other) == 0;	}
	bool 	operator != (const bitcount_t & other) const throw() { return compare(other) != 0;	}
	bool 	operator <  (const bitcount_t & other) const throw() { return compare(other) <  0;	}
	bool 	operator <= (const bitcount_t & other) const throw() { return compare(other) <= 0;	}
	bool 	operator >  (const bitcount_t & other) const throw() { return compare(other) >  0;	}
	bool 	operator >= (const bitcount_t & other) const throw() { return compare(other) >= 0;	}

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const bitcount_t &bitcount) throw()
				{ return os << bitcount.to_string();	}

	/*************** List of friend class	*******************************/
	friend class	bitcount_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BITCOUNT_HPP__  */










