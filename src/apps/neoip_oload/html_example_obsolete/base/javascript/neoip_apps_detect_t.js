/*! \file
    \brief Definition of the neoip.apps_detect_t

\par Brief Description
This object implement a autodetection for the neoip-apps.
- it does the xdomrpc_t in series, aka if the second is done IFF the first failed
- it support a expire_timeout, aka if the whole detection takes more than a given
  delay, give up and notify faillure.

\par Implementation notes
- the callback provide a simple information string on the result of the detection
  - "found" if the neoip-apps has been found
  - "absent" if the neoip-apps has not been found after probing all ports
  - "expired" if the neoip-apps has not been found after the expiration delay
- The results are stored in neoip_apps_detect_arr and MUST be accessed by
  - neoip.apps_present("oload") return true if the apps has been previously 
    found, false otherwise
  - neoip.outter_uri("oload") return the webdetect_uri of this neoip-apps
    - assumes it is present
  - neoip.apps_version("oload") return a string containing the version of this neoip-apps
    - assumes it is present

*/


// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
neoip.apps_detect_t = function(p_suffix_name, p_first_port, p_last_port, p_callback)
{
	// copy the parameter
	this.m_callback		= p_callback;
	this.m_suffix_name	= p_suffix_name;
	this.m_first_port	= p_first_port;
	this.m_last_port	= p_last_port;

	// zero some fields
	this.m_probe_xdomrpc	= null;

	// launch the m_expire_timeout	
	this.m_expire_delay	= 10.0;			// TODO make this tunable
	this.m_expire_timeout	= setTimeout(function(thisObj){thisObj._expire_timeout_cb(); }
						, this.m_expire_delay * 1000, this);

	// launch the first probe
	this.m_current_port	= this.m_first_port;
	this._launch_probe();
}

/** \brief Destructor of player_t
 */
neoip.apps_detect_t.prototype.destructor = function()
{
	// stop the probe_xdomrpc if needed
	if( this.m_probe_xdomrpc ){
		this.m_probe_xdomrpc.destructor();
		this.m_probe_xdomrpc	= null;
	}

	// stop the expire_timeout if needed
	if( this.m_expire_timeout ){
		clearTimeout( this.m_expire_timeout );
		this.m_expire_timeout	= null;
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// declare all the methods to read the variables
neoip.apps_detect_t.prototype.suffix_name	= function(){ return this.m_suffix_name;}
neoip.apps_detect_t.prototype.first_port	= function(){ return this.m_first_port;	}
neoip.apps_detect_t.prototype.last_port		= function(){ return this.m_last_port;	}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch a probe on this.m_current_port
 */
neoip.apps_detect_t.prototype._launch_probe	= function()
{
	// sanity check - this.m_probe_xdomrpc MUST be null at this point
	console.assert( this.m_probe_xdomrpc == null );
	// log to debug
	//console.info("launch probe on " + this.m_current_port);

	// launch the xdomrpc_t on this.m_current_port
	var	rpc_uri	= "http://127.0.0.1:" + this.m_current_port + "/"
				+ "neoip_" + this.m_suffix_name + "_appdetect_jsrest.js";
	this.m_probe_xdomrpc	= new neoip.xdomrpc_t(rpc_uri
					, neoip.xdomrpc_cb_t(this._xdomrpc_cb, this)
					, "probe_apps"
					);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			xdomrpc_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief xdomrpc_t callback
 */
neoip.apps_detect_t.prototype._xdomrpc_cb = function(notifier_obj, userptr, fault, returned_val)
{
	// log to debug
	//console.info("enter fault=" + fault + " returned_val=" + returned_val);
	// destructor for the m_probe_xdomrpc
	this.m_probe_xdomrpc.destructor();
	this.m_probe_xdomrpc	= null;

	// if there is no error, notify the caller of a success
	if( fault == null ){
		// put the detected information in neoip_apps_detect_arr
		var	obj	= {};
		obj.outter_uri	= "http://127.0.0.1:" + this.m_current_port;
		obj.version	= returned_val;
		obj.present	= true;
		neoip_apps_detect_arr[this.m_suffix_name]	= obj;
		// notify the caller of the completion
		if( this.m_callback )	this.m_callback("found");
		return;
	}

	// if all the port has been probed, notify the caller of an faillure
	if( this.m_current_port == this.m_last_port ){
		// mark this apps as undetected
		var	obj	= { "present"	: false };
		neoip_apps_detect_arr[this.m_suffix_name]	= obj;
		// notify the caller of the completion
		if( this.m_callback )	this.m_callback("absent");
		return;
	}
	
	// increment the this.m_current_port
	this.m_current_port++;
	// launch probe for it
	this._launch_probe();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			m_expire_timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback for the m_expire_timeout
 */
neoip.apps_detect_t.prototype._expire_timeout_cb	= function()
{
	// mark this apps as undetected
	var	obj	= { "present"	: false };
	neoip_apps_detect_arr[this.m_suffix_name]	= obj;
	// notify the caller of the completion
	if( this.m_callback )	this.m_callback("expired after " + this.m_expire_delay + "-sec");
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			apps_detect_cb_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor for a apps_detect_cb_t
 *
 * - see http://www.dustindiaz.com/javascript-curry/ for principle description 
 */
neoip.apps_detect_cb_t	= function(fct, p_scope, userptr) 
{
	var	scope	= p_scope || window;
	return	function(result_str) {
			fct.call(scope, this, userptr, result_str);
		};
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Storage of the result
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

var	neoip_apps_detect_arr	= new Array();

/** \brief Return true if the neoip-apps for this suffix_name is present, false otherwise
 */
neoip.apps_present	= function(suffix_name)
{
	// if neoip_apps_detect_arr doesnt contain suffix_name, return false
	// - as it means it havent even been tested
	if( neoip_apps_detect_arr[suffix_name] == null )	return false;
	// else return the present field
	return neoip_apps_detect_arr[suffix_name].present;
}

/** \brief Return the version for this suffix_name 
 *
 * - assume that the neoip-apps has been previously detected
 */
neoip.apps_version	= function(suffix_name)
{
	// sanity check - the apps MUST be present
	console.assert( neoip.apps_present(suffix_name) );
	// return the version for this apps
	return neoip_apps_detect_arr[suffix_name].version;
}

/** \brief Return the outter_uri for this suffix_name 
 *
 * - assume that the neoip-apps has been previously detected
 */
neoip.outter_uri	= function(suffix_name)
{
	// sanity check - the apps MUST be present
	console.assert( neoip.apps_present(suffix_name) );
	// return the outter_uri for this apps
	return neoip_apps_detect_arr[suffix_name].outter_uri;
}
