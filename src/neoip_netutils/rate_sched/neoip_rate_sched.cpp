/*! \file
    \brief Definition of the \ref rate_sched_t

- TODO this is overallocating quite a bit
  - from memory it is due to the when the allocation is done, you are garaneteed
    to get at least your share based on the maxi_precsum, even if your share
    is currently used by another.
    - so it may overallocated
  - depending on the usage pattern, especially with when thepool of rate_limit_t data arrives this 
    may overallocate a LOT
  - find a solution for this
- TODO allow an infinite max_rate
  - kludged rapidely thru rate_sched_t::INFINITE_RATE but not tested at all
 

\par Brief Description
- a rate_sched_t has a global maximum rate and got several rate_limit_t attached to it
  - the goal is to use this maximum rate without going above it
- it is not garanteed that it wont go above this rate, because there is a lot
  of statistics and timer which make the prediction possibly inaccurate.
  - so even if it is designed to avoid those cases, they will happen more often
    than not. but i tried to reduce the exceeding amount as much as possible
- each rate_limit_t has a maxi precedence which can be modified during the life
  of the rate_limit_t
- a precedence is a number chosen by the caller without constraint, coded in rate_prec_t
- rate_sched_t::maxi_precsum is the sum of all the attached rate_limit_t::maxi_prec
  - it is incrementally maintened allows a better scaling when there are a lot of
    rate_limit_t.
- to determine the maxi rate of a given rate_limit_t, it is done by the ratio
  of the rate_limit_t::maxi_prec versus the rate_sched_t::maxi_precsum
- a rate_limit_t is garanteed to obtained at least the maxi rate.
- this is inpired from trickle at http://monkey.org/~marius/pages/?page=trickle

\par About redistribution of unused rate
- sometime a rate_limit_t has a given maxi rate but doesnt use it completly.
  in this case the unused rate is redistributed among the other rate_limit_t
  - this allows to use the global maximum rate even if some rate_limit_t are underused
- this is implemented by an used precedence. which is computed as:
  - if the currently used rate is less that the maxi rate, the used precedence
    is the ratio maxi_prec * (used_rate / maxi_rate)
  - else the used precedence is the maxi precedence
  - an used precedence is NEVER greater than the maxi prececedence
- rate_sched_t::used_precsum is the sum of all the attached rate_limit_t::used_prec
  - similarly to rate_sched_t::maxi_precsum
- thus the used rate of a given rate_limit_t is the global maximum rate multiplied
  by the ratio rate_limit_t::maxi_prec versus the rate_sched_t::used_precsum

*/

/* system include */
/* local include */
#include "neoip_rate_sched.hpp"
#include "neoip_rate_limit.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

// definition of \ref rate_sched_t constant
const double	rate_sched_t::INFINITE_RATE	= INFINITE_RATE_VAL;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
rate_sched_t::rate_sched_t()	throw()
{
	// zero some fields
	m_max_rate	= 0;
}

/** \brief Destructor
 */
rate_sched_t::~rate_sched_t()	throw()
{
	// sanity check - all rate_limit_t MUST HAVE be unlinked before
	DBG_ASSERT( limit_db.empty() );
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
rate_err_t	rate_sched_t::start(double p_max_rate)	throw()
{
	// copy the parameter
	this->m_max_rate	= p_max_rate;
	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/rate_sched_" + OSTREAMSTR(this));
	// return no error
	return rate_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			update function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the new max_rate 
 */
rate_sched_t &	rate_sched_t::max_rate(double new_value)	throw()
{
	// set the new value
	m_max_rate	= new_value;
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if rate_sched_t is sane, false otherwise
 * 
 * - intended only for debug purpose 
 * - e.g. DBG_ASSERT( rate_sched.is_sane() );
 */
bool	rate_sched_t::is_sane()	throw()
{
	std::list<rate_limit_t *>::iterator	iter;

	// check the sanity of each rate_limit_t 
	for( iter = limit_db.begin(); iter != limit_db.end(); iter++ ){
		rate_limit_t *	rate_limit	= *iter;
		DBG_ASSERT( rate_limit->is_sane() );
	}	
	
	// recompute the maxi_precsum from the sum of each rate_limit_t 
	rate_prec_t	theorical_maxi_precsum;
	for( iter = limit_db.begin(); iter != limit_db.end(); iter++ ){
		rate_limit_t *	rate_limit	= *iter;
		theorical_maxi_precsum	+= rate_limit->maxi_prec();
	}
	// recompute the used_precsum from the sum of each rate_limit_t 
	rate_prec_t	theorical_used_precsum;
	for( iter = limit_db.begin(); iter != limit_db.end(); iter++ ){
		rate_limit_t *	rate_limit	= *iter;
		theorical_used_precsum	+= rate_limit->used_prec();
	}
	
	// sanity check - theorical_maxi_precsum MUST be equal to m_maxi_precsum
	DBG_ASSERT( theorical_maxi_precsum == m_maxi_precsum );
	// sanity check - theorical_used_precsum MUST be equal to m_used_precsum
	DBG_ASSERT( theorical_used_precsum == m_used_precsum );
	// sanity check - m_used_precsum MUST be <=  m_maxi_precsum at all time
	DBG_ASSERT( m_used_precsum <= m_maxi_precsum );
	// if this point is reached, it is considered sane, and return true
	return true;
}


NEOIP_NAMESPACE_END;




