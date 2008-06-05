/*! \file
    \brief Header of the get_cpu_timer function
*/


#ifndef __NEOIP_CPU_TIMER_HPP__ 
#define __NEOIP_CPU_TIMER_HPP__ 
/* system include */
#include <stdint.h>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Return the number of cycle of the cpu
 * 
 * - it is inlined and very precise way to measure cpu timing
 * - the number is influenced by the CPU frequency
 *   - so any CPU frequency change during 2 measures will make the result unusable
 *   - disable CPU frequency scaling while during measure
 * - as a consequence this function should be used only for CPU optimization and 
 *   not in 'production' code.
 */
extern "C" __inline__ uint64_t get_cpu_timer()	
{
	uint64_t nb_cycle;
	// read the nb_cycle with an asm intruction
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (nb_cycle));
	// return the result
	return nb_cycle;
}
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CPU_TIMER_HPP__  */



