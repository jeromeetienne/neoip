/*! \file
    \brief Header to choose the eloop_t from the various implementation of event loop
    
*/


#ifdef USE_ELOOP_GLIB
#	include "neoip_eloop_glib.hpp"
#endif

#ifdef USE_ELOOP_LEVT
#	include "neoip_eloop_levt.hpp"
#endif

