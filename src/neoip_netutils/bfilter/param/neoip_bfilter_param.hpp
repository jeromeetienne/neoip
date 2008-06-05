/*! \file
    \brief Header of the \ref neoip_bfilter.cpp

*/


#ifndef __NEOIP_BFILTER_PARAM_HPP__ 
#define __NEOIP_BFILTER_PARAM_HPP__ 

/* system include */
#include <stddef.h>
/* local include */
#include "neoip_argpack.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ARGPACK_DECL_CLASS_BEGIN(bfilter_param_t)
NEOIP_ARGPACK_DECL_ITEM(bfilter_param_t, size_t	, filter_width)
NEOIP_ARGPACK_DECL_ITEM(bfilter_param_t, size_t	, nb_hash)
NEOIP_ARGPACK_DECL_CLASS_END(bfilter_param_t)

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BFILTER_PARAM_HPP__  */



