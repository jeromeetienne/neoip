/*! \file
    \brief Fake main to start in c++ and init static constructor
    
*/

/* system include */
/* local include */
#include "neoip_cpp_extern_c.hpp"

NEOIP_CPP_EXTERN_C_BEGIN
int main_router(int argc, char **argv);
NEOIP_CPP_EXTERN_C_END

int main(int argc, char **argv)
{
//	LOGPP_DBG( "Starting in c++"<< std::endl );
	return main_router( argc, argv );
}


