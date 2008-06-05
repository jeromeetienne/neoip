/*! \file
    \brief Definition of the subplayer_jwplayer_t


OBSOLETE!!!!
- this is obsoleted by my own flash player
- this has been coded when i wasnt able to code flash


\par Notes
- for doc about the jwplayer flash:
 - http://www.jeroenwijering.com/?track=JW_FLV_Player

*/

// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the object
 */
neoip.subplayer_jwplayer_t = function(p_plugin_id)
{
	// copy the parameters
	this.m_plugin_id	= p_plugin_id;
	// zero some fields
	this.m_callback		= null;
	this.m_trackidx		= null;
	this.m_start_time	= null;
	this.m_current_state	= null;
	this.m_absolute_time	= null;
	// register this subplayer_jwplayer_t to the neoip_subplayer_jwplayer_cb_arr
	neoip_subplayer_jwplayer_cb_doreg(this);
}

/** \brief destructor of the object
 */
neoip.subplayer_jwplayer_t.prototype.destructor = function()
{
	// unregister this subplayer_jwplayer_t to the neoip_subplayer_jwplayer_cb_arr
	neoip_subplayer_jwplayer_cb_unreg(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief get the callback
 */
neoip.subplayer_jwplayer_t.prototype.callback = function(p_callback)
{
	// copy the parameter
	this.m_callback	= p_callback;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the current state
 */
neoip.subplayer_jwplayer_t.prototype.current_state	= function()
{
	return this.m_current_state;
}

/** \brief Return the current track_pos for the subplayer
 */
neoip.subplayer_jwplayer_t.prototype.current_track_pos	= function()
{
	// if no absolute_time has been
	if( this.m_absolute_time == null )	return null;
	// return the result
	var	track_pos	= {};
	track_pos.trackidx	= this.m_trackidx;
	track_pos.track_reltime	= this.m_absolute_time - this.m_start_time;
	return	track_pos;
}

/** \brief Return the time relative to the begining of this track
 */
neoip.subplayer_jwplayer_t.prototype.track_reltime	= function()
{
	// if no absolute_time has been set, return null
	if( this.m_absolute_time == null )	return null;
	// return the time relative this the begining of this track
	return this.m_absolute_time - this.m_start_time;
}

// declare all the methods to read the variables
neoip.subplayer_jwplayer_t.prototype.trackidx	= function(){ return this.m_trackidx;	}
neoip.subplayer_jwplayer_t.prototype.start_time	= function(){ return this.m_start_time;	}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build the player
 */
neoip.subplayer_jwplayer_t.prototype.build	= function(p_container_id, p_width, p_height)
{
	var	so	= new SWFObject("../obsolete/jwplayer/flvplayer.swf", this.m_plugin_id
					, p_width.toString(), p_height.toString()
					, "7", "#000000", true);

	so.addParam("allowScriptAccess"	, "always");
	so.addParam("allowfullscreen"	,"true");
	// the javascriptid is a key notified in the global callback
	// - this is used to forward the event back to this object
	so.addVariable("javascriptid"	,"tag_" + this.m_plugin_id);
	
	so.addVariable("enablejs"	,"true");
	so.addVariable("autostart"	,"false");
	// TODO this lighttpd is only for neoip-oload ... 
	// - what about neoip-casto ? what happen if there is start_time or track_reltime
	so.addVariable("streamscript"	, 'lighttpd');
	
	// NOTE: to remove the status bar
	so.addVariable("displayheight"	, p_height.toString());

	// to fit the video in the viewport	
	so.addVariable('overstretch'	, 'true');
	
	// TODO what does this do ?
	so.addVariable("usekeys"	, "false");
	so.addVariable("showicons"	, "false");
	
      	so.write(p_container_id);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			function from player_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Play the neoip.playlist_t.track_t with a delay_within
 */
neoip.subplayer_jwplayer_t.prototype.play	= function(p_url, p_trackidx, p_start_time
									, p_track_reltime)
{
	// copy the parameters
	this.m_trackidx		= p_trackidx;
	this.m_start_time	= p_start_time;

	var	plugin	= document.getElementById(this.m_plugin_id)

	console.info("play " + p_url + " starting at " + (this.m_start_time + p_track_reltime));

	plugin.addItem( { file: p_url, start: this.m_start_time + p_track_reltime }, 0);
	plugin.sendEvent('playtrack',0);
}

/** \brief Stop playing
 */
neoip.subplayer_jwplayer_t.prototype.stop	= function()
{
	var	plugin	= document.getElementById(this.m_plugin_id)
	
	plugin.sendEvent("stop");
	plugin.removeItem(0);
	// zero some fields
	this.m_trackidx		= null;
	this.m_start_time	= null;
	this.m_current_state	= null;
	this.m_absolute_time	= null;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			plugin_cb
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief The jwplayer plugin event callback
 */
neoip.subplayer_jwplayer_t.prototype.plugin_cb	= function(event_type, arg1, arg2)
{
	// log to debug
	//console.info("event_type=" + event_type + " arg1=" + arg1 + " arg2=" + arg2);
	// parse the event_type 
	if( event_type == "state" ){
		var	old_state	= this.m_current_state;
		var	new_state;
		// convert the callback arg into string
		if( arg1 == 0 )		new_state	= "connecting";
		else if( arg1 == 1 )	new_state	= "buffering";
		else if( arg1 == 2 )	new_state	= "playing";
		else 			new_state	= "unknown";
		// if the new state is different than m_current_state, update it
		if( new_state != this.m_current_state ){
			// update the m_current_state
			this.m_current_state	= new_state;
			// notify the caller if a callback is present
			if( this.m_callback ){
				var	arg = { old_state: old_state, new_state: new_state };
				this.m_callback("changed_state", arg);
			}
		}
	}else if( event_type == "time"){
		// update the m_absolute_time
		this.m_absolute_time	= arg1 + (arg2/100);
		
		// notify the caller if a callback is present
		if( this.m_callback ){
			var	arg = { new_time: this.track_reltime()};
			this.m_callback("new_time", arg);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Global callback for subplayer jwplayer_t
// - this global is dirty but this is a part of the jwplayer plugin
// - It include a callback registration for each instance of subplayer_jwplayer_t
//   to register and thus receives the event for itself.
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

var	neoip_subplayer_jwplayer_cb_arr	= new Array();

function neoip_subplayer_jwplayer_cb_doreg(subplayer)
{
	var	key	= "tag_" + subplayer.m_plugin_id;
	// sanity check - this key MUST NOT already exists
	console.assert( !neoip_subplayer_jwplayer_cb_arr[key] );
	// register this subplayer into the neoip_subplayer_jwplayer_cb_arr
	neoip_subplayer_jwplayer_cb_arr[key]	= subplayer;
}

function neoip_subplayer_jwplayer_cb_unreg(subplayer)
{
	// sanity check - this key MUST already exists
	console.assert( neoip_subplayer_jwplayer_cb_arr[key] );
	// delete it from the array - aka make it null
	delete neoip_subplayer_jwplayer_cb_arr[key];
}

function getUpdate(event_type, arg1, arg2, javascriptid)
{
	// log to debug
	//console.info("event_type=" + event_type + " arg1=" + arg1 + " arg2=" + arg2 + " javascriptid=" + javascriptid);
	// get the subplayer for this pid
	var	subplayer	= neoip_subplayer_jwplayer_cb_arr[javascriptid];
	// sanity check - this key MUST already exists
	console.assert( neoip_subplayer_jwplayer_cb_arr[javascriptid] );
	// notify the subplayer of the event
	subplayer.plugin_cb(event_type, arg1, arg2)
};


