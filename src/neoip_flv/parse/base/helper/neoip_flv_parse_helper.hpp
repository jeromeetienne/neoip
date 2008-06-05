/*! \file
    \brief Header of the flv_parse_helper_t class
    
*/


#ifndef __NEOIP_FLV_PARSE_HELPER_HPP__ 
#define __NEOIP_FLV_PARSE_HELPER_HPP__ 
/* system include */
#include <stdio.h>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	flv_parse_t;
class	flv_parse_event_t;
class	file_size_t;

/** \brief static helpers for libneoip_cast
 */
class flv_parse_helper_t {
public:
	static file_size_t	kframe_boffset_from(flv_parse_t *flv_parse
					, const flv_parse_event_t &event)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_PARSE_HELPER_HPP__  */



