/*! \file
    \brief Header of the neoip_fdwatch_t
    
*/


#ifndef __NEOIP_FDWATCH_COND0_HPP__ 
#define __NEOIP_FDWATCH_COND0_HPP__ 
/* system include */
#include <iostream>
#include <stdint.h>
#include <glib.h>
/* local include */
#include "neoip_namespace.hpp"
NEOIP_NAMESPACE_BEGIN

/** \brief Determine the possible conditions for \ref neoip_fdwatch_t
 */
class fdwatch_cond0_t {
public:	enum type {	INPUT	= 1 << 0,
			OUTPUT	= 1 << 1,
			ERROR	= 1 << 2,
			};
private:
	uint32_t	value;

	fdwatch_cond0_t &	set_flag( type flag, bool on )	throw();
	bool			get_flag( type flag )		const throw();
public:
	fdwatch_cond0_t() throw()	: value(0) {}
	fdwatch_cond0_t &	reset()			throw();

	// shortcut for INPUT
	bool			is_input()	const throw()	{ return get_flag(INPUT);	}
	fdwatch_cond0_t &	set_input(bool on=true)	throw()	{ return set_flag(INPUT, on);	}
	// shortcut for OUTPUT
	bool			is_output()	const throw()	{ return get_flag(OUTPUT);	}
	fdwatch_cond0_t &	set_output(bool on=true) throw(){ return set_flag(OUTPUT, on);	}
	// shortcut for ERROR
	bool			is_error()	const throw()	{ return get_flag(ERROR);	}
	fdwatch_cond0_t &	set_error(bool on=true)	throw()	{ return set_flag(ERROR, on);	}

	bool			is_null()	const throw();

	static fdwatch_cond0_t	from_glib_cond(GIOCondition glib_cond)		throw();
	GIOCondition		to_glib_cond() 					const throw();

friend	std::ostream & operator << (std::ostream & os, const fdwatch_cond0_t &fdwatch_cond ) throw();
};

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_FDWATCH_COND0_HPP__  */



