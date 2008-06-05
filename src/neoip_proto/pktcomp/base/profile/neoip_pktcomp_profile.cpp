/*! \file
    \brief Definition of the \ref pktcomp_profile_t

*/


/* system include */
/* local include */
#include "neoip_pktcomp_profile.hpp"
#include "neoip_pktcomp.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
pktcomp_profile_t::pktcomp_profile_t()	throw()
{
	// setup the default pktcomp option list
	// - used only by nlay_pktcomp_t during the parameter negociation
#if 0
	optlist( compress_type_arr_t().append(compress_type_t("deflate"))
					.append(compress_type_t("null_comp")));
#else	// TODO disable deflate as it create a huge memory leak
	// - transfering 4mb file over neoip-router leak 5mbyte or so
	// - bzip/zlib/null_comp doesnt leak
	// - deflate gzip/deflate does leak
	// - i left it unfixed as i dont have time now and zlib+bzip works
	optlist( compress_type_arr_t().append(compress_type_t("zlib"))
					.append(compress_type_t("null_comp")));
#endif
	// setup max_uncompress_len
	max_uncompress_len	( pktcomp_t::MAX_UNCOMPRESSED_LEN_DFL	);
	// set the default parameter for the nocomp period
	nocomp_max_nb_succ	( pktcomp_t::NOCOMP_NB_SUCC_MAX_DFL 	);
	nocomp_delay_min	( pktcomp_t::NOCOMP_DELAY_MIN_DFL	);
	nocomp_delay_max	( pktcomp_t::NOCOMP_DELAY_MAX_DFL	);
}

/** \brief Destructor
 */
pktcomp_profile_t::~pktcomp_profile_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
pktcomp_err_t	pktcomp_profile_t::check()	const throw()
{
	// return no error
	return pktcomp_err_t::OK;
}

NEOIP_NAMESPACE_END

