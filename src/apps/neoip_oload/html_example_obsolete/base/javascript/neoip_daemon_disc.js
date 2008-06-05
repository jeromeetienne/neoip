// 
// this script implement the neoip daemon discovery
// 
// - it probes the local host of the browser to find a deamon
// - the daemon is supposed to listen on a port between []first_port, last_port]
// - the daemon got a generic name say, neoip_oload, neoip_casto etc...
//   - note that it is a underscore "_" and not a dash "-". the reason is that this
//     name is used in variable and so a "-" would be interpreted as a minus
//     arithmetic operation.
// POSSIBLE IMPROVEMENT:
// - do the <script> in serie. aka test them one by one
//   - if one succeed, no need to do the others.
//   - this will speed up the discovery if the apps is found


/*

\par About rewriting this using the additionnal js knowledge you got
- use xdomrpc_t to probe the neoip-apps
- rename it appsdetect_t
- do the xdomrpc_t in series, aka do one, if it failed, do the other etc...
- have a callback to notify the result
- have a timeout for the whole detection to expire after a while
- neoip.appsdetect_t("neoip_oload", 4550, 4560)
- this require to port the c++ http_appsdetect_t on top of xmlrpc_net. 
  - a good thing as it is more generic
- ok in fact this will be a class with a complete reimplementation :)

*/


// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};

// produce the html to probe a local port and see there is a webserver providing
// the proper well-known javascript
neoip.build_port_probe = function(daemon_name, probe_port)
{
	var	html_str = "";
	// build the html requite to probe this local_port
	html_str	+= "<script src=\"http://127.0.0.1:" + probe_port + "/"
				+ "neoip_" + daemon_name + "_local_ctor.js\"<\/script>\n";
	html_str	+= "<script>"
				+ "if( neoip." + daemon_name + "_outter_uri == \"\" && neoip." + daemon_name + "_present ){" 
				+ 	"neoip." + daemon_name + "_outter_uri	=\"http://127.0.0.1:" + probe_port + "\""
				+ "}"
				+ "<\/script>";
	// return the just built html_str
	return html_str;
}

// return the require html to autodiscover a local daemon exporting the
// url http://127.0.0.1:port/neoip_(daemon_name)_local_ctor.js
// - port is between first_port and last_port, included
neoip.discover_daemon	= function(daemon_name, first_port, last_port)
{
	var	html_str = "";
	// build the html to init the state variable *_hostport and *_found
	html_str	+= "<script>"
				+ "neoip." + daemon_name + "_outter_uri = \"\";"
				+ "neoip." + daemon_name + "_present = false;"
				+ "<\/script>";
	// build the html required to autodiscover a local daemon
	for(var cur_port = first_port; cur_port <= last_port; cur_port++){
		html_str	+= neoip.build_port_probe(daemon_name, cur_port);
	}
	// write the just built html
	document.write(html_str)
}


