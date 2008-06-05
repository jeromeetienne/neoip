/*! \file
    \brief Header to choose the timeout_t from the various implementation of event loop
    
*/


#ifdef USE_ELOOP_GLIB
#	include "neoip_timeout_glib.hpp"
#endif

#ifdef USE_ELOOP_LEVT
#	include "neoip_timeout_levt.hpp"
#endif


