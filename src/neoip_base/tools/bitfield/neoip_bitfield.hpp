/*! \file
    \brief Header of the \ref bitfield_t
    
*/


#ifndef __NEOIP_BITFIELD_HPP__ 
#define __NEOIP_BITFIELD_HPP__ 
/* system include */
#include <vector>
/* local include */
#include "neoip_bitfield_wikidbg.hpp"
#include "neoip_serial.hpp"
#include "neoip_math_shortcut.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for bitfield_t
 */
class bitfield_t : NEOIP_COPY_CTOR_ALLOW
		, private wikidbg_obj_t<bitfield_t, bitfield_wikidbg_init> {
private:
	std::vector<bool>	bitfield_arr;	//!< the bitfield with a bool for each piece_idx
public:
	/*************** ctor/dtor	***************************************/
	bitfield_t(size_t nb_bit = 0)		throw();
	
	
	/*************** query function		*******************************/
	bool		is_null()		const throw()	{ return bitfield_arr.empty();	}
	size_t		size()			const throw()	{ return bitfield_arr.size();	}
	size_t		nb_bit()		const throw()	{ return size();		}
	bool		get(size_t idx)		const throw()	{ DBG_ASSERT( !is_null() );
								  DBG_ASSERT( idx < size() );
								  return bitfield_arr[idx];	}
	bitfield_t &	set(size_t idx, bool new_val=true)throw(){DBG_ASSERT( !is_null() );
								  DBG_ASSERT( idx < size() );
								  bitfield_arr[idx] = new_val;
								  return *this;			}
	bitfield_t &	clear(size_t idx)	throw()		{ return set(idx, false);	}
	bitfield_t &	assign(size_t nb_bit, bool value)	throw();

	/*************** utility function	*******************************/
	double		coverage_factor()	const throw()	{ return (double)sum()/size();	}
	bool		is_all_set()		const throw();
	bool		is_any_set()		const throw();
	bool		is_none_set()		const throw()	{ return !is_any_set();	}
	size_t		nb_set()		const throw();
	size_t		sum()			const throw()	{ return nb_set();	}
	bool		fully_contain(const bitfield_t &other)	const throw();
	size_t		next_set(size_t start_idx)		const throw();
	size_t		next_unset(size_t start_idx)		const throw();

	/*************** compatibility function	*******************************/
	size_t		rawformat_nbbyte()			const throw()	{ return to_datum_size();	}
	size_t		get_next_set(size_t start_idx)		const throw()	{ return next_set(start_idx);	}
	size_t		get_next_unset(size_t start_idx)	const throw()	{ return next_unset(start_idx);	}

	/*************** datum_t conversion	*******************************/
	size_t			to_datum_size()	const throw()	{ return ceil_div(size(), size_t(8));	}
	datum_t			to_datum()					const throw();
	static bitfield_t	from_datum(const datum_t &datum, size_t nb_bit)	throw();

	/*************** arithmetic operator	*******************************/
	bitfield_t	operator ~  ()				const throw();
	bitfield_t &	operator |= (const bitfield_t &other)	throw();
	bitfield_t &	operator &= (const bitfield_t &other)	throw();
	bitfield_t &	operator ^= (const bitfield_t &other)	throw();
	bitfield_t	operator &  (const bitfield_t &other)	const throw()
						{ bitfield_t tmp(*this); tmp &= other; return tmp;	}
	bitfield_t	operator |  (const bitfield_t &other)	const throw()
						{ bitfield_t tmp(*this); tmp |= other; return tmp;	}
	bitfield_t	operator ^  (const bitfield_t &other)	const throw()
						{ bitfield_t tmp(*this); tmp ^= other; return tmp;	}

	/*************** Alias helper	***************************************/
	bool		test(size_t idx)	const throw()	{ return get(idx);	}
	bool		operator[](size_t idx)	const throw()	{ return get(idx);	}

	/*************** Comparison function	*******************************/
	int	compare(const bitfield_t &other)	const throw();
	bool 	operator == (const bitfield_t & other) 	const throw() { return compare(other) == 0;	}
	bool 	operator != (const bitfield_t & other) 	const throw() { return compare(other) != 0;	}
	bool 	operator <  (const bitfield_t & other) 	const throw() { return compare(other) <  0;	}
	bool 	operator <= (const bitfield_t & other) 	const throw() { return compare(other) <= 0;	}
	bool 	operator >  (const bitfield_t & other) 	const throw() { return compare(other) >  0;	}
	bool 	operator >= (const bitfield_t & other) 	const throw() { return compare(other) >= 0;	}

	/*************** display function	*******************************/	
	std::string		to_canonical_string()	const throw();
	std::string		to_string()		const throw();
	friend	std::ostream &	operator << (std::ostream & os, const bitfield_t &bitfield)	throw()
				{ return os << bitfield.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bitfield_t &bitfield)	throw();
	friend	serial_t& operator >> (serial_t & serial, bitfield_t &bitfield) 	throw(serial_except_t);	

	/*************** List of friend class	*******************************/
	friend class	bitfield_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BITFIELD_HPP__  */










