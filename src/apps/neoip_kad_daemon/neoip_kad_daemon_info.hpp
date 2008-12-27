/*! \file
    \brief Header of all the info about the application

*/


#ifndef __NEOIP_KAD_DAEMON_INFO_HPP__
#define __NEOIP_KAD_DAEMON_INFO_HPP__

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/* TODO
 * - version		= 0.0.1
 * - summary		= "super apps in one line"
 * - description	= "many lines to explain super-apps"
 * - type		= apps_type_t "sys_boot" usr_boot etc...
 * - canon name 	= "neoip_kad_daemon"
 * - human name		= "NeoIP kad_daemon"
 *
 * - all those field will be passed to lib_apps_t
 * - additionnaly they will be parsed in the packager to fill the package
 *   - how to format the info to make it easy to extract
 *   - especially the description which is multi line
 * - this include MUST be included ONLY in *_main.cpp
 *   - other part of the software should use lib_apps_t object
 * - CANON_NAME must not use '-' but '_'
 *
 */

#define NEOIP_APPS_CANON_NAME	"neoip_kad_daemon"
#define NEOIP_APPS_HUMAN_NAME	"NeoIP kad_daemon"
#include			"neoip_kad_daemon_info_version.hpp"
#define NEOIP_APPS_SUMMARY	"daemon to use from neoip technology"
#define NEOIP_APPS_TYPE		"USR_BOOT"
#define NEOIP_APPS_LONGDESC		\
		"sloaaaaaa one\n"	\
		"sloaaaaaa two\n"	\
		"sloaaaaaa three\n"	\
		"sloaaaaaa four"
/******** NEOIP_APPS_LONGDESC-ENDMARKER -- DO NOT REMOVE	***************/

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_DAEMON_INFO_HPP__  */



