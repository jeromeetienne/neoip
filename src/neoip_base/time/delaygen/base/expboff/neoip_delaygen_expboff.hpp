/*! \file
    \brief Header of the delaygen_expboff_arg_t

*/


#ifndef __NEOIP_DELAYGEN_EXPBOFF_HPP__ 
#define __NEOIP_DELAYGEN_EXPBOFF_HPP__ 
/* system include */
/* local include */
#include "neoip_delaygen_expboff_arg.hpp"
#include "neoip_delaygen_arg.hpp"
#include "neoip_delaygen_vapi.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief a expboff delay_t generator
 */
class delaygen_expboff_t : NEOIP_COPY_CTOR_ALLOW, public delaygen_vapi_t {
private:
	delay_t		current_real;	//!< the current value taking the random into account
	delay_t		current_theo;	//!< the current value without taking random into account
	delay_t		elapsed_delay;	//!< the sum of all notified expired delay	
	int		inc_counter;	//!< the number of time has been incremented
	delaygen_expboff_arg_t	arg;		//!< the argument for this delaygen_expboff_t
public:
	/*************** ctor/dtor	***************************************/
	delaygen_expboff_t(const delaygen_expboff_arg_t &arg)	throw();

	/*************** delagen_t vapi	***************************************/
	void			increment()		throw();
	delaygen_arg_t 	get_arg()		const throw()	{ return delaygen_arg_t(arg);	}
	delay_t			get_current_delay()	const throw();
	delay_t			get_elapsed_delay()	const throw()	{ return elapsed_delay;		}
	void			notify_expiration()	throw();
	bool			is_timedout()		const throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAYGEN_EXPBOFF_HPP__  */






