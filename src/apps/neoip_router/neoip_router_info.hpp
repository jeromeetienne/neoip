/*! \file
    \brief Header of all the info about the application

*/


#ifndef __NEOIP_ROUTER_INFO_HPP__
#define __NEOIP_ROUTER_INFO_HPP__

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/* TODO
 * - version		= 0.0.1
 * - summary		= "super apps in one line"
 * - description	= "many lines to explain super-apps"
 * - type		= apps_type_t "sys_boot" usr_boot etc...
 * - canon name 	= "neoip_router"
 * - human name		= "NeoIP router"
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

#define NEOIP_APPS_CANON_NAME	"neoip_router"
#define NEOIP_APPS_HUMAN_NAME	"NeoIP router"
#include			"neoip_router_info_version.hpp"
#define	NEOIP_APPS_VERSION	"0.0.1"
#define NEOIP_APPS_SUMMARY	"deamon to use neoip ip routing technology"
#define NEOIP_APPS_TYPE		"SYS_BOOT"
#define NEOIP_APPS_LONGDESC								\
		"neoip-router attemps to restore end-to-end to IPv4.\n"			\
		"In the process, it provides transparent security and mobility.\n"	\
		"In short, if neoip-router is installed on a computer, all IPv4\n"	\
		"application will be able to have end-to-end, security and mobility\n"	\
		"while using normal IPv4"
/******** NEOIP_APPS_LONGDESC-ENDMARKER -- DO NOT REMOVE	***************/

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_INFO_HPP__  */



