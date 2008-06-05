/*! \file
    \brief Header of the \ref profile_nunit_example_t

- see \ref neoip_profile_nunit_example.cpp
*/


#ifndef __NEOIP_PROFILE_NUNIT_EXAMPLE_HPP__ 
#define __NEOIP_PROFILE_NUNIT_EXAMPLE_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_profile.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the subprofile and direct constants 
#include "neoip_pkttype.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration for the struct fields
class	kad_rpc_profile_t;

/** \brief to handle the param in the nlay stack
 */
class profile_nunit_example_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! true if it is the ping rpc must be answered, false otherwise
	static const pkttype_profile_t	PKTTYPE_PROFILE;
public:
	/*************** ctor/dtor	***************************************/
	profile_nunit_example_t()		throw();
	~profile_nunit_example_t()		throw();

	/*************** object copy stuff	*******************************/
	profile_nunit_example_t(const profile_nunit_example_t &other)			throw();
	profile_nunit_example_t &operator = (const profile_nunit_example_t & other)	throw();

	/*************** validity function	*******************************/
	kad_err_t	check()		const throw();
	
	/*************** comparison operator	*******************************/
	int	compare(const profile_nunit_example_t & other)	    const throw();
	bool	operator ==(const profile_nunit_example_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const profile_nunit_example_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const profile_nunit_example_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const profile_nunit_example_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const profile_nunit_example_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const profile_nunit_example_t & other)  const throw(){ return compare(other) >= 0; }

#	define NUNIT_EXAMPLE_PROFILE_DIRECT_FIELD(COMMAND)	\
			COMMAND(profile_nunit_example_t, pkttype_profile_t	, pkttype)

#	define NUNIT_EXAMPLE_PROFILE_STRUCT_FIELD(COMMAND)	\
			COMMAND(profile_nunit_example_t, kad_rpc_profile_t	, rpc)

	NUNIT_EXAMPLE_PROFILE_DIRECT_FIELD( NEOIP_PROFILE_DECLARE_DIRECT_FIELD );
	NUNIT_EXAMPLE_PROFILE_STRUCT_FIELD( NEOIP_PROFILE_DECLARE_STRUCT_FIELD );
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PROFILE_NUNIT_EXAMPLE_HPP__  */



