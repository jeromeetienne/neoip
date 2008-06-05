/*! \file
    \brief Header for handling the convertion of bt_swarm_resumedata_t from/to file
    
*/


#ifndef __NEOIP_BT_SWARM_RESUMEDATA_HELPER_HPP__ 
#define __NEOIP_BT_SWARM_RESUMEDATA_HELPER_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_bt_swarm_resumedata.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief static helpers to manipulate bt_swarm_resumedata_t
 */
class bt_swarm_resumedata_helper_t {
public:
	static bt_swarm_resumedata_t	from_file(const file_path_t &file_path)		throw();
	static bt_err_t			to_file(const bt_swarm_resumedata_t &bt_swarm_resumedata
								, const file_path_t &file_path)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_RESUMEDATA_HELPER_HPP__  */










