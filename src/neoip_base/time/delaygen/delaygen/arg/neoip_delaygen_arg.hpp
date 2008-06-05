/*! \file
    \brief Header of the delaygen_arg_t

*/


#ifndef __NEOIP_DELAYGEN_ARG_HPP__ 
#define __NEOIP_DELAYGEN_ARG_HPP__ 
/* system include */

/* local include */
#include "neoip_delaygen_regular_arg.hpp"
#include "neoip_delaygen_expboff_arg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Define the argument for a delaygen_t
 */
class delaygen_arg_t : NEOIP_COPY_CTOR_ALLOW {
private:
	delaygen_regular_arg_t *	regular;
	delaygen_expboff_arg_t *	expboff;
	
	/*************** internal function	*******************************/
	void		nullify()				throw();
	void		copy(const delaygen_arg_t &other)	throw();
public:
	/*************** ctor/dtor	***************************************/
	delaygen_arg_t()	throw();
	delaygen_arg_t(const delaygen_regular_arg_t &regular_arg)	throw();
	delaygen_arg_t(const delaygen_expboff_arg_t &expboff_arg)	throw();
	~delaygen_arg_t()	throw();

	/*************** copy stuff	***************************************/
	delaygen_arg_t(const delaygen_arg_t &other)			throw();
	delaygen_arg_t &operator = (const delaygen_arg_t & other)	throw();

	/*************** Query Function	***************************************/
	bool		is_null()	const throw() { return !is_regular() && !is_expboff();	}
	bool		is_regular()			const throw() { return regular;		}
	bool		is_expboff()			const throw() { return expboff;		}
	delaygen_regular_arg_t &	get_regular()	const throw() { return *regular;	}
	delaygen_expboff_arg_t &	get_expboff()	const throw() { return *expboff;	}
	
	/*************** Some helper	***************************************/
	delay_t		first_delay()	const throw() { return is_regular() ? regular->first_delay()  : expboff->first_delay();		}
	float		random_range()	const throw() { return is_regular() ? regular->random_range() : expboff->random_range();	}
	delay_t		timeout_delay()	const throw() { return is_regular() ? regular->timeout_delay(): expboff->timeout_delay();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAYGEN_ARG_HPP__  */






