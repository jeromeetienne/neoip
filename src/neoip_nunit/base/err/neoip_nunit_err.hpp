/*! \file
    \brief Header of the \ref nunit_err_t

*/


#ifndef __NEOIP_NUNIT_ERR_HPP__ 
#define __NEOIP_NUNIT_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// TODO may be a good idea to rename the test result as PASSED/FAILED
// it is closer to the usual test vocabulary that the usual OK/ERROR
NEOIP_ERRTYPE_DECLARATION_START(nunit_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(nunit_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(nunit_err_t	, ERROR)
NEOIP_ERRTYPE_DECLARATION_ITEM(nunit_err_t	, DELAYED)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(nunit_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(nunit_err_t)


NEOIP_NAMESPACE_END


#endif	/* __NEOIP_NUNIT_ERR_HPP__  */



