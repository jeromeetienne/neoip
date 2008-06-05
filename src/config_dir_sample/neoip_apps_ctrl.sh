#!/bin/bash
# This script is done to control neoip apps
#
# - TODO to limit the init.d function to the *_BOOT apps_type ?
#   - to avoid confusion in the usage
#   - how to get the APPS_TYPE ? 
#     - hardcoded in the script "if thisapps then USR_BOOT etc..." - privacy issue ?
#     - get from a file ? - prevent to share the config directory
# - TODO make a special flag for 'dont destroy file'
#   - aka no remove -f and no overwrite
#   - will make the installation safer as in less dangerous in case of mistake


# get the FULL_PATH and BASE_NAME
FULL_PATH=`case $0 in /*) echo $0;; *) echo $PWD/$0;; esac`
BASE_NAME=`basename $FULL_PATH`
INST_DIR=`dirname $FULL_PATH`
# get the APPS_NAME from the BASENAME
APPS_NAME="$BASE_NAME"


# APPS_EXT is the extension to add to the APPS_NAME to get the neoip apps executable name
APPS_EXT="-bin-static"
#APPS_EXT="-bin-dynamic"

# define all the apps rootdir
# - handle only the user apps for now
# - TODO handle the system apps too
#   - how to determine the APPS_TYPE ?
#   - current kludge it hardcoded
CANON_NAME=`echo "$APPS_NAME" | tr "-" "_"`
CONF_ROOTDIR=$HOME/.config/$CANON_NAME
RUN_ROOTDIR=$HOME/.var/run
LOG_ROOTDIR=$HOME/.var/log
CACHE_ROOTDIR=$HOME/.cache/$CANON_NAME

# INITD_CMD_OK is "1" if the apps is a *_BOOT and initd-like command are supported
# - otherwise it is "0"
#INITD_CMD_OK="1"
INITD_CMD_OK="1"

EXTRSC_ROOTDIR="$INST_DIR/pkg_extrsc"
#EXTRSC_ROOTDIR="$PWD/apps/neoip_router/pkg_extrsc"


# if the cmdline is NOT for control, just forward it to the executable
[ "$#" = "0" -o "$1" != "--ctrl" ] && { exec $INST_DIR/$APPS_NAME$APPS_EXT $* ;}
#[ "$#" = "0" -o "$1" != "--ctrl" ] && { echo "exec $INST_DIR/$APPS_NAME$APPS_EXT $*"; exit ;}


################################################################################
################################################################################
#			CTRL HANDLING
################################################################################
################################################################################

# TODO should it be possible to do multiple CTRL_CMD in sequence ?
# - e.g. neoip-router --ctrl doinstall_confdir doinstall_extrsc start
# - yep seems good as it is more practical 
# - especially it would allow less command for the first install and thus
#   make it easier for the user
# - possible solution:
#   - have a loop which recall this same script each time with a single CTRL_CMD

# TODO clean up those variable and document them
# - i think some of them are not even used
DESC=$APPS_NAME
NAME=$APPS_NAME
EXEC_NAME=$INST_DIR/$APPS_NAME$APPS_EXT
EXEC_CMDLINE_OPT=""
PIDFILE=$RUN_ROOTDIR/$CANON_NAME.pid
CTRL_CMD=$2


################################################################################
# Function that doinstall configuration directory
################################################################################
d_isinstall_confdir() {
	local	is_present
	# test if the configuration dir is present or not
	if [ -a "$CONF_ROOTDIR/neoip_session.conf" ]; then
		is_present=1
	else
		is_present=0
	fi
	# display the result
	if [ "$is_present" = "1" ]; then
		echo " IS present (in $CONF_ROOTDIR)"
	else
		echo " IS NOT present (in $CONF_ROOTDIR)"
	fi
}

