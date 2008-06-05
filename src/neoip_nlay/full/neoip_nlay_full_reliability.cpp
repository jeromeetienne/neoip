/*! \file
    \brief Definition of the \ref nlay_full_t

*/


/* system include */
/* local include */
#include "neoip_nlay_full.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          recv_max_len function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

nlay_err_t	nlay_full_t::recv_max_len_set(size_t recv_max_len)	throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( get_inner_type().is_reliable() );
	// TODO to code
	KLOG_ERR("NOT YET IMPLEMENTED");
	DBG_ASSERT( 0 );
	// return no error
	return nlay_err_t::OK;
}

size_t	nlay_full_t::recv_max_len_get()				const throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( get_inner_type().is_reliable() );
	// TODO to code
	KLOG_ERR("NOT YET IMPLEMENTED");
	DBG_ASSERT( 0 );
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          maysend_threshold function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

nlay_err_t	nlay_full_t::maysend_set_threshold(size_t threshold)	throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( get_inner_type().is_reliable() );
	// forward the function to nlay_full_t
	return nlay_reliability->maysend_set_threshold(threshold);
}

size_t	nlay_full_t::maysend_get_threshold()			const throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( get_inner_type().is_reliable() );
	// forward the function to nlay_full_t
	return nlay_reliability->maysend_get_threshold();
}

bool	nlay_full_t::maysend_is_set()			const throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( get_inner_type().is_reliable() );
	// forward the function to nlay_full_t
	return nlay_reliability->maysend_is_set();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            sendbuf function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

nlay_err_t	nlay_full_t::sendbuf_set_max_len(size_t sendbuf_max_len)	throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( get_inner_type().is_reliable() );
	// forward the function to nlay_full_t
	return nlay_reliability->sendbuf_set_max_len(sendbuf_max_len);
}

size_t	nlay_full_t::sendbuf_get_max_len()			const throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( get_inner_type().is_reliable() );
	// forward the function to nlay_full_t
	return nlay_reliability->sendbuf_get_max_len();
}

size_t	nlay_full_t::sendbuf_get_used_len()			const throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( get_inner_type().is_reliable() );
	// forward the function to nlay_full_t
	return nlay_reliability->sendbuf_get_used_len();
}

size_t	nlay_full_t::sendbuf_get_free_len()			const throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( get_inner_type().is_reliable() );
	// forward the function to nlay_full_t
	return nlay_reliability->sendbuf_get_free_len();
}

bool	nlay_full_t::sendbuf_is_limited()				const throw()
{
	// sanity check - the inner type MUST be a reliable one
	DBG_ASSERT( get_inner_type().is_reliable() );
	// forward the function to nlay_full_t
	return nlay_reliability->sendbuf_is_limited();
}


NEOIP_NAMESPACE_END


