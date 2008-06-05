/*! \file
    \brief Header of the delaygen_t
*/


#ifndef __NEOIP_DELAYGEN_HPP__ 
#define __NEOIP_DELAYGEN_HPP__ 
/* system include */
/* local include */
#include "neoip_delaygen_arg.hpp"
#include "neoip_delaygen_regular.hpp"
#include "neoip_delaygen_expboff.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Generator of delay_t
 */
class delaygen_t : NEOIP_COPY_CTOR_ALLOW {
private:
	delaygen_regular_t *	regular;
	delaygen_expboff_t *	expboff;
	delaygen_vapi_t *	delaygen_vapi;
	
	/*************** internal function	*******************************/
	void		nullify()			throw();
	void		copy(const delaygen_t &other)	throw();
public:
	/*************** ctor/dtor	***************************************/
	delaygen_t()						throw();
	delaygen_t(const delaygen_arg_t &delaygen_arg)		throw();
	~delaygen_t()						throw();

	/*************** copy stuff	***************************************/
	delaygen_t(const delaygen_t &other)			throw();
	delaygen_t &operator = (const delaygen_t & other)	throw();

	/*************** query function	***************************************/
	bool		is_null()	const throw()	{ return !delaygen_vapi;		}
	bool		is_regular()	const throw()	{ return regular;			}
	bool		is_expboff()	const throw()	{ return expboff;			}
	delaygen_arg_t	get_arg()	const throw()	{ return delaygen_vapi->get_arg();	}
	
	delay_t		get_current_delay()	const throw()	{ return delaygen_vapi->get_current_delay();	}
	delay_t		get_elapsed_delay()	const throw()	{ return delaygen_vapi->get_elapsed_delay();	}
	delaygen_t &	notify_expiration()	throw()		{ delaygen_vapi->notify_expiration(); return *this;	}
	bool		is_timedout()		const throw()	{ return delaygen_vapi->is_timedout();		}

	//! post increment operator returning a delay_t
	delay_t	operator ++ (int dummy)		throw() { delay_t tmp = get_current_delay(); delaygen_vapi->increment(); return tmp;	}
	//! pre increment operator returning a delay_t
	delay_t	operator ++ ()			throw() { delaygen_vapi->increment(); return get_current_delay(); 			}
	
	/*************** Compatibility function	*******************************/
	delaygen_t &	reset()		throw()	{ *this = delaygen_t(this->get_arg()); return *this;	}
	delay_t		pre_inc()	throw()		{ return ++(*this);	}
	delay_t		post_inc()	throw()		{ return (*this)++;	}
	delay_t		current()	const throw()	{ return get_current_delay();	}
	delay_t		elapsed_delay()	const throw()	{ return get_elapsed_delay();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DELAYGEN_HPP__  */



