/*! \file
    \brief main() for neoip_netcat
    
*/

/* system include */
#include <iostream>
#include <string>
#include <signal.h>
#include <getopt.h>

/* local include */
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_property.hpp"
#include "neoip_mem_utest.hpp"
#include "neoip_httpd_utest.hpp"
#include "neoip_skey_auth_utest.hpp"
#include "neoip_skey_ciph_utest.hpp"
#include "neoip_skey_pkcs5_derivkey_utest.hpp"
#include "neoip_dh_utest.hpp"
#include "neoip_x509_utest.hpp"
#include "neoip_compress_utest.hpp"
#include "neoip_pktcomp_utest.hpp"
#include "neoip_option_list_utest.hpp"
#include "neoip_negorange_utest.hpp"
#include "neoip_wai_utest.hpp"
#include "neoip_id2.hpp"
#include "neoip_cnxstat_utest.hpp"
#include "neoip_bytearray_utest.hpp"
#include "neoip_acl_utest.hpp"
#include "neoip_slidwin_utest.hpp"
#include "neoip_rat0_estim_utest.hpp"
#include "neoip_rat0_limit_utest.hpp"
#include "neoip_datum_utest.hpp"
#include "neoip_datumlist_utest.hpp"
#include "neoip_serial_utest.hpp"
#include "neoip_socket_utest.hpp"
#include "neoip_omni_utest.hpp"
#include "neoip_obj_factory_utest.hpp"
#include "neoip_cpp_demangle_utest.hpp"
#include "neoip_cpp_stack_utest.hpp"
#include "neoip_strtype_utest.hpp"
#include "neoip_errtype_utest.hpp"
#include "neoip_version_utest.hpp"
#include "neoip_string_utest.hpp"
#include "neoip_event_hook_utest.hpp"
#include "neoip_expireset_utest.hpp"
#include "neoip_property_utest.hpp"
#include "neoip_slotpool_utest.hpp"
#include "neoip_base64_utest.hpp"
#include "neoip_file_utest.hpp"
#include "neoip_rdgram_utest.hpp"
#include "neoip_delay_utest.hpp"
#include "neoip_date_utest.hpp"
#include "neoip_timer_policy_utest.hpp"
#include "neoip_pktfrag_utest.hpp"
#include "neoip_reachpeer_utest.hpp"
#include "neoip_inet_addr_utest.hpp"
#include "neoip_udp_utest.hpp"
#include "neoip_tcp_utest.hpp"
#include "neoip_tcp_echo_server.hpp"
#include "neoip_tcp_chargen_server.hpp"
#include "neoip_nudp_utest.hpp"
#include "neoip_kad_utest.hpp"
#include "neoip_xmlrpc_utest.hpp"
#include "neoip_scnx_profile_utest.hpp"
#include "neoip_cb_functor_utest.hpp"

#include "nc2_server.hpp"
#include "nc2_client.hpp"


using namespace neoip;

/** \brief unix signal handler
 */
static void signal_hd( int signum )
{
	signal( signum, signal_hd );
	lib_session_get()->loop_stop();
}

/** \brief Display the command line usage
 */
static void disp_cmdline_usage( void )
{
	std::cout << "Usage: neoip_netcat [--help] --config_dir dirname [[--property var=value]*]\n";
	std::cout << "  -c, --config_dir dirname: set the specified dirname as the configuration directory\n";
	std::cout << "  -p, --property var=value: set a variable in the session property\n";
	std::cout << "  -h, --help              : display this short inlined help\n";
}

//! Define the cmdline option for getopt()
enum {
	KAD_CMDLINE_HELP	= 'h',
	KAD_CMDLINE_CONFIG_DIR	= 'c',
	KAD_CMDLINE_PROPERTY	= 'p',
	};
static struct option kad_long_option[] = {
	{ "help"	, no_argument		, 0, KAD_CMDLINE_HELP		},
	{ "config_dir"	, required_argument	, 0, KAD_CMDLINE_CONFIG_DIR	},
	{ "property"	, required_argument	, 0, KAD_CMDLINE_PROPERTY	},
	{ 0, 0, 0, 0}
	};

// store the option found in the command line
static std::string	config_dir;		//!< the configuration directory from the cmdline
static property_t	cmdline_prop;		//!< the property from the cmxline. see -p option

/** \brief Parse the command line
 */
void parse_cmdline(struct option *long_option, int argc, char *argv[] )
{
	int	c;
	// reset the getopt_long
	optind = 0;
	while( 1 ){
		int	opt_idx = 0;
		c = getopt_long_only( argc, argv, "", long_option, &opt_idx );
		/* if the parsing is completed, exit */
		if( c == -1 )  break;

		switch(c){
		case '?':	case ':':
				break;
		case KAD_CMDLINE_CONFIG_DIR:
				config_dir = optarg;
				break;
		case KAD_CMDLINE_PROPERTY:{
				// parse the option 
				std::vector<std::string>	words	= string_t::split(optarg, "=", 2);
				// check the syntax is "var=value"
				if( words.size() != 2 ){
					KLOG_STDERR("Invalid command line property : " << optarg << "\n");
					goto error;
				}
				// insert it in the property_t
				bool failed = cmdline_prop.insert_string(words[0], words[1]);
				if( failed ){
					KLOG_STDERR("Failed to insert command line property "
							<< "(have you defined it more than once?): "
							<< optarg << "\n");
					goto error;
				}
				break;}
		case KAD_CMDLINE_HELP:		goto error;
		default:	break;
		}
	}
	
	// if the config_dir is not set, report the error and exit
	if( config_dir == "" ){
		std::cerr << "The configuration directory is not specified, Set it with --config_dir\n";
		goto error;
	}
	return;
error:	disp_cmdline_usage();
	exit(-1);
}

