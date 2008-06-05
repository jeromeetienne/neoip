/*! \file
    \brief Header to choose the fdwatch_t from the various implementation of event loop
    
*/


#ifdef USE_ELOOP_GLIB
#	include "neoip_fdwatch_glib.hpp"
#endif

#ifdef USE_ELOOP_LEVT
#	include "neoip_fdwatch_levt.hpp"
#endif

