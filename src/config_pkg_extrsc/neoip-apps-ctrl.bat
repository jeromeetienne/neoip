@echo off
rem This script is done to control neoip *_BOOT apps
rem 
rem

rem get the control command from the parameter
set CTRL_CMD=%1

rem goto the proper handler depending on the CTRL_CMD
IF "%CTRL_CMD%" == "start"	GOTO do_start
IF "%CTRL_CMD%" == "stop"	GOTO do_stop
IF "%CTRL_CMD%" == "status"	GOTO do_status
IF "%CTRL_CMD%" == "help"	GOTO do_help
rem if none matches, display and error and the inline help
echo Invalid %CTRL_CMD% command
goto do_help

:do_start
	sc start @APPS_NAME@
	GOTO END
:do_stop
	sc stop @APPS_NAME@
	GOTO END
:do_status
	sc query @APPS_NAME@
	GOTO END
:do_help
	echo %0 [options] - to control the @APPS_NAME@ service
	echo The possible options are:
	echo start	: Start the @APPS_NAME@ service
	echo stop	: Stop the @APPS_NAME@ service
	echo status	: Display the status @APPS_NAME@ service. (i.e. running or not)
	echo help	: Display the inlined help
	GOTO END

:END