/*! \file
    \brief Header of the \ref rate_sched_t
    
*/


#ifndef __NEOIP_RATE_SCHED_HPP__ 
#define __NEOIP_RATE_SCHED_HPP__ 
/* system include */
#include <list>
#include <float.h>
/* local include */
#include "neoip_rate_sched_wikidbg.hpp"
#include "neoip_rate_err.hpp"
#include "neoip_rate_prec.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rate_limit_t;

/** \brief Handle a rate scheduler which allow global max_rate and
 *         precendence per user.
 */
class rate_sched_t : NEOIP_COPY_CTOR_DENY, private wikidbg_obj_t<rate_sched_t, rate_sched_wikidbg_init> {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the max_rate when it is infinite
	static const double	INFINITE_RATE_VAL	= FLT_MAX;
	static const double	INFINITE_RATE;
private:
	double			m_max_rate;	//!< the global maximum rate (aka nb occurence per second)
	rate_prec_t		m_maxi_precsum;
	rate_prec_t		m_used_precsum;

	/*************** Internal function	*******************************/
	bool			is_sane()	throw();
	
	/*************** store the rate_limit_t	*******************************/
	std::list<rate_limit_t *>	limit_db;
	void item_dolink(rate_limit_t *item) 	throw()	{ limit_db.push_back(item);	}
	void item_unlink(rate_limit_t *item)	throw()	{ limit_db.remove(item);		}
public:
	/*************** ctor/dtor	***************************************/
	rate_sched_t()		throw();
	~rate_sched_t()		throw();

	/*************** Setup function	***************************************/
	rate_err_t		start(double _max_rate = INFINITE_RATE)		throw();
	
	/*************** Query function	***************************************/
	double			max_rate()	const throw()	{ return m_max_rate;	}
	const rate_prec_t &	maxi_precsum()	const throw()	{ return m_maxi_precsum;}
	const rate_prec_t &	used_precsum()	const throw()	{ return m_used_precsum;}

	/*************** Action function	*******************************/
	rate_sched_t &		max_rate(double new_value)	throw();
	
	/*************** list of friend class	*******************************/
	friend class	rate_sched_wikidbg_t;
	friend class	rate_limit_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RATE_SCHED_HPP__  */










