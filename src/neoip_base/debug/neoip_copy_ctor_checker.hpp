/*! \file
    \brief Header of the nocopy_ctor_t

\par Brief description
- this module allow to verify at compile time if an object is allowed to be copied
  or not.
- it does not use any CPU rescource if the code is optimized

*/


#ifndef __NEOIP_COPY_CTOR_CHECKER_HPP__ 
#define __NEOIP_COPY_CTOR_CHECKER_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief The class which inherit \ref copy_ctor_deny_t will produce a compilation error
 *         if the code require a copy constructor.
 * 
 * - This is only a debug means, it may be removed in production code
 */
class copy_ctor_deny_t {
private:// private copy constructor to produce link error if a copy of this object is required
	copy_ctor_deny_t(const copy_ctor_deny_t &other)	throw() {}
public:	copy_ctor_deny_t()	throw() {}
};

/** \brief The class doesnt do anything. it is used only to allow users to explicitly
 *         mark class as NEOIP_COPY_CTOR_ALLOW
 */
class copy_ctor_allow_t {};

// basic #define to ease the declaration
#define NEOIP_COPY_CTOR_ALLOW	private copy_ctor_allow_t
#define NEOIP_COPY_CTOR_DENY	private copy_ctor_deny_t

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_COPY_CTOR_CHECKER_HPP__  */



