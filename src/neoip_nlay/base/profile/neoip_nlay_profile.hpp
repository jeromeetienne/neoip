/*! \file
    \brief Header of the \ref nlay_profile_t

- see \ref neoip_nlay_profile.cpp
*/


#ifndef __NEOIP_NLAY_PROFILE_HPP__ 
#define __NEOIP_NLAY_PROFILE_HPP__ 
/* system include */
/* local include */
#include "neoip_nlay_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// include for the sub-profile
#include "neoip_nlay_idletimeout_profile.hpp"
#include "neoip_nlay_closure_profile.hpp"
#include "neoip_pktfrag_profile.hpp"
#include "neoip_pktcomp_profile.hpp"
#include "neoip_nlay_rate_limit_profile.hpp"
#include "neoip_reachpeer_profile.hpp"
#include "neoip_scnx_profile.hpp"
#include "neoip_simuwan_profile.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief to handle the profile in the nlay stack
 */
class nlay_profile_t : NEOIP_COPY_CTOR_ALLOW {
private:
	// list of sub-profile
	nlay_idletimeout_profile_t	idletimeout_profile;
	nlay_closure_profile_t		closure_profile;
	pktfrag_profile_t		pktfrag_profile;
	pktcomp_profile_t		pktcomp_profile;
	nlay_rate_limit_profile_t	rate_limit_profile;
	reachpeer_profile_t		reachpeer_profile;
	scnx_profile_t			scnx_profile;
	simuwan_profile_t		simuwan_profile;
	
public:
	// ctor/dtor
	nlay_profile_t()	throw();
	~nlay_profile_t()	throw();

	// to access all the sub-profile
	nlay_idletimeout_profile_t &	idletimeout()	throw()		{ return idletimeout_profile;	};
	const nlay_idletimeout_profile_t &idletimeout()	const throw()	{ return idletimeout_profile;	};
	nlay_closure_profile_t &	closure()	throw()		{ return closure_profile;	};
	const nlay_closure_profile_t &	closure()	const throw()	{ return closure_profile;	};
	pktfrag_profile_t &		pktfrag()	throw()		{ return pktfrag_profile;	};
	const pktfrag_profile_t &	pktfrag()	const throw()	{ return pktfrag_profile;	};
	pktcomp_profile_t &		pktcomp()	throw()		{ return pktcomp_profile;	};
	const pktcomp_profile_t &	pktcomp()	const throw()	{ return pktcomp_profile;	};
	nlay_rate_limit_profile_t &	rate_limit()	throw()		{ return rate_limit_profile;	};
	const nlay_rate_limit_profile_t &rate_limit()	const throw()	{ return rate_limit_profile;	};
	reachpeer_profile_t &		reachpeer()	throw()		{ return reachpeer_profile;	};
	const reachpeer_profile_t &	reachpeer()	const throw()	{ return reachpeer_profile;	};
	scnx_profile_t &		scnx()		throw()		{ return scnx_profile;		};
	const scnx_profile_t &		scnx()		const throw()	{ return scnx_profile;		};
	simuwan_profile_t &		simuwan()	throw()		{ return simuwan_profile;	};
	const simuwan_profile_t &	simuwan()	const throw()	{ return simuwan_profile;	};
	
	// to check the profile
	nlay_err_t		check()	const throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_PROFILE_HPP__  */



