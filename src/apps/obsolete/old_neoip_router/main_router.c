/*===========================[ (c) JME SOFT ]===================================
FILE        : [main.c]
CREATED     : 01/02/17 18:21:30		LAST SAVE    : 01/12/11 23:12:09
WHO         : jerome@mycpu Linux 2.2.14
REMARK      :
================================================================================
==============================================================================*/

/* system include */
#include <glib.h>
#include <gmodule.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>


/* local include */
#include "util.h"
#include "onet.h"
#include "usercfg.h"
#include "reg_client.h"
#include "natdiag_sym.h"
#include "natdiag_loop.h"
#include "netdiag.h"
#include "ns_unittest.h"
#include "watchdog.h"
#include "emergency_exit.h"
#include "iaddr_log_server.h"
#include "libmain.h"
#include "prop_util.h"
#include "prop_db.h"
//#include "memb_ship.h"
#include "rdgram_utest.h"
#include "ft_fsum.h"
#include "ft_share.h"
#include "bitfield.h"
#include "intval.h"
#include "bdelta.h"
#include "bfilter.h"
#include "ncoord2.h"

static 	int 	user_interrupted_f = 0;
#define NEOIP_ROUTER_VERSION	"beta"

/////////////////////////////////////////////////////////////////////////
//   handling signal and emergency exit
/////////////////////////////////////////////////////////////////////////


/**
 * called when a timer for emergency exit expire
 */
static gboolean emergency_exit_expire(gpointer userptr)
{
	fprintf(stderr, "was unable to exit cleanly for %d-sec. doing panic shutdown\n"
						, EMERGENCY_GRACE_PERIOD_SEC );
	emergency_exit();
	return TRUE;
}

/**
 * called when a timer for emergency exit expire
 */
static void emergency_exit_signal_hd( int signum )
{
	if( signum == SIGALRM ){
		fprintf(stderr, "was unable to exit cleanly for %d-sec. doing panic shutdown\n"
						, EMERGENCY_GRACE_PERIOD_SEC );
	}
	emergency_exit();
}

//! true if the clean shutdown must not be performed (debug only)
static int no_clean_shutdown = 0;
/**
 * unix signal handler
 */
static void signal_hd( int signum )
{
	signal( signum, signal_hd );
	// test if a clean shutdown must not be done (debug only)
	if( no_clean_shutdown ){
		fprintf(stderr,"received signal %d. no clean shutdown is explicitly configured so SIGKILL the application.\n", signum );
		kill( 0, SIGKILL );	// this kill the watchdog too
		EXP_ASSERT(0);		// this point is never reached
	}

	fprintf(stderr,"received signal %d. i will try to stop as soon as possible\n", signum );
	if( signum == SIGTERM ){
		// leave more chance to glib with a shorter period
		g_timeout_add( EMERGENCY_GRACE_PERIOD_SEC*1000 - 200, emergency_exit_expire, NULL );
		signal( SIGALRM, emergency_exit_signal_hd );
		alarm( EMERGENCY_GRACE_PERIOD_SEC );
	}
	/* to call g_main_quit() in a signal hander is safe, according
	** to jme's understanding of glib-1.2 source */
	libmain_mainloop_stop();
	user_interrupted_f = 1;
}
#if 0
static prop_db_rec_t	prop_router_item_arr[] = {
	// iaddr_log section
	PROP_REC_ADMINI("iaddr_log:server", BOOL, "false", "true if the local peer must act as iaddr_log server", "Only a single peer in the whole network plays this role")
	PROP_REC_ADMINI("iaddr_log:netmask_addr", IP_ADDR, NULL, "define the network address for the iaddr allocation", "Only valid is the peer acts as a server")
	PROP_REC_ADMINI("iaddr_log:netmask_prefix_len", INT, NULL, "define the prefix length of the network address for the iaddr allocation", "Only valid is the peer acts as a server")
	// debug section
	PROP_REC_ADMINI("debug:daemon", BOOL, "true", "false if the executable should not daemonize itself when starting", NULL )
	PROP_REC_ADMINI("debug:dnsgrab", BOOL, "true", "false if the neoip router should not act as dnsgrabber", "This means the neoip router will intercept the dns request to determine if they are for neoip router." )
	PROP_REC_EXPERT("debug:tunnel", BOOL, "true", "false if the neoip router should not open a virtual network device", NULL )
	PROP_REC_EXPERT("debug:tun_stub", BOOL, "false", "true if the neoip router should not forward ip packet to the virtual network device", NULL )
	PROP_REC_EXPERT("debug:clean_shutdown", BOOL, "true", "true if the neoip router should not forward ip packet to the virtual network device", NULL )
	PROP_REC_EOL
	};
