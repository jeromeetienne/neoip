/*! \file
    \brief regroup all the .hpp needed to use the kad layer

*/


#ifndef __NEOIP_KAD_HPP__ 
#define __NEOIP_KAD_HPP__ 

// base include
#include "neoip_kad_listener.hpp"
#include "neoip_kad_peer.hpp"

// rpc object
#include "neoip_kad_ping_rpc.hpp"
#include "neoip_kad_store_rpc.hpp"
#include "neoip_kad_findnode_rpc.hpp"
#include "neoip_kad_findsomeval_rpc.hpp"
#include "neoip_kad_findallval_rpc.hpp"
#include "neoip_kad_delete_rpc.hpp"

// command object
#include "neoip_kad_store.hpp"
#include "neoip_kad_query.hpp"
#include "neoip_kad_delete.hpp"
#include "neoip_kad_query_some.hpp"
#include "neoip_kad_closestnode.hpp"

#endif // __NEOIP_KAD_HPP__ 

