/*! \file
    \brief Header of the \ref dvar_vapi_t
*/


#ifndef __NEOIP_DVAR_VAPI_HPP__ 
#define __NEOIP_DVAR_VAPI_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_dvar_type.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Definition of the virtual API for the dvar_t
 */
class dvar_vapi_t {
public:
	//! Return the dvar_type_t of the dvar
	virtual	dvar_type_t	type()					const throw() = 0;
	//! return a clone of this dvar, dynamically allocated
	virtual dvar_vapi_t *	clone()					const throw() = 0;
	//! Convert the object to a string
	virtual std::string	to_string()				const throw() = 0;
	//! Compare 2 dvar_t of the same dvar_type_t
	virtual int		compare(const dvar_vapi_t &dvar_vapi)	const throw() = 0;

	//! virtual destructor
	virtual ~dvar_vapi_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DVAR_VAPI_HPP__  */