static prop_db_realm_t	prop_router_realm  = { "neoip_router", prop_router_item_arr };
#endif
/**
 * parse a config file
 */
static int router_read_cfgfile( void  )
{
	char		fname[PATH_MAX];
	char		*prop_list[] = {
			"iaddr_log:server",
			"iaddr_log:netmask_addr",
			"iaddr_log:netmask_prefix_len",
			"debug:daemon",
			"debug:dnsgrab",
			"debug:tunnel",
			"debug:tun_stub",
			"debug:clean_shutdown",
			NULL
			};
	// sanity check
	DBG_ASSERT( prop_get_str("neoip_lib", "config_dir") );
	// parse config file
	snprintf(fname, sizeof(fname), "%sneoip_router_default.ini", prop_get_str("neoip_lib", "config_dir") );
	// read the ini file
	prop_from_inifile( "neoip_router", fname, prop_list );
	// parse config file
	snprintf(fname, sizeof(fname), "%sneoip_router.ini", prop_get_str("neoip_lib", "config_dir") );	
	// read the ini file
	return prop_from_inifile( "neoip_router", fname, prop_list );
}

/**
 * display the command line usage
 */
static void disp_cmdline_usage( void )
{
	fprintf( stderr, "neoip_router version %s\n", NEOIP_ROUTER_VERSION );
	fprintf( stderr, "Usage: neoip_router [--help] [--version] [--register nodename] [--config_dir dirname] [--peername peername] [--netdiag]\n" );
	fprintf( stderr, "  -c, --config_dir dirname: set the specified dirname as the configuration directory\n" );
	fprintf( stderr, "  -r, --register nodename : register the specified nodename\n" );
	fprintf( stderr, "  -p, --peername peername : use a specific peername\n" );
	fprintf( stderr, "  -n, --netdiag           : perform a network diagnostic and display the result\n" );
	fprintf( stderr, "  -h, --help              : display this short inlined help\n" );
	fprintf( stderr, "  -v, --version           : display the software version\n" );
}

enum {
	YAV_CMDLINE_HELP 	= 'h',
	YAV_CMDLINE_REGISTER 	= 'r',
	YAV_CMDLINE_CONFIG_DIR 	= 'c',
	YAV_CMDLINE_NETDIAG 	= 'n',
	YAV_CMDLINE_VERSION 	= 'v',
	YAV_CMDLINE_PEERNAME 	= 'p'
	};
static struct option long_option[] = {
	{ "help", no_argument, 0, YAV_CMDLINE_HELP },
	{ "version", no_argument, 0, YAV_CMDLINE_VERSION },
	{ "register", required_argument, 0, YAV_CMDLINE_REGISTER},
	{ "config_dir", required_argument, 0, YAV_CMDLINE_CONFIG_DIR},
	{ "netdiag", no_argument, 0, YAV_CMDLINE_NETDIAG},
	{ "peername", required_argument, 0, YAV_CMDLINE_PEERNAME},
	{ 0, 0, 0, 0}
	};

/****************************************************************
 NAME	: main					01/02/17 18:21:47
 AIM	:
 REMARK	:
****************************************************************/
static int handle_user_config( int argc, char **argv )
{
	char	*str;
	prop_openlay();
	if( prop_from_cmdline( "cmdline", long_option, argc, argv ) ){
		disp_cmdline_usage();
		return -1;
	}
	// handle basic command line option (--help, --version)
	if( prop_get_str( "cmdline", "help" ) ){
		disp_cmdline_usage();
		return -1;	
	}
	if( prop_get_str( "cmdline", "version" ) ){
		fprintf( stderr, "neoip_router version %s\n", NEOIP_ROUTER_VERSION);
		return -1;
	}	
	// set the config dir
	str = prop_get_str_dfl( "cmdline", "config_dir", "/etc/neoip_router/" );
	if( libmain_set_config_dir( str ) )	return -1;	

	// parse config file
	if( router_read_cfgfile() ){
		fprintf(stderr,"Cant parse the neoip_router config file\n" );
		return -1;
	}
	// parse config file
	if( libmain_read_cfgfile() ){
		fprintf(stderr,"Cant parse the neoip_lib config file\n" );
		return -1;
	}
if( !prop_get_str("cmdline", "register") ){
	// set the peername in neoip_lib
	str = prop_get_str( "cmdline", "peername" );
	if( str ){
		prop_add( "neoip_lib", "main:peername", str );
	}else{
		prop_add( "neoip_lib", "main:peername", usercfg_read_oneline_file("peername.neoip") );
	}
}
	return 0;
}



/* the function signature for 'say_hello' */
typedef void (* SayHelloFunc) (const char *message);