int main(int argc, char **argv)
{
	bool failed;

	// parse the command line
	parse_cmdline(kad_long_option, argc, argv);

	// initialize the lib_session
	failed = lib_session_init();
	DBG_ASSERT( !failed );
	failed = lib_session_get()->start(config_dir, cmdline_prop);
	DBG_ASSERT( !failed );

	// unbuffer the output just to debug
	setbuf(stderr, NULL); setbuf(stdout, NULL);
	/* init signal handler for stun shutdown */
	signal( SIGINT, signal_hd );
	signal( SIGTERM, signal_hd );
	// put a 'random' seed
	srand(getpid());
#if 0
	nc2_server_t *		nc2_server	= NULL;
	nc2_client_t *		nc2_client	= NULL;
	if( lib_session.get_property().find_string("name") == "netcat1.4afree.net" ){
		nc2_server	= nipmem_new nc2_server_t("127.0.0.1:4002");
	}else if( lib_session.get_property().find_string("name") == "netcat2.4afree.net" ){
		nc2_client	= nipmem_new nc2_client_t("127.0.0.1:4002");
	}
#endif

//	neoip_inet_udp_utest();
//	utest_udp_resp_t *	utest_udp_resp = nipmem_new neoip::utest_udp_resp_t();
//	utest_udp_itor_t *	utest_udp_itor = nipmem_new neoip::utest_udp_itor_t();
//	utest_tcp_resp_t *	utest_tcp_resp = nipmem_new neoip::utest_tcp_resp_t();
//	utest_tcp_itor_t *	utest_tcp_itor = nipmem_new neoip::utest_tcp_itor_t();
	
//	tcp_echo_server_t *	tcp_echo_server		= nipmem_new neoip::tcp_echo_server_t();
//	tcp_chargen_server_t *	tcp_chargen_server	= nipmem_new neoip::tcp_chargen_server_t();


#if 0
	neoip_xmlrpc_utest();	// half backed attempt to get xmlrpc aka serial_t
	neoip_bytearray_utest();
	neoip_datum_utest();
	neoip_datumlist_utest();
	neoip_serial_utest();
	neoip_option_list_utest();
	neoip_negorange_utest();
	neoip_wai_utest();
	neoip_slidwin_utest();
	neoip_rat0_estim_utest();
	neoip_rat0_limit_utest();
	neoip_id2_t_utest();
	neoip_cnxstat_utest();
	neoip_compress_utest();
	neoip_acl_utest();
	neoip_socket_utest();
	neoip_obj_factory_utest();
	neoip_cpp_demangle_utest();
	neoip_cpp_stack_utest();
	neoip_strtype_utest();
	neoip_errtype_utest();
	neoip_version_utest();
	neoip_string_utest();
	neoip_event_hook_utest();
	neoip_slotpool_utest();
	neoip_property_utest();
	neoip_base64_utest();
	neoip_expireset_utest();
	neoip_rdgram_utest();
	neoip_delay_utest();
	neoip_date_utest();
	neoip_timer_policy_utest();
	neoip_pktfrag_utest();
	neoip_reachpeer_utest();
	neoip_inet_addr_utest();
	neoip_cb_functor_utest();		// half backed attemped to have C like callback in c++
	neoip_socket_utest_start();
	neoip_skey_auth_utest();
	neoip_skey_ciph_utest();
	neoip_skey_pkcs5_derivkey_utest();
	neoip_dh_utest();
	neoip_x509_utest();
	neoip_scnx_profile_utest();
	neoip_pktcomp_utest();
	neoip_httpd_utest();
	neoip_mem_utest();
	neoip_log_utest();
#endif
//	neoip_rat0_estim_utest_start();
//	neoip_rat0_limit_utest_start();
//	neoip_omni_utest_start();
//	neoip_socket_utest_start();
//	neoip_langrp_utest();
//	neoip_langrp_utest_start();
//	neoip_nudp_utest();
//	neoip_nudp_utest_start();
//	neoip_expireset_utest();
//	neoip_kad_utest();
//	neoip_kad_utest_start();
	neoip_xmlrpc_utest();


	KLOG_ERR("NeoIP NetCat Initialized");
	lib_session_get()->loop_run();
	//lib_session_get().loop_run();
	
//	neoip_rat0_estim_utest_end();
//	neoip_rat0_limit_utest_end();
//	neoip_omni_utest_end();
//	neoip_socket_utest_end();
//	neoip_langrp_utest_end();
//	neoip_nudp_utest_end();
//	neoip_kad_utest_end();


//	if(tcp_echo_server)	nipmem_delete	tcp_echo_server;
//	if(tcp_chargen_server)	nipmem_delete	tcp_chargen_server;

//	if(utest_udp_resp)	nipmem_delete	utest_udp_resp;
//	if(utest_udp_itor)	nipmem_delete	utest_udp_itor;
//	if(utest_tcp_resp)	nipmem_delete	utest_tcp_resp;
//	if(utest_tcp_itor)	nipmem_delete	utest_tcp_itor;


//	nipmem_delete cnxfwd_main;
#if 0
	nipmem_delete udp_utest;
	nipmem_delete tcp_utest;
	nipmem_delete echo_server;
#endif
#if 0
	if( nc2_client )	nipmem_delete nc2_client;
	if( nc2_server )	nipmem_delete nc2_server;
#endif

	lib_session_deinit();

	return 0;
}


