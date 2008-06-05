/*! \file
    \brief Header of the \ref neoip_crypto_rnd_poll.cpp

\todo to reenable the strong crypto once done developping. disabled not to wait for entropy gathering

*/


#ifndef __NEOIP_RANDOM_POOL_HPP__ 
#define __NEOIP_RANDOM_POOL_HPP__ 

/* system include */
#include <string.h>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN
	
/** \brief class to handle diffie hellman parameters
 */
class random_pool_t {
public:	enum rndlevel_t {	NONE,
				WEAK,	//!< may be a hashchain or even a pseudo random generator
				NORMAL,	//!< it is at least a hashchain based on strong random
				STRONG,	//!< it is pure entropy
				MAX,
		};
public:
	static void read( void *data_ptr, size_t data_len, rndlevel_t level)	throw();
	static void read_weak  (void *data_ptr, int data_len)	throw(){ read(data_ptr,data_len, WEAK);	}
#if 0	// TODO to reenable strong random, disabled to gain time while testing
	static void read_normal(void *data_ptr, int data_len)	throw(){ read(data_ptr,data_len, NORMAL);}
	static void read_strong(void *data_ptr, int data_len)	throw(){ read(data_ptr,data_len, STRONG);}
#else
	static void read_normal(void *data_ptr, int data_len)	throw(){ read(data_ptr,data_len, WEAK);	}
	static void read_strong(void *data_ptr, int data_len)	throw(){ read(data_ptr,data_len, WEAK);	}
#endif	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RANDOM_POOL_HPP__  */



