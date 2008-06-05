/*! \file
    \brief Header of the \ref nunit_event_t
*/


#ifndef __NEOIP_NUNIT_EVENT_HPP__ 
#define __NEOIP_NUNIT_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_nunit_res.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store event of nunit_event_t
 */
class nunit_event_t : NEOIP_COPY_CTOR_ALLOW {
public:
	enum type {
		NONE,
		TEST_BEGIN,	//!< to notify the begining of a testfunction
		TEST_END,	//!< to notify the end of a testfunction
		TEST_NOMORE,	//!< to notify the fact that nomore test are to be done
		MAX
	};	
private:
	nunit_event_t::type	type_val;	//!< the type of the event
	
	// data specific to the type_val
	nunit_res_t		nunit_res;	//!< the nunit_res_t IIF type_val == TEST_END
	nunit_err_t		nunit_err;	//!< the nunit_err_t IIF type_val == TEST_NOMORE
public:
	/*************** ctor/dtor	***************************************/
	nunit_event_t()	throw();
	~nunit_event_t()	throw();
	
	/*************** ctor/dtor	***************************************/
	nunit_event_t::type	get_value()	const throw()	{ return type_val;	}

	/*************** fct specific to each event	***********************/
	bool			is_test_begin() 				const throw();
	static nunit_event_t	build_test_begin()				throw();
	
	bool			is_test_end() 					const throw();
	static nunit_event_t	build_test_end(const nunit_res_t &nunit_res)	throw();
	const nunit_res_t &	get_test_end_result()				const throw();
	
	bool			is_test_nomore() 				const throw();
	static nunit_event_t	build_test_nomore(const nunit_err_t &nunit_err = nunit_err_t::OK)throw();
	const nunit_err_t &	get_test_nomore_result()			const throw();

	/*************** display function	*******************************/
	friend std::ostream & operator << (std::ostream & os, const nunit_event_t &nunit_event) throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_EVENT_HPP__  */



