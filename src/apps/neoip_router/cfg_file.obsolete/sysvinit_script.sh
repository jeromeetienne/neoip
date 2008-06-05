#! /bin/sh
### BEGIN INIT INFO
# Provides:          neoip-router
# Required-Start:    $local_fs $remote_fs
# Required-Stop:     $local_fs $remote_fs
# Default-Start:     2 3 4 5
# Default-Stop:      S 0 1 6
# Short-Description: NeoIP router restores IP end-to-end and provides ip security.
# Description:       A more simple and understandable version, the users of NeoIp router
#                    no more have to care about NAT and about configuring VPN.
### END INIT INFO
#
# Author:	Jerome Etienne <jme@off.net>
#
# Version:	@(#)neoip-router 0.01	28-dec-2006  jme@off.net
#

set -e

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
DESC="NeoIP Router"
NAME=neoip-router
DAEMON=/usr/sbin/$NAME
PIDFILE=/var/run/$NAME.pid
SCRIPTNAME=/etc/init.d/$NAME

# Gracefully exit if the package has been removed.
test -x $DAEMON || exit 0

# Read config file if it is present.
#if [ -r /etc/default/$NAME ]
#then
#	. /etc/default/$NAME
#fi

#
#	Function that starts the daemon/service.
#
d_start() {
	start-stop-daemon --start --quiet --pidfile $PIDFILE	\
		--exec $DAEMON					\
		|| echo -n " already running"
}

#
#	Function that stops the daemon/service.
#
d_stop() {
	start-stop-daemon --stop --quiet --pidfile $PIDFILE	\
		--exec $DAEMON					\
		|| echo -n " not running"
}

#
#	Function that sends a SIGHUP to the daemon/service.
#
d_reload() {
	start-stop-daemon --stop --quiet --pidfile $PIDFILE	\
		--exec $DAEMON --signal 1
}

case "$1" in
  start)
	echo -n "Starting $DESC: $NAME"
	d_start
	echo "."
	;;
  stop)
	echo -n "Stopping $DESC: $NAME"
	d_stop
	echo "."
	;;
  #reload)
	#
	#	If the daemon can reload its configuration without
	#	restarting (for example, when it is sent a SIGHUP),
	#	then implement that here.
	#
	#	If the daemon responds to changes in its config file
	#	directly anyway, make this an "exit 0".
	#
	# echo -n "Reloading $DESC configuration..."
	# d_reload
	# echo "done."
  #;;
  restart|force-reload)
	#
	#	If the "reload" option is implemented, move the "force-reload"
	#	option to the "reload" entry above. If not, "force-reload" is
	#	just the same as "restart".
	#
	echo -n "Restarting $DESC: $NAME"
	d_stop
	# One second might not be time enough for a daemon to stop, 
	# if this happens, d_start will fail (and dpkg will break if 
	# the package is being upgraded). Change the timeout if needed
	# be, or change d_stop to have start-stop-daemon use --retry. 
	# Notice that using --retry slows down the shutdown process somewhat.
	sleep 1
	d_start
	echo "."
	;;
  *)
	echo "Usage: $SCRIPTNAME {start|stop|restart|force-reload}" >&2
	exit 3
	;;
esac

exit 0
