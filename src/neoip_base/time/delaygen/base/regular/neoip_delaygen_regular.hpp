/*! \file
    \brief Header of the delaygen_regular_arg_t

*/


#ifndef __NEOIP_DELAYGEN_REGULAR_HPP__ 
#define __NEOIP_DELAYGEN_REGULAR_HPP__ 
/* system include */
/* local include */
#include "neoip_delaygen_regular_arg.hpp"
#include "neoip_delaygen_arg.hpp"
#include "neoip_delaygen_vapi.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief a regular delay_t generator
 */
class delaygen_regular_t : NEOIP_COPY_CTOR_ALLOW, public delaygen_vapi_t {
private:
	delay_t			current_real;	//!< the current value taking the random into account
	delay_t			elapsed_delay;	//!< the sum of all notified expired delay	
	delaygen_regular_arg_t	arg;		//!< the argument for this delaygen_regular_t
public:
	/*************** ctor/dtor	***************************************/
	delaygen_regular_t(const delaygen_regular_arg_t &arg)	throw();

	/*************** delagen_t vapi	***************************************/
	void			increment()		throw();
	delaygen_arg_t 	get_arg()		const throw()	{ return delaygen_arg_t(arg);	}
	delay_t			get_current_delay()	const throw();
	delay_t			get_elapsed_delay()	const throw()	{ return elapsed_delay;		}
	void			notify_expiration()	throw();
	bool			is_timedout()		const throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAYGEN_REGULAR_HPP__  */






