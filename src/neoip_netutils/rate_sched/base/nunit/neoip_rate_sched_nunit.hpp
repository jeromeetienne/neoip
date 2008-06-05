/*! \file
    \brief Header of the test of rate_sched_t
*/


#ifndef __NEOIP_RATE_SCHED_NUNIT_HPP__ 
#define __NEOIP_RATE_SCHED_NUNIT_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	rate_sched_t;
class	rate_sched_nunit_item_t;

/** \brief Class which implement a nunit for the rate_sched_t
 */
class rate_sched_testclass_t : public nunit_testclass_api_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	rate_sched_t *		rate_sched;
	
	/*************** store the rate_sched_nunit_item_t	***********************/
	std::list<rate_sched_nunit_item_t *>	item_db;
	void item_dolink(rate_sched_nunit_item_t *item) throw()	{ item_db.push_back(item);	}
	void item_unlink(rate_sched_nunit_item_t *item)	throw()	{ item_db.remove(item);		}
public:
	/*************** ctor/dtor	***************************************/
	rate_sched_testclass_t()	throw();
	~rate_sched_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	
	/*************** List of friend class	*******************************/
	friend class	rate_sched_nunit_item_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RATE_SCHED_NUNIT_HPP__  */



