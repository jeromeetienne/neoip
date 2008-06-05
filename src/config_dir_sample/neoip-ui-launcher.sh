#!/bin/sh
#
# launch a web browser to the neoip web server to configure it
#

#
# notify the user of a message (likely an error)
#
notify_user(){
	MSG=$*
	echo `basename $0` | grep _tui >/dev/null 2>/dev/null
	RET=$?
	if [ "$RET" != "0" ]; then
		FNAME=/tmp/neoip_router_configurator.$$.html
		echo $MSG >$FNAME
		$BROWSER file://$FNAME
		# remove the file after the browser
		rm $FNAME
	else
		echo $MSG
	fi
}

#
# test a list of program name and set $BROWSER to the first which match
#
find_existing_browser(){
	for i in $*; do
		if [ -x "`which $i 2>/dev/null`" ]; then
			BROWSER=$i
			return
		fi
	done
}

#
# find a browser text or graphic
#
get_browser(){
	BROWSER_GUI_LIST="konqueror firefox mozilla opera"
	BROWSER_TUI_LIST="elinks lynx"
	BROWSER=""
	if [ "$UI_TYPE" = "gui" ]; then
		BROWSER_LIST=$BROWSER_GUI_LIST
	else
		BROWSER_LIST=$BROWSER_TUI_LIST
	fi
	find_existing_browser $BROWSER_LIST
	if [ "$BROWSER" = "" ];then
		echo "unable to find a browser. the following have been tested: $BROWSER_LIST"
		exit 1
	fi
}

#
# get the address of the neoip program http web site
#
get_neoip_url(){
	CONFIG_FNAME=$URLFILE_DIR/.neoip_debug_url.$APPS_PID.http_url
	# read the url file
	if [ -r $CONFIG_FNAME ]; then
		APPS_URL=`head $CONFIG_FNAME`/neoip_lib/handler_list.html
	else
		notify_user "NeoIP url file for pid $APPS_PID is assumed to be $CONFIG_FNAME. But is not found"
		exit 1
	fi
}

#
# exit the program and display an error message if the url isnt reachable
#
tst_neoip_url(){
	APPS_URL_PORT=`echo $APPS_URL | cut -d ':' -f 3 | cut -d"/" -f 1`
	APPS_URL_HOST=`echo $APPS_URL | cut -d ':' -f 2 | tr -d '/'`
	echo 1 2>/dev/null >/dev/tcp/$APPS_URL_HOST/$APPS_URL_PORT
	RET=$?
	# if the url doesnt answer, notify the user
	if [ "$RET" != "0" ]; then
		notify_user "NeoIP router url is assumed to be $APPS_URL. But it doesnt answer! <br> try to type 'service neoip_router start' as root "
		exit 1
	fi
}

#
# Parse the command line options
#
parse_cmdline_option(){
	# log to debug
	echo cmdline=$*
	# set the default type of UI
	UI_TYPE=gui
	# set the default URLFILE_DIR
	URLFILE_DIR=/tmp
	# set the default APPS_PID
	APPS_PID=""
	# parse the command line
	while [ "$1" = "-ui" -o "$1" = "-urlfile_dir" ]; do
		case "$1" in
		
		  -ui)
		  	echo set ui
			UI_TYPE=$2
			shift 2
			;;
		  -urlfile_dir)
			URLFILE_DIR=$2
			shift 2
			;;
		  *)
			;;
		esac
	done
	# get the pid from the command line parameter
	APPS_PID=$1
}

# parse command line option
parse_cmdline_option $*

# log to debug
#echo APPS_PID=$APPS_PID
#echo UI_TYPE=$UI_TYPE
#echo URLFILE_DIR=$URLFILE_DIR

# if there is no pid
if [ "$APPS_PID" = "" ];then
	notify_user "Unable to find the running application"
	exit 1
fi

# find the better browser
get_browser
# get the http url of the program
get_neoip_url
# test if the url answer
# NOTE: disable as the /dev/tcp/host/port trick of bash dont work on debian
#tst_neoip_url
# launch the browser itself
$BROWSER $APPS_URL
