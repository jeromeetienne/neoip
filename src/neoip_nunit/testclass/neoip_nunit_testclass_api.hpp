/*! \file
    \brief Header of the nunit_testclass_t callback
*/


#ifndef __NEOIP_NUNIT_TESTCLASS_API_HPP__ 
#define __NEOIP_NUNIT_TESTCLASS_API_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_nunit_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief virtual api for the nunit_testclass_t
 */
class nunit_testclass_api_t {
public:
	/*************** init/deinit function - as non-pure virtual	*******/
	//! Init the testclass implementation
	virtual nunit_err_t	neoip_nunit_testclass_init()	throw()	{ return nunit_err_t::OK;	}
	//! DeInit the testclass implementation
	virtual void		neoip_nunit_testclass_deinit()	throw()	{}

	/*************** virtual destructor	*******************************/
	virtual ~nunit_testclass_api_t()	throw()	{}
};


#define NUNIT_ASSERT(cond)	do{	if( !(cond) )	return NUNIT_RES_ERROR;	}while(0)


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_TESTCLASS_API_HPP__  */



