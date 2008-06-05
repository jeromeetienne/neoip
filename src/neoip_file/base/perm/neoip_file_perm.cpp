/*! \file
    \brief Definition of \ref file_perm_t

\par About _WIN32 and to/from_octal
- window do not support permission for group/other. so the group/other flags
  of file_perm_t are not converted to/from octal value


*/

/* system include */
#include <fcntl.h>	// to get the S_IRWXU and cie for the convertion
#include <sys/stat.h>
/* local include */
#include "neoip_file_perm.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

//! constant definition
const file_perm_t	file_perm_t::USR_RWX	= file_perm_t(file_perm_t::USR_READ_VAL
								| file_perm_t::USR_WRITE_VAL
								| file_perm_t::USR_EXEC_VAL);
const file_perm_t	file_perm_t::USR_RW_	= file_perm_t(file_perm_t::USR_READ_VAL
								| file_perm_t::USR_WRITE_VAL);
const file_perm_t	file_perm_t::USR_R_X	= file_perm_t(file_perm_t::USR_READ_VAL
								| file_perm_t::USR_EXEC_VAL);
const file_perm_t	file_perm_t::GRP_RWX	= file_perm_t(file_perm_t::GRP_READ_VAL
								| file_perm_t::GRP_WRITE_VAL
								| file_perm_t::GRP_EXEC_VAL);
const file_perm_t	file_perm_t::GRP_RW_	= file_perm_t(file_perm_t::GRP_READ_VAL
								| file_perm_t::GRP_WRITE_VAL);
const file_perm_t	file_perm_t::GRP_R_X	= file_perm_t(file_perm_t::GRP_READ_VAL
								| file_perm_t::GRP_EXEC_VAL);
const file_perm_t	file_perm_t::OTH_RWX	= file_perm_t(file_perm_t::OTH_READ_VAL
								| file_perm_t::OTH_WRITE_VAL
								| file_perm_t::OTH_EXEC_VAL);
const file_perm_t	file_perm_t::OTH_RW_	= file_perm_t(file_perm_t::OTH_READ_VAL
								| file_perm_t::OTH_WRITE_VAL);
const file_perm_t	file_perm_t::OTH_R_X	= file_perm_t(file_perm_t::OTH_READ_VAL
								| file_perm_t::OTH_EXEC_VAL);
const file_perm_t	file_perm_t::DIR_DFL	= file_perm_t(file_perm_t::USR_READ_VAL
								| file_perm_t::USR_WRITE_VAL
								| file_perm_t::USR_EXEC_VAL);
const file_perm_t	file_perm_t::FILE_DFL	= file_perm_t(file_perm_t::USR_READ_VAL
								| file_perm_t::USR_WRITE_VAL
								| file_perm_t::USR_EXEC_VAL);

// al the definitions - typically in a .cpp
#define NEOIP_BITFLAG_COMMAND_LIST(COMMAND)							\
		COMMAND(file_perm_t, uint32_t, usr_read		, USR_READ		, 0)	\
		COMMAND(file_perm_t, uint32_t, usr_write	, USR_WRITE		, 1)	\
		COMMAND(file_perm_t, uint32_t, usr_exec		, USR_EXEC		, 2)	\
		COMMAND(file_perm_t, uint32_t, grp_read		, GRP_READ		, 3)	\
		COMMAND(file_perm_t, uint32_t, grp_write	, GRP_WRITE		, 4)	\
		COMMAND(file_perm_t, uint32_t, grp_exec		, GRP_EXEC		, 5)	\
		COMMAND(file_perm_t, uint32_t, oth_read		, OTH_READ		, 6)	\
		COMMAND(file_perm_t, uint32_t, oth_write	, OTH_WRITE		, 7)	\
		COMMAND(file_perm_t, uint32_t, oth_exec		, OTH_EXEC		, 8)

NEOIP_BITFLAG_DEFINITION	(file_perm_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)
#undef NEOIP_BITFLAG_COMMAND_LIST

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			convertion from/to octal
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the file_perm_t matching the usual octal representation
 * 
 * - it is typically what open()/umask() syscall() uses
 */
file_perm_t	file_perm_t::from_octal(int value)	throw()
{
	file_perm_t	file_perm;
	// convert the octal into a file_perm_t
	if( value & S_IRUSR )	file_perm	|= file_perm_t::USR_READ;
	if( value & S_IWUSR )	file_perm	|= file_perm_t::USR_WRITE;
	if( value & S_IXUSR )	file_perm	|= file_perm_t::USR_EXEC;
#ifndef	_WIN32
	if( value & S_IRGRP )	file_perm	|= file_perm_t::GRP_READ;
	if( value & S_IWGRP )	file_perm	|= file_perm_t::GRP_WRITE;
	if( value & S_IXGRP )	file_perm	|= file_perm_t::GRP_EXEC;
	if( value & S_IROTH )	file_perm	|= file_perm_t::OTH_READ;
	if( value & S_IWOTH )	file_perm	|= file_perm_t::OTH_WRITE;
	if( value & S_IXOTH )	file_perm	|= file_perm_t::OTH_EXEC;
#endif
	// return the just build file_perm_t
	return file_perm;
}

/** \brief Return the octal representation of the file_perm_t
 * 
 * - it is typically what open()/umask() syscall() uses
 */
int	file_perm_t::to_octal()		const throw()
{
	int	value	= 0;
	// convert the file_perm_t into octal
	if( is_usr_read() )	value	|= S_IRUSR;
	if( is_usr_write() )	value	|= S_IWUSR;
	if( is_usr_exec() )	value	|= S_IXUSR;
#ifndef	_WIN32
	if( is_grp_read() )	value	|= S_IRGRP;
	if( is_grp_write() )	value	|= S_IWGRP;
	if( is_grp_exec() )	value	|= S_IXGRP;
	if( is_oth_read() )	value	|= S_IROTH;
	if( is_oth_write() )	value	|= S_IWOTH;
	if( is_oth_exec() )	value	|= S_IXOTH;
#endif
	// return the just built value
	return value;
}

NEOIP_NAMESPACE_END

