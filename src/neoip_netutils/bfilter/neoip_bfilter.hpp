/*! \file
    \brief Header of the \ref neoip_bfilter.cpp

*/


#ifndef __NEOIP_BFILTER_HPP__ 
#define __NEOIP_BFILTER_HPP__ 

/* system include */
#include <vector>
#include <iostream>
/* local include */
#include "neoip_bfilter_param.hpp"
#include "neoip_datum.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Implement a bloom filter
 */
class bfilter_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::vector<bool>	filter_arr;	//!< the bit array containing the filter

	size_t	filter_width;	//!< the width of the filter in bit
	size_t	nb_hash;	//!< nb_hash per element (the number of bit to set per element)
	
	datum_t	build_key(const datum_t &elem)					const throw();
	size_t	bit_idx_from_hash_idx(const datum_t &datum, size_t hash_idx)	const throw();
public:
	/*************** ctor/dtor	***************************************/
	bfilter_t()					throw();
	bfilter_t(const bfilter_param_t &param)		throw();
	~bfilter_t()					throw();

	/*************** basic query function	*******************************/
	bool		is_null()	const throw()	{ return filter_arr.empty();	}
	bfilter_param_t	get_param()	const throw()	{ return bfilter_param_t().filter_width(filter_width).nb_hash(nb_hash); }
	/*************** element function	*******************************/
	bfilter_t &	insert(const datum_t &elem)	throw();
	bool		contain(const datum_t &elem)	const throw();
	bfilter_t &	insert(const void *elem_ptr, size_t elem_len)	throw()
			{ return insert( datum_t(elem_ptr, elem_len, datum_t::NOCOPY));		}
	bool		contain(const void *elem_ptr, size_t elem_len)	throw()
			{ return contain( datum_t(elem_ptr, elem_len, datum_t::NOCOPY));	}

	/*************** inter bfilter_t operation	***********************/
	bfilter_t	unify(const bfilter_t & other)		const throw();
	bfilter_t	intersect(const bfilter_t & other)	const throw();

	/*************** Comparison function	*******************************/
	int	compare(const bfilter_t &other)		const throw();
	bool 	operator == (const bfilter_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const bfilter_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const bfilter_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const bfilter_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const bfilter_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const bfilter_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/*************** static function to tune bfilter_t	***************/
	static double	cpu_false_positive_rate(size_t filter_width, size_t nb_hash
								, size_t nb_inserted_key)	throw();
	static bfilter_param_t	cpu_optimal_param(double desired_error_rate, size_t nb_inserted_key)	throw();
	
	/*************** display function	*******************************/
	std::string	to_string()	const throw();
	friend std::ostream& operator << ( std::ostream& os, const bfilter_t &bfilter)	throw()
		{ return os << bfilter.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BFILTER_HPP__  */



