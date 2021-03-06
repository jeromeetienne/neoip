#!/bin/sh
#
# launch a web browser to the neoip_router web server to configure it
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
		sleep 10
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
		if [ "`which $i 2>/dev/null`" != "" ]; then
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
	echo `basename $0` | grep _tui >/dev/null 2>/dev/null
	RET=$?
	if [ "$RET" != "0" ]; then
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
# get the address of the neoip router http web site
#
get_neoip_router_url(){
	CONFIG_DIR=/etc/neoip_router
	if [ -r $CONFIG_DIR/httpd_url.conf ]; then
		URL=`head $CONFIG_DIR/httpd_url.conf`
	else
		URL=http://localhost:8080
	fi
}

#
# exit the program and display an error message if the url isnt reachable
#
tst_neoip_router_url(){
	URL_PORT=`echo $URL | cut -d ':' -f 3`
	URL_HOST=`echo $URL | cut -d ':' -f 2 | tr -d '/'`
	echo 1 2>/dev/null >/dev/tcp/$URL_HOST/$URL_PORT
	RET=$?
	# if the url doesnt answer, notify the user
	if [ "$RET" != "0" ]; then
		notify_user "NeoIP router url is assumed to be $URL. But it doesnt answer! <br> try to type 'service neoip_router start' as root "
		exit -1
	fi
}

# find the better browser
get_browser
# get the http url of the router
get_neoip_router_url
# test if the url answer
tst_neoip_router_url
# launch the browser itself
$BROWSER $URL
