/*! \file
    \brief Header of the bt_swarm_profile_helper_t class
    
*/


#ifndef __NEOIP_BT_SWARM_PROFILE_HELPER_HPP__ 
#define __NEOIP_BT_SWARM_PROFILE_HELPER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_profile_t;

/** \brief profileic helpers to manipulate bt_swarm_profile_t
 */
class bt_swarm_profile_helper_t {
public:
	static	std::string	to_html(const bt_swarm_profile_t &swarm_profile)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_PROFILE_HELPER_HPP__  */



