/*! \file
    \brief Header of the delaygen_vapi_t

*/


#ifndef __NEOIP_DELAYGEN_VAPI_HPP__ 
#define __NEOIP_DELAYGEN_VAPI_HPP__ 
/* system include */

/* local include */
#include "neoip_delay.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief the delaygen_vapi_t object is the base from which delaygen_*_t derives
 */
class delaygen_vapi_t {
private:

public:	/**************** virtal API	***************************************/
	//! goto the next value of the delaygen_vapi_t
	virtual	void			increment()		throw()		= 0;
	//! return the delaygen_arg_t for this delaygen_vapi_t
	virtual	delaygen_arg_t		get_arg()		const throw()	= 0;
	//! return the current value of the delaygen_vapi_t
	virtual	delay_t			get_current_delay()	const throw()	= 0;
	//! return the amount of time already notified as expired
	virtual delay_t			get_elapsed_delay()	const throw()	= 0;
	//! notify the expiration of the current() delay - to be called when the delay has been expired
	virtual void			notify_expiration()	throw()		= 0;
	//! return true if the delaygen_vapi_t is timedout, false otherwise
	virtual bool			is_timedout()		const throw()	= 0;

	//! virtual destructorclass_name
	virtual ~delaygen_vapi_t() {};	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAYGEN_VAPI_HPP__  */






