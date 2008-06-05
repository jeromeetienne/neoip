#!/bin/bash
# This script is done to control neoip *_BOOT apps
#
# - TODO to clean up this mess
#   - do i still need the hooks pre/post ?
#   - a lot of variable are duplicate or not even used
# - TODO should i make a special case for neoip-router in case the user try to
#   start the daemon without registering an identity ?
#   - yep would be nice to detect... how to integrate it tho 

# TODO about rpm/fedora porting
# - start-stop-deamon is debian specific
#   - fedora put those function in /etc/init.d/functions and this file doesnt exists on debian
#   - /etc/init.d/functions doesnt exists on ubuntu, so it may be used to determine if fedora or ubuntu
#   - if [ -f /etc/init.d/functions ] ; then
#               . /etc/init.d/functions
#     fi
#   - start-stop-daemon --start --test => status $EXEC_NAME
#   - start-stop-daemon --start => daemon $EXEC_NAME $EXEC_CMDLINE_OPT
#   - start-stop-daemon --stop  => killproc $EXEC_NAME
#   - look at init.d function... those function seems to be quite complete


# get the FULL_PATH and BASE_NAME from $0 (first parameter of cmdline)
FULL_PATH=`case $0 in /*) echo $0;; *) echo $PWD/$0;; esac`
BASE_NAME=`basename $FULL_PATH | sed 's/-ctrl$//'`
INST_DIR=`dirname $FULL_PATH`
# get the APPS_NAME from the BASENAME
APPS_NAME="$BASE_NAME"


# APPS_EXT is the extension to add to the APPS_NAME to get the neoip apps executable name
APPS_EXT="-bin"


CANON_NAME=`echo "$APPS_NAME" | tr "-" "_"`
EXTRSC_ROOTDIR="$INST_DIR/pkg_extrsc"

# determine the APPS_TYPE SYS_BOOT/USR_BOOT/USR_TEMP
# - this is a pattern remplaced by the neoip-pkger while building the package
APPS_TYPE="@APPS_TYPE@"

# define all the apps rootdir - depend on the $APPS_TYPE
echo $APPS_TYPE | grep USR >/dev/null
if [ "$?" = "0" ]; then
	# all the rootdir in case APPS_TYPE is a USR_*
	#
	CONF_ROOTDIR=$HOME/.config/$CANON_NAME
	# if there is no ~/.config/$CANON_NAME, fall back on the system config dir
	if [ ! -e "$CONF_ROOTDIR/neoip_session.conf" ]; then
		CONF_ROOTDIR=/etc/$CANON_NAME
	fi
	TEMP_ROOTDIR=/tmp
	CACHE_ROOTDIR=$HOME/.cache/$CANON_NAME
	RUN_ROOTDIR=$HOME/.var/run
	LOG_ROOTDIR=$HOME/.var/log
else
	# all the rootdir in case APPS_TYPE is a SYS_*
	#
	CONF_ROOTDIR=/etc/$CANON_NAME
	TEMP_ROOTDIR=/tmp
	CACHE_ROOTDIR=/var/cache/$CANON_NAME
	RUN_ROOTDIR=/var/run
	LOG_ROOTDIR=/var/log
fi


# determine if the script is running on a rpm-based distribution or not
# - if $is_rpm_based="0", then it is assumed to be a deb_based
is_rpm_based="0"
test -f /etc/init.d/functions
if [ "$?" = "0" ]; then
	source /etc/init.d/functions
	is_rpm_based="1"
fi


################################################################################
################################################################################
#			CTRL HANDLING
################################################################################
################################################################################


# TODO clean up those variable and document them
# - i think some of them are not even used
DESC=$APPS_NAME
NAME=$APPS_NAME
EXEC_NAME=/usr/lib/$CANON_NAME/$APPS_NAME$APPS_EXT
EXEC_CMDLINE_OPT=""
PIDFILE=$RUN_ROOTDIR/$CANON_NAME.pid
CTRL_CMD=$1

################################################################################
#			function to display the inlined help
################################################################################

# the short help - to be displayed when there is an invalid option in the command line
display_short_help() {
	# build the option string
	local	OPTIONS_STR="help"
	OPTIONS_STR="$OPTIONS_STR|start|stop|restart|status"
	# display the short_help
	echo "Usage: $BASE_NAME-ctrl {$OPTIONS_STR}" >&2
}

# the long help - to be displayed when a help is explicitly asked in the command line
display_long_help() {
	display_short_help
	echo "start	: Start the $APPS_NAME daemon"
	echo "stop	: Stop the $APPS_NAME daemon"
	echo "restart	: Restart the $APPS_NAME daemon"
	echo "status	: Display the status $APPS_NAME daemon. (i.e. running or not)"
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
	if [ "$is_rpm_based" = "1" ]; then
		killproc $EXEC_NAME
                daemon $EXEC_NAME $EXEC_CMDLINE_OPT
	else
		start-stop-daemon --start --quiet --pidfile $PIDFILE -u `id -u`		\
					--exec $EXEC_NAME -- $EXEC_CMDLINE_OPT
	fi
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
	if [ "$is_rpm_based" = "1" ]; then
		killproc $EXEC_NAME
	else
		start-stop-daemon --stop --quiet --pidfile $PIDFILE -u `id -u`		\
				--exec $EXEC_NAME
	fi
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
	if [ "$is_rpm_based" = "1" ]; then
		status $EXEC_NAME
	else	
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
	fi
}

# run the hook_pre if present
hook_pre_run_if_present;

################################################################################
#		case switch for the *_BOOT apps type - mimic /etc/init.d script
################################################################################
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
	help)	display_long_help;;
	*)	echo "$APPS_NAME-ctrl: ERROR: $CTRL_CMD is an unknown command."
		display_short_help
		exit 3;;
esac

# run the hook_post if present
hook_post_run_if_present;


exit 0