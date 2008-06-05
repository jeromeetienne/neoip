/*! \file
    \brief Definition of the \ref ntudp_profile_t

\par TODO
- TODO it would be good to add the packet offset for the pkttype of the whole
  ntudp layer in the ntudp_profile_t
  - similarly in the kad_profile_t thus the pkttype WONT be lamely hardcoded as 
    it is now. and it is not only dirty from a theory point of view, it is 
    unflexible from a code point of view.
    - the second being unacceptable
    - recoding the kad_profile_t will be a good time for this
    - maybe some experiment local to ntudp for this
  - i did it in kad dht with success
- TODO to tune all the timer of every profile
  - to display them in http
  - to add somerandomness where needed

*/


/* system include */
/* local include */
#include "neoip_ntudp_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_profile_t::ntudp_profile_t()	throw()
{
	// sanity check - the profile MUST succeed check() by default
	DBG_ASSERT( check().succeed() );	
}
/** \brief Destructor
 */
ntudp_profile_t::~ntudp_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
ntudp_err_t	ntudp_profile_t::check()	const throw()
{
	ntudp_err_t	ntudp_err;
	// check all the subprofile
	ntudp_err	= peer().check();
	if( ntudp_err.succeed() )	ntudp_err = pserver_pool().check();
	if( ntudp_err.succeed() )	ntudp_err = pserver().check();
	if( ntudp_err.succeed() )	ntudp_err = relpeer().check();
	if( ntudp_err.succeed() )	ntudp_err = npos_watch().check();
	if( ntudp_err.succeed() )	ntudp_err = itor().check();
	if( ntudp_err.succeed() )	ntudp_err = resp().check();
	if( ntudp_err.failed() )	return ntudp_err;
	// return no error
	return ntudp_err_t::OK;
}

NEOIP_NAMESPACE_END

