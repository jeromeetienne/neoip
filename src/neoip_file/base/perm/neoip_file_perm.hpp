/*! \file
    \brief Header of the \ref bt_handshake_t

*/


#ifndef __NEOIP_FILE_PERM_HPP__ 
#define __NEOIP_FILE_PERM_HPP__ 
/* system include */
#include <stdint.h>
/* local include */
#include "neoip_bitflag.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// all the declaration - typically in a .hpp
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

NEOIP_BITFLAG_DECLARATION_START	(file_perm_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)

public:	/*************** constant declaration	*******************************/
	static const file_perm_t	USR_RWX;
	static const file_perm_t	USR_RW_;
	static const file_perm_t	USR_R_X;
	static const file_perm_t	GRP_RWX;
	static const file_perm_t	GRP_RW_;
	static const file_perm_t	GRP_R_X;
	static const file_perm_t	OTH_RWX;
	static const file_perm_t	OTH_RW_;
	static const file_perm_t	OTH_R_X;
	static const file_perm_t	DIR_DFL;
	static const file_perm_t	FILE_DFL;
public:
	/*************** convertion from/to octal	***********************/
	static	file_perm_t	from_octal(int value)	throw();
	int			to_octal()		const throw();

NEOIP_BITFLAG_DECLARATION_END	(file_perm_t, uint32_t, NEOIP_BITFLAG_COMMAND_LIST)

#undef NEOIP_BITFLAG_COMMAND_LIST

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_PERM_HPP__  */



