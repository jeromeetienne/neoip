/*! \file
    \brief Header of all the info about the application
    
*/


#ifndef __NEOIP_UPNP_INFO_HPP__ 
#define __NEOIP_UPNP_INFO_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/* TODO
 * - version		= 0.0.1
 * - summary		= "super apps in one line"
 * - description	= "many lines to explain super-apps"
 * - type		= apps_type_t "sys_boot" usr_boot etc...
 * - canon name 	= "neoip_upnp"
 * - human name		= "NeoIP upnp"
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

#define NEOIP_APPS_CANON_NAME	"neoip_upnp" 
#define NEOIP_APPS_HUMAN_NAME	"NeoIP upnp"
#define	NEOIP_APPS_VERSION	"0.0.1"
#define NEOIP_APPS_SUMMARY	"Small tool to use neoip upnp technology"
#define NEOIP_APPS_TYPE		"USR_TEMP"
#define NEOIP_APPS_LONGDESC		\
		"sloaaaaaa one\n"	\
		"sloaaaaaa two\n"	\
		"sloaaaaaa three\n"	\
		"sloaaaaaa four"
/******** NEOIP_APPS_LONGDESC-ENDMARKER -- DO NOT REMOVE	***************/

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_INFO_HPP__  */