gboolean
just_say_hello (const char *filename)
{
#if 0
	SayHelloFunc  say_hello;
	void		*ptr;
	GModule      *module;
// http://www.tcfs.it/docs/manpages/Linux/gcc-howto-6.html
// for the plugins link option
	printf("gmodule supported=%d\n", g_module_supported() );

	module = g_module_open(filename, G_MODULE_BIND_LAZY|G_MODULE_BIND_LOCAL);
	if (!module){
		printf("unable to open the sharedlib %s\n", filename);
		return FALSE;
	}

	if(!g_module_symbol (module, "say_hello", (gpointer *)&ptr)){
		printf("unable to find the symbol\n");
		return FALSE;
	}

	say_hello = ptr;
	/* call our function in the module */
	say_hello ("Hello world!");

	if(!g_module_close (module))
		g_warning ("%s: %s", filename, g_module_error ());
#endif
	return TRUE;
}

/**
 * write the pid file for this application
 */
static int write_pid_file( void )
{
	char	str[300];
	char	fname[PATH_MAX];	
	pid_t	pid = getpid();
	snprintf(str,sizeof(str), "%d", pid );
	if( !strcmp("/etc/neoip_router/", usercfg_get_config_dir() ) ){
		snprintf(fname,sizeof(fname), "/var/run/neoip_router.pid" );
	}else{
		snprintf(fname,sizeof(fname), "neoip_router.pid" );
	}
	return usercfg_oneline_file_write( fname, str );
}

/**
 * callback called when netdiag is completed
 */
static void router_netdiag_cb( void *userptr, netdiag_result_t *result )
{
	extern	GMainLoop	*glib_mainloop;
	LOGM_ERR("netdiag result: %s\n", netdiag_result_str(result) );
	// if netdiag worked, save it
	if( netdiag_result_has_error( result ) == 0 )
		netdiag_result_save( result );
	// stop the unit test
	g_main_quit( glib_mainloop );
}

static ft_share_t	*ft_share1;
static ft_share_t	*ft_share2;
static void G_GNUC_UNUSED ft_fsum_gen_cb( void *userptr, ft_fsum_t *ft_fsum, int err )
{
	LOGM_ERR("producing the ft_fsum returned %d\n", err );
//	ft_fsum_disp( ft_fsum );
#if 0
// temporary stuff to test the ft_fsum_load/save
	err = ft_fsum_save( ft_fsum, "/tmp/test.ft_fsum" );
	EXP_ASSERT( !err );
	ft_fsum_t *ft_fsum2 = ft_fsum_load( "/tmp/test.ft_fsum" );
	EXP_ASSERT( ft_fsum2 );
//	ft_fsum_disp( ft_fsum2 );
	ft_fsum_close( ft_fsum2 );
//	ft_fsum_close( ft_fsum );
#endif
	// publish the ft_fsum
	ft_share1 = ft_share_publish( NULL, ft_fsum );
}

static void G_GNUC_UNUSED test_ft_share_recved_ft_fsum_cb( void *userptr, struct ft_fsum_stt *ft_fsum )
{
	LOGM_ERR("recved a ft_fsum\n" );
	ft_fsum_disp( ft_fsum );
}

/**
 * the main function
 */
