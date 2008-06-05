/*! \file
    \brief Definition of the \ref lib_apps_oswarp_t class
    
*/

/* system include */
#ifdef	_WIN32
#	include <windows.h>
#	include <signal.h>
#endif
/* local include */
#include "neoip_lib_apps_oswarp.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			WIN32 specific 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

// to backup the argc/argv of the main() - aka the binPath cmdline
// - as opposed as the one given in the start function - aka the one of the start cmline
static char **	backup_argv					= NULL;
static int 	backup_argc					= 0;

int 		(*backup_main_internal)(int argc, char *argv[])	= NULL; 
static CHAR *	backup_apps_name 				= NULL;
static SERVICE_STATUS_HANDLE	serviceStatusHandle		= 0;
static SERVICE_STATUS		serviceStatus;

/** \brief The service control handler - aka the one called to controle the service 
 */
static void WINAPI ServiceControlHandler( DWORD controlCode )
{
	// handle the control request according to the controlCode
	switch ( controlCode ){
	case SERVICE_CONTROL_SHUTDOWN:	// fallback
	case SERVICE_CONTROL_STOP:
		// Mark the service as STOP_PENDING
		serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus( serviceStatusHandle, &serviceStatus );

		// warn the service that it should start stopping
		// - this raise() sends the signal to all the thread of the current
		//   process. this include the main_internal thread.
		raise(SIGTERM);
		return;
	case SERVICE_CONTROL_INTERROGATE:	break;
	case SERVICE_CONTROL_PAUSE:		break;
	case SERVICE_CONTROL_CONTINUE:		break;
	default:
		if ( controlCode >= 128 && controlCode <= 255 )		break;
		else							break;
	}
	// update the service status
	SetServiceStatus( serviceStatusHandle, &serviceStatus );
}

/** \brief The service main - aka the one called to run the service
 * 
 * - WARNING: the argc/argv of this function are the one of the "sc start servicename"
 *   NOT the one of the "sc create servicename"
 *   - this is why the argc/argv of the main() are backed up 
 */
static void WINAPI ServiceMain( DWORD argc, CHAR* argv[] )
{
	// initialise service status
	serviceStatus.dwServiceType		= SERVICE_WIN32;
	serviceStatus.dwCurrentState		= SERVICE_STOPPED;
	serviceStatus.dwControlsAccepted	= 0;
	serviceStatus.dwWin32ExitCode		= NO_ERROR;
	serviceStatus.dwServiceSpecificExitCode	= NO_ERROR;
	serviceStatus.dwCheckPoint		= 0;
	serviceStatus.dwWaitHint		= 0;
	
	// get the serviceStatusHandle
	serviceStatusHandle = RegisterServiceCtrlHandler(backup_apps_name, ServiceControlHandler );
	if( !serviceStatusHandle )	return;

	// Mark the service as starting
	serviceStatus.dwCurrentState	= SERVICE_START_PENDING;
	SetServiceStatus( serviceStatusHandle, &serviceStatus );

	// Mark the service as running
	serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
	serviceStatus.dwCurrentState	  = SERVICE_RUNNING;
	SetServiceStatus( serviceStatusHandle, &serviceStatus );

	// start running the service itself now
	int ret_code	= (*backup_main_internal)(backup_argc, backup_argv);
	if( ret_code );	// just to avoid a compiler warning about unused variable

	// Mark the service as stopped
	serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
	serviceStatus.dwCurrentState	  = SERVICE_STOPPED;
	SetServiceStatus( serviceStatusHandle, &serviceStatus );
}
#endif // _WIN32

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			mkdir
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int	lib_apps_oswarp_t::main_for_daemon(const char *apps_name
				, int (*main_internal)(int argc, char *argv[]) 
				, int argc, char *argv[])		throw()
{
#ifndef	_WIN32
	// on linux, just forward to main_internal
	return (*main_internal)(argc, argv);
#else

#if 1
	/*************** daemonize kludge	*******************************/
	// - the goal is to be able to keep the usual "-d/--nodaemon" cmdline
	//   option on win32. but win32 doesnt do daemonize(), but do 'service'
	// - implemented solution/kludge:
	//   - if cmdline contains "-d", dont init the service and run main_internal directly
	//   - if cmdline doesnt contain "-d", start the service BUT fake insert "-d"
	//     in main_internal parameters to avoid calling lib_apps_helper_t::daemonize()
	// look for "-d" in the cmdline option
	int	i = 0;
	for(; i < argc && strcmp(argv[i],"-d"); i++);
	// if "-d" is present, launch main_internal directly without creating a service
	if( i != argc )	return (*main_internal)(argc, argv);
	// fake insertion the "-d" option at the end the argv
	static char *	argv_buff[255];
	for(int i = 0; i < argc; i++ )	argv_buff[i] = argv[i];
	argv_buff[argc++]	= "-d";
	argv_buff[argc]		= NULL;
	argv			= argv_buff;
#endif

	// on win32, create a window service 
	// - it gonna call the main_internal in a separate thread,
	// - it gonna listen on service event
	// - if it receives the SERVICE_CONTROL_STOP/SERVICE_CONTROL_SHUTDOWN
	//   it gonna trigger a SIGTERM signal in the main_internal thread

	// copy the parameters
	backup_apps_name	= apps_name;
	backup_main_internal	= main_internal;
	backup_argc		= argc;
	backup_argv		= argv;
	
	// create the service_table
	SERVICE_TABLE_ENTRY service_table[] = {
		{ backup_apps_name, ServiceMain },
		{ 0, 0 }
	};

	// dispatch the service_table
	StartServiceCtrlDispatcher( service_table );
#endif
	return 0;
}

NEOIP_NAMESPACE_END


