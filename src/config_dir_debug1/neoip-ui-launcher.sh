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
		# let the time for the browser to read the file
		sleep 2
		# remove the file
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
	BROWSER_GUI_LIST="firefox mozilla konqueror opera"
	BROWSER_TUI_LIST="elinks lynx"
	BROWSER=""
	if [ "$UI_TYPE" == "gui" ]; then
		BROWSER_LIST=$BROWSER_GUI_LIST
	else
		BROWSER_LIST=$BROWSER_TUI_LIST
	fi
	find_existing_browser $BROWSER_LIST
	if [ "$BROWSER" == "" ];then
		echo "unable to find a browser. the following have been tested: $BROWSER_LIST"
		exit -1
	fi
}

#
# get the address of the neoip program http web site
#
get_neoip_url(){
	CONFIG_FNAME=/etc/neoip/session_http_url.d/session_pid$PID.http_url
	# read the url file
	if [ -r $CONFIG_FNAME ]; then
		URL=`head $CONFIG_FNAME`/neoip_lib/handler_list.html
	else
		notify_user "NeoIP url file for pid $PID is assumed to be $CONFIG_FNAME. But is not found"
		exit -1
	fi
}

#
# exit the program and display an error message if the url isnt reachable
#
tst_neoip_url(){
	URL_PORT=`echo $URL | cut -d ':' -f 3 | cut -d"/" -f 1`
	URL_HOST=`echo $URL | cut -d ':' -f 2 | tr -d '/'`
	echo 1 2>/dev/null >/dev/tcp/$URL_HOST/$URL_PORT
	RET=$?
	# if the url doesnt answer, notify the user
	if [ "$RET" != "0" ]; then
		notify_user "NeoIP router url is assumed to be $URL. But it doesnt answer! <br> try to type 'service neoip_router start' as root "
		exit -1
	fi
}

# get the type of UI from the command line parameter
UI_TYPE=$1
# get the pid from the command line parameter
PID=$2
# find the better browser
get_browser
# get the http url of the program
get_neoip_url
# test if the url answer
# NOTE: disable as the /dev/tcp/host/port trick of bash dont work on debian
#tst_neoip_url
# launch the browser itself
$BROWSER $URL