int main_router(int argc, char **argv)
{
	int	error_code = 0;
	rdgram_utest_srv_t *rdgram_utest_srv = NULL;
	rdgram_utest_cli_t *rdgram_utest_cli = NULL;

	srand(time(NULL));
	just_say_hello("/home/jerome/workspace/yavipin/src/libfoo.so.1.0");
//	asyncdns_req_start( "www.google.com", NULL, NULL );
	// unbuffer the output just to debug
	setbuf(stderr, NULL); setbuf(stdout, NULL);

	/* init signal handler for stun shutdown */
	signal( SIGINT, signal_hd );
	signal( SIGTERM, signal_hd );

	// handle user config
	if( handle_user_config( argc, argv ) )	return -1;

	// init no clean shutdown (debug only)
	no_clean_shutdown = prop_get_bool_dfl( "neoip_router", "debug:clean_shutdown", 1 ) ? 0 : 1;

	/* daemonize if needed */
	if( prop_get_bool_dfl( "neoip_router", "debug:daemon", 1) && !prop_get_str("cmdline", "netdiag") && !prop_get_str("cmdline", "register") && daemon(1,0) ){
		fprintf(stderr,"Cant be daemon because of %s\n", strerror(errno) );
		exit( 1 );
	}
	libmain_init_core();

	// register a name if needed
	if( prop_get_str("cmdline", "register") ){
		char	*name = prop_get_str("cmdline", "register");
		// register the name
		int err = reg_client_register(name, usercfg_get_reg_srv() );
		// test if the registration went well
		if( user_interrupted_f ){
			LOGM_ERR("interrupted during registration. PANIC!!!!\n");
			error_code = 99;
		}
		// if the registration succeed and no peername.neoip create one
		if( !err ){
			char *peername = usercfg_read_oneline_file("peername.neoip");
			if( !peername )	usercfg_oneline_file_write( "peername.neoip", name );
			else		free( peername );
		}
		error_code = err;
		// output the result
		LOGM_ERR("registration of %s %s \n", name, err ? "failed" : "succeed" );
		goto close_libmain_core;
	}
	
	LOGM_WARNING("NeoIP router started (version %s)\n", NEOIP_ROUTER_VERSION );

	libmain_init_net();

	// start a net diagnostic
	if( prop_get_str("cmdline", "netdiag") ){
		extern	GMainLoop	*glib_mainloop;
		netdiag_t 		*netdiag;
		netdiag_result_t	result;
{
	
	if( !netdiag_result_load( &result ) )
		LOGM_ERR("previous netdiag result: %s\n", netdiag_result_str(&result) );
	else
		LOGM_ERR("unable to load the previous netdiag result\n");
}
LOGM_ERR("starting netdiag\n");
		// open the netdiag
		netdiag = netdiag_open( router_netdiag_cb, NULL );
		// wait for the netdiag to be completed
		g_main_run( glib_mainloop );
		// close the netdiag
		netdiag_close( netdiag );
		
		if( netdiag_result_load( &result ) )	error_code = -1;
		goto close_libmain_net;
	}
	if( prop_get_bool_dfl( "neoip_router", "iaddr_log:server", 0 ) && iaddr_log_server_open() ){
		LOGM_CRIT("cant start the iaddr_log server\n");
	}
	if( strcmp(usercfg_get_peername(), "mother.4afree.net") ){
		// start the onet
		onet_openlay();
	}


	// write the pidfile
	write_pid_file();

#if 0
	// just to test the rdgram layer
	if( !strcmp(usercfg_get_peername(), "resp.4afree.net") ){
		rdgram_utest_srv = rdgram_utest_srv_open( NULL );
	}
	if( !strcmp(usercfg_get_peername(), "itor.4afree.net") ){
		nipid_t	remote_peerid;
		nipid_build_peer( &remote_peerid, "resp.4afree.net" );
		rdgram_utest_cli = rdgram_utest_cli_open( &remote_peerid, NULL );
	}
#endif
	//bitfield_utest();
	//intval_utest();
	//bdelta_utest();
	//bfilter_utest();
	//ncoord2_utest();
	//ns2_db_utest();
	//n_db_utest();
#if 0
	if( !strcmp(usercfg_get_peername(), "resdp.4afree.net") ){
		ft_fsum_t	*ft_fsum = ft_fsum_creation_start(100*1024, FT_AUTHTYPE_MSGDIGEST, NULL);
		int		err;
		EXP_ASSERT( ft_fsum );
		err = ft_fsum_creation_add_files( ft_fsum, "/tmp/test_dir", "test_dir", 1 );
		EXP_ASSERT( !err );
		ft_fsum_creation_complete( ft_fsum, ft_fsum_gen_cb, NULL );
	//	ft_fsum_close( ft_fsum );
	}
	if( !strcmp(usercfg_get_peername(), "itor.4afree.net") ){
		nipid_t		ft_fileid;
		// build the ft_fileid
		nipid_from_str( &ft_fileid, "0x950bcd7f78ec799a4bad1a8fe3d47826" );
		ft_share2 = ft_share_get_ft_fileid( NULL, &ft_fileid, FT_AUTHTYPE_MSGDIGEST, NULL, "/tmp/dst_dir"
							, test_ft_share_recved_ft_fsum_cb, NULL );
	}
#endif
	// the main loop
	libmain_mainloop_start();

	if( rdgram_utest_srv )	rdgram_utest_srv_close( rdgram_utest_srv );
	if( rdgram_utest_cli )	rdgram_utest_cli_close( rdgram_utest_cli );

	if( ft_share1 )		ft_share_close( ft_share1 );
	if( ft_share2 )		ft_share_close( ft_share2 );

/* should be reached only after a SIGINT/TERM */

	if( strcmp(usercfg_get_peername(), "mother.4afree.net") ){
		// stop the onet
		onet_closelay();
	}

	if( prop_get_bool_dfl( "neoip_router", "iaddr_log:server", 0 ) ){
		iaddr_log_server_close();
	}
close_libmain_net:;
	libmain_deinit_net();
close_libmain_core:;
	libmain_deinit_core();

	LOGM_WARNING("NeoIP router exit\n");
	return error_code;
}


