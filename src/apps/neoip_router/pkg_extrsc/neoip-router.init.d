#! /bin/sh
### BEGIN INIT INFO
# Provides:          @APPS_NAME@
# Required-Start:    $local_fs $remote_fs
# Required-Stop:     $local_fs $remote_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: @APPS_SUMMARY@
# Description:       here put the APPS_LONGDESC but dunno how to handle it for now
### END INIT INFO
#
# Author:	Jerome Etienne <jme@off.net>
#
# Version:	@(#)@APPS_NAME@ 0.01	28-dec-2006  jme@off.net
#

set -e

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
DAEMON=/usr/lib/@APPS_NAME@-ctrl

# Gracefully exit if the package has been removed.
test -x $DAEMON || exit 0

case "$1" in
	start|restart)
		# if neoip-router has NO current identity, log the event and return an error
		IDENTITY_FILE="/etc/neoip_router/router/current_identity.priv"
		if [ ! -e $IDENTITY_FILE ]; then
			echo "$IDENTITY_FILE not found. register an identity before launching it." >&2
			exit 1
		fi
		# do the operation if all previous tests passed
		$DAEMON $1
		;;
	stop|status)
		$DAEMON $1
		;;
	*)
		echo "Usage: $SCRIPTNAME {start|stop|restart|status}" >&2
		exit 3
		;;
esac

exit 0
