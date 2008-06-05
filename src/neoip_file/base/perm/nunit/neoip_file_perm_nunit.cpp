/*! \file
    \brief Definition of the unit test for the \ref file_perm_t

- NOTE: this nunit uses absolute values of the octal conversion
  - this seems to be unix-specific

*/

/* system include */
#include <fcntl.h>	// to get the S_IRWXU and cie for the convertion
#include <sys/stat.h>
/* local include */
#include "neoip_file_perm_nunit.hpp"
#include "neoip_file_perm.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_perm_testclass_t::shortcut(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// do some text of the file_perm_t::to_octal()
	NUNIT_ASSERT( file_perm_t::USR_RWX.to_octal() == 0700 );
#ifndef	_WIN32	// _WIN32 do not support permission for group/other
	NUNIT_ASSERT( file_perm_t::GRP_RW_.to_octal() == 0060 );
	NUNIT_ASSERT( file_perm_t::OTH_R_X.to_octal() == 0005 );
#endif
	// report no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_perm_testclass_t::to_octal(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// do some text of the file_perm_t::to_octal()
	NUNIT_ASSERT( file_perm_t::USR_READ.to_octal()  == 0400 );
	NUNIT_ASSERT( file_perm_t::USR_WRITE.to_octal() == 0200 );
	NUNIT_ASSERT( file_perm_t::USR_EXEC.to_octal()  == 0100 );
#ifndef	_WIN32	// _WIN32 do not support permission for group/other
	NUNIT_ASSERT( file_perm_t::GRP_EXEC.to_octal()  == 0010 );
#endif
	// report no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_perm_testclass_t::from_octal(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// do some text of the file_perm_t::from_octal()
	NUNIT_ASSERT( file_perm_t::from_octal(0400) == file_perm_t::USR_READ	);
	NUNIT_ASSERT( file_perm_t::from_octal(0200) == file_perm_t::USR_WRITE	);
	NUNIT_ASSERT( file_perm_t::from_octal(0100) == file_perm_t::USR_EXEC	);
	NUNIT_ASSERT( file_perm_t::from_octal(0700) == file_perm_t::USR_RWX	);
	NUNIT_ASSERT( file_perm_t::from_octal(0600) == file_perm_t::USR_RW_	);
#ifndef	_WIN32	// _WIN32 do not support permission for group/other
	NUNIT_ASSERT( file_perm_t::from_octal(0007) == (file_perm_t::OTH_READ | file_perm_t::OTH_WRITE 
							| file_perm_t::OTH_EXEC) );
#endif
	// report no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

