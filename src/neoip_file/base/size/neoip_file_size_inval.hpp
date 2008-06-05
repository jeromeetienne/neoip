/*! \file
    \brief Declaration of the file_size_inval_t - an interval_t of file_size_t
    
*/


#ifndef __NEOIP_FILE_SIZE_INVAL_HPP__ 
#define __NEOIP_FILE_SIZE_INVAL_HPP__ 
/* system include */
/* local include */
#include "neoip_file_size.hpp"
#include "neoip_interval.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

typedef	interval_t<file_size_t>		file_size_inval_t;
typedef	interval_item_t<file_size_t>	file_size_inval_item_t;

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_SIZE_INVAL_HPP__  */



