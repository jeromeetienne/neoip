/*! \file
    \brief Implementation of \ref bfilter_param_t

- TODO to rename this as bfilter_arg_t as you do for the other
*/

/* system include */
/* local include */
#include "neoip_bfilter_param.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ARGPACK_DEF_CLASS_BEGIN(bfilter_param_t)
NEOIP_ARGPACK_DEF_ITEM(bfilter_param_t, size_t	, filter_width)
NEOIP_ARGPACK_DEF_ITEM(bfilter_param_t, size_t	, nb_hash)
NEOIP_ARGPACK_DEF_CLASS_END(bfilter_param_t)


NEOIP_NAMESPACE_END



