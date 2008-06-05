/*! \file
    \brief Definition of the unit test for the \ref argpack_t and co
    
\par NOTE
- each arguement got:
  - a type	int/float/etc...
  - a name	slota
  - a potential default value
- for each arguement, the following function are provided
  - argpack &name(const type &val) throw()	- to set the argument
  - type name(void)		const throw()	- to get the argument
  - bool name_is_set(void)	const throw()	- to know if the argument have been set

*/

/* system include */
/* local include */
#include "neoip_argpack_utest.hpp"
#include "neoip_argpack.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"



using namespace neoip;

NEOIP_NAMESPACE_BEGIN;

NEOIP_ARGPACK_DECL_CLASS_BEGIN(argpack_utest_t)
NEOIP_ARGPACK_DECL_ITEM(argpack_utest_t, float, rate)
NEOIP_ARGPACK_DECL_ITEM(argpack_utest_t, std::string, username)
NEOIP_ARGPACK_DECL_ITEM_PTR(argpack_utest_t, std::string *, str_ptr)
NEOIP_ARGPACK_DECL_CLASS_END(argpack_utest_t)

NEOIP_ARGPACK_DEF_CLASS_BEGIN(argpack_utest_t)
NEOIP_ARGPACK_DEF_ITEM(argpack_utest_t, float, rate)
NEOIP_ARGPACK_DEF_ITEM(argpack_utest_t, std::string, username)
NEOIP_ARGPACK_DEF_ITEM(argpack_utest_t, std::string *, str_ptr)
NEOIP_ARGPACK_DEF_CLASS_END(argpack_utest_t)

NEOIP_NAMESPACE_END

/** \brief unit test for the argpack_t class
 */
int neoip_argpack_utest()
{
	int	n_error = 0;
	std::string	slota	= "gribo string ptr";
	
	argpack_utest_t	argpack_utest;
	
	KLOG_ERR("rate_present=" << argpack_utest.rate_present() );
	argpack_utest.rate(0.6).username("john doe").str_ptr(&slota);
	
	
	const argpack_utest_t	argpack_const	= argpack_utest;
	KLOG_ERR("rate_present="	<< argpack_const.rate_present() );
	KLOG_ERR("rate value="		<< argpack_const.rate() );
	KLOG_ERR("rate username="	<< argpack_const.username() );
	KLOG_ERR("rate str_ptr=" 	<< *argpack_const.str_ptr() );

	if( n_error )	goto error;
	KLOG_ERR("argpack_t unit test PASSED!!!");

	return n_error;
error:;	KLOG_ERR("argpack_t unit test FAILED!!!");
	return 1;
}