################################################################################
# Function that doinstall configuration directory
################################################################################
d_doinstall_confdir() {
	local	is_present
	# test if the configuration dir is present or not
	if [ -a "$CONF_ROOTDIR/neoip_session.conf" ]; then
		is_present=1
	else
		is_present=0
	fi
	# if it is already present do nothing
	if [ "$is_present" = "1" ]; then
		echo "Already present in $CONF_ROOTDIR. aborting"
		return
	fi
	# log the event
	echo -n "copying sample to $CONF_ROOTDIR..."
	# do copy the config_dir_sample
	cp -r $INST_DIR/config_dir_sample $CONF_ROOTDIR
	# set the $CONF_ROOTDIR as writable for the user
	# - 0install makes $INST_DIR/config_dir_sample unwritable by all
	chmod -R u+w $CONF_ROOTDIR
	# log the event
	echo "DONE"
}

################################################################################
# Function that uninstall configuration directory
################################################################################
d_uninstall_confdir() {
	local	is_present
	# test if the configuration dir is present or not
	if [ -a "$CONF_ROOTDIR/neoip_session.conf" ]; then
		is_present=1
	else
		is_present=0
	fi
	# if it is already present do nothing
	if [ "$is_present" = "0" ]; then
		echo "NOT present in $CONF_ROOTDIR. aborting"
		return
	fi
	# log the event
	echo -n "removing $CONF_ROOTDIR..."
	# do copy the config_dir_sample
	echo "rm -fr $CONF_ROOTDIR"
	rm -fr $CONF_ROOTDIR
	# log the event
	echo "DONE"
}

################################################################################
# Function that determine what are the instfile for extrsc
################################################################################

