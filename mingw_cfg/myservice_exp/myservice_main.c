#include <windows.h>
#include <signal.h>

// to backup the argc/argv of the main() - aka the binPath cmdline
// - as opposed as the one given in the start function - aka the one of the start cmline
static char **	backup_argv;
static int 	backup_argc;

int main_internal(int argc, char *arg[]);

static CHAR *			serviceName 		= "myservice";
static SERVICE_STATUS_HANDLE	serviceStatusHandle	= 0;
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
	serviceStatusHandle = RegisterServiceCtrlHandler( serviceName, ServiceControlHandler );
	if( !serviceStatusHandle )	return;

	// Mark the service as starting
	serviceStatus.dwCurrentState	= SERVICE_START_PENDING;
	SetServiceStatus( serviceStatusHandle, &serviceStatus );

	// Mark the service as running
	serviceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
	serviceStatus.dwCurrentState	  = SERVICE_RUNNING;
	SetServiceStatus( serviceStatusHandle, &serviceStatus );

	// start running the service itself now
	int ret_code	= main_internal(backup_argc, backup_argv);
	if( ret_code );	// just to avoid a compiler warning about unused variable

	// Mark the service as stopped
	serviceStatus.dwControlsAccepted &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
	serviceStatus.dwCurrentState	  = SERVICE_STOPPED;
	SetServiceStatus( serviceStatusHandle, &serviceStatus );
}

/** \brief The actual main function
 */
int main( int argc, CHAR* argv[] )
{
	// create the service_table
	SERVICE_TABLE_ENTRY service_table[] = {
		{ serviceName, ServiceMain },
		{ 0, 0 }
	};

	// backup argc/argv - aka the one of the binPath of the service
	// - they are the one being used by the internal main
	backup_argc	= argc;
	backup_argv	= argv;
	
	// dispatch the service_table
	StartServiceCtrlDispatcher( service_table );
	
	// always return 0
	return 0;
}

