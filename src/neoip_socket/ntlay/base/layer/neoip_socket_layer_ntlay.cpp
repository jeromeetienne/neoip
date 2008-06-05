/*! \file
    \brief Definition of the \ref socket_layer_ntlay_t

- TODO i think this module is used only for the old version of http
  - check it and if so remove it as wikidbg make it useless
*/

/* system include */
/* local include */
#include "neoip_socket_layer_ntlay.hpp"
#include "neoip_socket_full_close_ntlay.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_namespace.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                 CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
socket_layer_ntlay_t::socket_layer_ntlay_t()	throw()
{
}

/** \brief Destructor
 */
socket_layer_ntlay_t::~socket_layer_ntlay_t()	throw()
{
	// delete all the pending socket_full_close_t
	while( !full_close_db.empty() )	nipmem_delete	full_close_db.front();
}


NEOIP_NAMESPACE_END