instfile_extrsc_man() {
	local declare INSTFILE_ARR=()
	local MAN_SRCDIR="$EXTRSC_ROOTDIR"
	local MAN_DSTDIR="/usr/share/man"

	# if there are no manpage, do nothing
	ls $MAN_SRCDIR/*.[0-9] 2>/dev/null >/dev/null
	if [ "$?" != "0" ]; then
		echo ""
		return
	fi

	# loop over all the man page
	# - NOTE: this work IIF some files match the pattern... hence the test above
	for MAN_SRCFILE in $MAN_SRCDIR/*.[0-9]
	do
		# get the MAN_SECTION from the MAX_SRCFILE extension
		local MAN_SECTION=`echo $MAN_SRCFILE | sed "s/^.*\.//"`
		# get the basename
		local MAN_BASENAME=`basename $MAN_SRCFILE`
		# populat the INSTFILE_ARR
		INSTFILE_ARR[${#INSTFILE_ARR[@]}]=$MAN_SRCFILE
		INSTFILE_ARR[${#INSTFILE_ARR[@]}]=$MAN_DSTDIR/man$MAN_SECTION/$MAN_BASENAME
	done
	# 'return' the value
	echo "${INSTFILE_ARR[*]}"
}

instfile_extrsc_desktop() {
	local declare INSTFILE_ARR=()
	local SRCDIR="$EXTRSC_ROOTDIR"
	local DSTDIR="$HOME/.config/autostart"

	# if there are no .desktop, do nothing
	ls $SRCDIR/*.desktop 2>/dev/null >/dev/null
	if [ "$?" != "0" ]; then
		return
	fi

	# loop over all the .desktop
	# - NOTE: this work IIF some files match the pattern... hence the test above
	for SRCFILE in $SRCDIR/*.desktop
	do
		# populat the INSTFILE_ARR
		INSTFILE_ARR[${#INSTFILE_ARR[@]}]=$SRCFILE
		INSTFILE_ARR[${#INSTFILE_ARR[@]}]=$DSTDIR/`basename $SRCFILE`
	done
	# 'return' the value
	echo "${INSTFILE_ARR[*]}"
}

instfile_extrsc_shared_lib() {
	local declare INSTFILE_ARR=()
	local SRCDIR="$EXTRSC_ROOTDIR"
	local DSTDIR="/usr/lib"

	# if there are no .shared_lib, do nothing
	ls $SRCDIR/*.shared_lib 2>/dev/null >/dev/null
	if [ "$?" != "0" ]; then
		return
	fi

	# loop over all the .shared_lib
	# - NOTE: this work IIF some files match the pattern... hence the test above
	for SRCFILE in $SRCDIR/*.shared_lib
	do
		# populat the INSTFILE_ARR
		INSTFILE_ARR[${#INSTFILE_ARR[@]}]=$SRCFILE
		INSTFILE_ARR[${#INSTFILE_ARR[@]}]=$DSTDIR/`basename $SRCFILE | sed 's/\.shared_lib$//g'`
	done
	# 'return' the value
	echo "${INSTFILE_ARR[*]}"
}

instfile_extrsc_initd() {
	local declare INSTFILE_ARR=()
	local SRCDIR="$EXTRSC_ROOTDIR"
	local DSTDIR="/etc/init.d"

	# if there are no .init.d, do nothing
	ls $SRCDIR/*.init.d 2>/dev/null >/dev/null
	if [ "$?" != "0" ]; then
		return
	fi

	# loop over all the .init.d
	# - NOTE: this work IIF some files match the pattern... hence the test above
	for SRCFILE in $SRCDIR/*.init.d
	do
		# populat the INSTFILE_ARR
		INSTFILE_ARR[${#INSTFILE_ARR[@]}]=$SRCFILE
		INSTFILE_ARR[${#INSTFILE_ARR[@]}]=$DSTDIR/`basename $SRCFILE | sed 's/\.init.d$//g'`
	done
	# 'return' the value
	echo "${INSTFILE_ARR[*]}"
}

instfile_extrsc_all() {
	# NOTE: $(instfile_extrsc_shared_lib) is NOT included on purpose due to 
	#       two bugs/features of ld.so.conf. see neoip-router/neoip_apps_ctrl_post_hook.sh
	# TODO beaifullation - try to split this ugly long line into several
	# - i tried and failed due to syntax error
	local declare INSTFILE_ARR=($(instfile_extrsc_man) $(instfile_extrsc_desktop) $(instfile_extrsc_initd))

	# 'return' the value
	echo "${INSTFILE_ARR[*]}"
}

################################################################################
# Function that doinstall external rescource
################################################################################
d_isinstall_extrsc() {
	local declare INSTFILE_ARR=($(instfile_extrsc_all))
	
	# loop to doinstall all extrsc
	for (( i = 0 ; i < ${#INSTFILE_ARR[@]} ; i+=2 )); do
		if [ -a "${INSTFILE_ARR[$i+1]}" ]; then
			echo "${INSTFILE_ARR[$i+1]} IS installed."
		else
			echo "${INSTFILE_ARR[$i+1]} IS NOT installed."
		fi
	done
}

################################################################################
# Function that doinstall external rescource
################################################################################
d_doinstall_extrsc() {
	local declare INSTFILE_ARR=($(instfile_extrsc_all))

	# loop to doinstall all extrsc
	for (( i = 0 ; i < ${#INSTFILE_ARR[@]} ; i+=2 )); do
		# mkdir to create the directory if needed
		echo "mkdir -p `dirname ${INSTFILE_ARR[$i+1]}`"
		mkdir -p `dirname ${INSTFILE_ARR[$i+1]}`

		# symlink to the installation directory
		# - symlink thus if it is deleted, the external rsc will just 
		#   act as file not found and not as 'present' but not working.
		# TODO do i need a -f in the ln ?
		echo "ln -s ${INSTFILE_ARR[$i]} ${INSTFILE_ARR[$i+1]}"
		ln -s ${INSTFILE_ARR[$i]} ${INSTFILE_ARR[$i+1]}
	done
}

################################################################################
# Function that uninstall external rescource
################################################################################
d_uninstall_extrsc() {
	local declare INSTFILE_ARR=($(instfile_extrsc_all))
	
	# loop to uninstall all extrsc
	for (( i = 0 ; i < ${#INSTFILE_ARR[@]} ; i+=2 )) do
		echo "rm -f ${INSTFILE_ARR[$i+1]}"
		rm -f ${INSTFILE_ARR[$i+1]}
	done
}


################################################################################
# Function that clean the deamon temporary data
################################################################################
d_clean() {
	# tmp is removed automatically by the computer as it is stored in /tmp itself
	echo "rm -f $RUN_ROOTDIR/$CANON_NAME.pid"
	echo "rm -f $LOG_ROOTDIR/$CANON_NAME.log"
	echo "rm -rf $CACHE_ROOTDIR"
	echo "DONE."
}

################################################################################
#			function to display the inlined help
################################################################################

# the short help - to be displayed when there is an invalid option in the command line
display_short_help() {
	# build the option string
	local	OPTIONS_STR="help"
	if [ "$INITD_CMD_OK" = "1" ]; then
		OPTIONS_STR="$OPTIONS_STR|start|stop|restart|status"
	fi
	OPTIONS_STR="$OPTIONS_STR|clean|install_dir"
	OPTIONS_STR="$OPTIONS_STR|isinstall_extrsc|doinstall_extrsc|uninstall_extrsc"
	OPTIONS_STR="$OPTIONS_STR|isinstall_confdir|doinstall_confdir|uninstall_confdir"
	# display the short_help
	echo "Usage: $BASE_NAME --ctrl {$OPTIONS_STR}" >&2
}

# the long help - to be displayed when a help is explicitly asked in the command line
display_long_help() {
	echo "$BASE_NAME Long help (TODO stub to be completed)"



	echo "If the first parameter is '--ctrl' then the command is used to control $APPS_NAME"
	echo " else all the parameters are simply forwarded to the actual executable."

	if [ "$INITD_CMD_OK" = "1" ]; then
		echo 
		echo "They mimic the usual command of /etc/init.d scripts."
		echo "The command to handle the running of $APPS_NAME daemon are:"	
		echo "start	: Start the $APPS_NAME daemon"
		echo "stop	: Stop the $APPS_NAME daemon"
		echo "restart	: Restart the $APPS_NAME daemon"
		echo "status	: Display the status $APPS_NAME daemon. (i.e. running or not)"
	fi


	echo "clean		: Clean all the temporary and cache file of $APPS_NAME"
	echo "install_dir	: return a string containing the current installation directory"
	
	echo 
	echo "The external rescources are the file which are 'external' to the installed package"
	echo "itself - aka copied outside the installation directory. This may include man pages"
	echo ", .desktop or /etc/init.d files to automatically start the deamons or shared libraries."
	echo "The command to handles the external rescources installation are:"
	echo "isinstall_extrsc	: test if the external rescources are installed"
	echo "doinstall_extrsc	: try to install the external rescources"
	echo "uninstall_extrsc	: try to uninstall the external rescources"
	
	echo 
	echo "The configuration directory is the directory containing all the configuration"
	echo "for $APPS_NAME. It MUST be installed before starting $APPS_NAME."
	echo "The command to handles the configuration directory installation are:"
	echo "isinstall_confdir	: test if the configuration directory is installed"
	echo "doinstall_confdir	: try to install the external rescources"
	echo "uninstall_confdir	: try to uninstall the external rescources"	
}

################################################################################
#			run hook_pre if present
################################################################################
hook_pre_run_if_present() {
	local SCRIPT_NAME="$EXTRSC_ROOTDIR/neoip_apps_ctrl_hook_pre.sh"
	if [ -r "$SCRIPT_NAME" ]; then
		source $SCRIPT_NAME $CTRL_CMD
	fi
}

################################################################################
#			run hook_post if present
################################################################################
hook_post_run_if_present() {
	local SCRIPT_NAME="$EXTRSC_ROOTDIR/neoip_apps_ctrl_hook_post.sh"
	if [ -r "$SCRIPT_NAME" ]; then
		source $SCRIPT_NAME $CTRL_CMD
	fi
}

################################################################################
# Function that starts the daemon/service.
################################################################################
display_start() {
	# Start the daemon
	start-stop-daemon --start --quiet --pidfile $PIDFILE -u `id -u`		\
				--exec $EXEC_NAME -- $EXEC_CMDLINE_OPT
	# if an error occured, notify it
	if [ "$?" = "1" ]; then
		echo -n " already running"
	fi
}

################################################################################
# Function that stops the daemon/service.
################################################################################
display_stop() {
	# stop the daemon
	start-stop-daemon --stop --quiet --pidfile $PIDFILE -u `id -u`		\
				--exec $EXEC_NAME
	# if an error occured, notify it
	if [ "$?" = "1" ]; then
		echo -n " is not running"
	fi
}

################################################################################
# Function that do a status on the daemon/service.
################################################################################
display_status() {
	# test if the daemon is running
	# --quiet is not honored, so i have to put >/dev/null, hourray
	start-stop-daemon --start --test --quiet --pidfile $PIDFILE -u `id -u`	\
				--exec $EXEC_NAME 				\
				2>/dev/null >/dev/null
	local is_running=$?
	# display the result
	if [ "$is_running" = "1" ]; then
		echo -n " is running"
	else
		echo -n " is NOT running"
	fi
}




# run the hook_pre if present
hook_pre_run_if_present;

################################################################################
#		case switch for the *_BOOT apps type - mimic /etc/init.d script
################################################################################
if [ "$INITD_CMD_OK" = "1" ]; then
	case "$CTRL_CMD" in
		start)	echo -n "Starting $DESC: $NAME"
			display_start
			echo ".";;
		stop)	echo -n "Stopping $DESC: $NAME"
			display_stop
			echo ".";;
		restart)echo -n "Stopping $DESC: $NAME"
			display_stop
			echo "."
			echo -n "Starting $DESC: $NAME"
			display_start
			echo ".";;
		status)	echo -n "Statuing $DESC: $NAME"
			display_status
			echo "."
			;;
	esac
fi

################################################################################
#		case switch for the all apps type
################################################################################
case "$CTRL_CMD" in
	clean)	echo -n "Cleaning $DESC: $NAME"
		d_clean
		echo "."
		;;
	install_dir)
		echo -n "$INST_DIR"
		;;
	isinstall_extrsc)
		echo -n "Testing $DESC external resource presence..."
		d_isinstall_extrsc
		echo "DONE"
		;;
	doinstall_extrsc)
		echo -n "Installing $DESC external resource..."
		d_doinstall_extrsc
		echo "DONE"
		;;
	uninstall_extrsc)
		echo -n "Uninstalling $DESC external resource..."
		d_uninstall_extrsc
		echo "DONE"
		;;
	isinstall_confdir)
		echo -n "Testing presence of $DESC configuration directory..."
		d_isinstall_confdir
		;;
	doinstall_confdir)
		echo -n "Installing $DESC configuration directory..."
		d_doinstall_confdir
		;;
	uninstall_confdir)
		echo -n "Uninstalling $DESC configuration directory..."
		d_uninstall_confdir
		;;
esac

# run the hook_post if present
hook_post_run_if_present;

# last case on the command to display the help in case of unknown command
case "$CTRL_CMD" in
	clean)								;;
	install_dir)							;;
	isinstall_extrsc|doinstall_extrsc|uninstall_extrsc)		;;
	isinstall_confdir|doinstall_confdir|uninstall_confdir)		;;
	help)	display_long_help;;
	start|stop|restart|status)
		# TODO make a much cleaner integration of INITD_CMD_OK here
		# - currently it duplicate the help) stuff
		if [ "$INITD_CMD_OK" != "1" ]; then
			echo "$APPS_NAME: ERROR: $CTRL_CMD is an unknown command."
			display_short_help
			exit 3
		fi
		;;
	*)	echo "$APPS_NAME: ERROR: $CTRL_CMD is an unknown command."
		display_short_help
		exit 3;;
esac


exit 0