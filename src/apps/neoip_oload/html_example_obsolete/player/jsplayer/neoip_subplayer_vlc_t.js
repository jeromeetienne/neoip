/*! \file
    \brief Definition of the subplayer_vlc_t

\par Notes
- for doc about the vlc mozilla plugin:
 - http://www.videolan.org/doc/play-howto/en/ch04.html#id293992

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
neoip.subplayer_vlc_t = function(p_plugin_id)
{
	// copy the parameters
	this.m_plugin_id	= p_plugin_id;
	// zero some fields
	this.m_callback		= null;
	this.m_trackidx		= null;
	this.m_start_time	= null;
	this.m_current_state	= null;
	this.m_plugin_abstime	= null;

	// some value for the event feature
	// - TODO make those values tunable
	// - TODO experiment to determine what are good defaults
	this.m_event_period	= 1*1000;
	this.m_event_timeout	= null;
}

/** \brief destructor of the object
 */
neoip.subplayer_vlc_t.prototype.destructor = function()
{
	// TODO to destruct the object
	// - especially the timeout
	// - and likely other thing
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief get the callback
 */
neoip.subplayer_vlc_t.prototype.callback = function(p_callback)
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
neoip.subplayer_vlc_t.prototype.current_state	= function()
{
	return this.m_current_state;
}

/** \brief Return the current track_pos for the subplayer
 */
neoip.subplayer_vlc_t.prototype.current_track_pos	= function()
{
	// build the track_pos
	var track_pos	= {	"trackidx"	: this.m_trackidx,
			 	"track_reltime"	: this.track_reltime() };
	// if the track_reltime can not be established, return null
	if( track_pos.track_reltime == null )	return null;
	// return the track_pos
	return track_pos;
}

/** \brief Return the time relative to the begining of this track
 */
neoip.subplayer_vlc_t.prototype.track_reltime	= function()
{
	// if this.m_reltime_begdate is not null, report the track_reltime according to it
	if( this.m_reltime_begdate ){
		var present_date	= new Date();
		return present_date - this.m_reltime_begdate;
	}

	// NOTE: if this.m_reltime_begdate is null, report according to the plugin value
	
	// if no absolute_time has been set, return null
	if( this.m_plugin_abstime == null )	return null;
	// if the current_state is not playing, return null
	if( this.m_current_state != "playing" )	return null;
	// return the time relative this the begining of this track
	return this.m_plugin_abstime - this.m_start_time;
}
// declare all the methods to read the variables
neoip.subplayer_vlc_t.prototype.trackidx	= function(){ return this.m_trackidx;		}
neoip.subplayer_vlc_t.prototype.start_time	= function(){ return this.m_start_time;		}
neoip.subplayer_vlc_t.prototype.current_state	= function(){ return this.m_current_state;	}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build the player
 */
neoip.subplayer_vlc_t.prototype.build_objembed	= function(p_container_id, p_width, p_height)
{
	var	html_str = "";
	// build the html object string
	html_str	+= '<object type="application/x-vlc-plugin"'
	html_str	+= 	' pluginspage="http://www.videolan.org/"'
	html_str	+= 	' version="VideoLAN.VLCPlugin.2"'
	html_str	+= ' width="'	+ p_width		+ '"';
	html_str	+= ' height="'	+ p_height		+ '"';
	html_str	+= ' id="'	+ this.m_plugin_id	+ '"';
	html_str	+= '>'
	html_str	+= '</object>'
	// log to debug
	//console.log(html_str)
	// write the html object string to the dest_id
	document.getElementById(p_container_id).innerHTML = html_str;
}


/** \brief Play the neoip.playlist_t.track_t with a delay_within
 */
neoip.subplayer_vlc_t.prototype.play	= function(p_track_pos, p_player)
{
	var playlist_track	= p_player.playlist().track_at(p_track_pos.trackidx);
	var track_uri		= playlist_track.cooked_uri();

	// sanity check - track_data MUST be non null
// TODO syntax error in that
//	console.assert(track instanceof neoip.playlist_t.track_t);
	
	// stop the current playing if any
	this.stop();
	

	// copy the parameters
	this.m_trackidx		= p_track_pos.trackidx;
	this.m_start_time	= playlist_track.start_time();

	
	// if the playlist_track.is_stream, then this.m_reltime_begdate is present, else is null
	// - this is used to determine the track_reltime. 
	// - in stream, the plugin track_reltime is not valid
	if( playlist_track.is_stream() )	this.m_reltime_begdate	= new Date();
	else					this.m_reltime_begdate	= null;

	// relaunch a new one
	var opt		= new Array();
	opt.push(":start-time=" + (this.m_start_time + p_track_pos.track_reltime));
// to reduce the delay of input caching by vlc in ms
	opt.push(":http-caching=50");
// to force vlc to fill the output window
//	opt.push(":crop");
	var plugin	= document.getElementById(this.m_plugin_id);
	plugin.playlist.add(track_uri, null, opt);
	plugin.playlist.play();

	// start the event_timeout
	this.m_event_timeout= setTimeout(function(thisObj){thisObj.event_timeout_cb(); }
							, this.m_event_period, this);
}

/** \brief Stop playing
 */
neoip.subplayer_vlc_t.prototype.stop	= function()
{
	var 	plugin	= document.getElementById(this.m_plugin_id);
	// zero some fields
	this.m_trackidx		= null;
	this.m_start_time	= null;
	this.m_current_state	= null;
	this.m_plugin_abstime	= null;
	this.m_reltime_begdate	= null;
	// stop the event_timeout if needed
	if( this.m_event_timeout ){
		clearTimeout( this.m_event_timeout );
		this.m_event_timeout	= null;
	}
	
	try {
		// if plugin is currently playing, stop it
		if( plugin.playlist.isPlaying )		plugin.playlist.stop();
		// if the plugin.playlist is is not empty, clear it
		if( plugin.playlist.tracks.count > 0 )	plugin.playlist.clear();
	}catch(e){
		console.log("exception=" + e);
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			event_timeout_cb
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief The  event_timeout_cb
 */
neoip.subplayer_vlc_t.prototype.event_timeout_cb	= function()
{
	var 	plugin	= document.getElementById(this.m_plugin_id);

	/*************** handle the m_current_state update	***************/
	var	old_state	= this.m_current_state;
	var	new_state;
	if( plugin.input.state == 1 )		new_state	= "connecting";
	else if( plugin.input.state == 2 )	new_state	= "buffering";
	else if( plugin.input.state == 3 )	new_state	= "playing";
	else					new_state	= "unknown";
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

	// if not playing, what to do !?!?!
	// - reset the value ?!?!
	if( !plugin.playlist.isPlaying )	return;
	

	/*************** handle the m_plugin_abstime update	***************/
	try {	// get the abs_pos within this track
		var abs_pos	= plugin.input.time;
	}catch(e){
		console.info("exception=" + e);
		return;
	}
		
	// WORKAROUND: sometime vlc plugin report an abs_pos of 0 even with a start_time > 0
	// - so this workaround this bug
	if( abs_pos < this.m_start_time )	abs_pos	= this.m_start_time;
	// update the m_plugin_abstime
	this.m_plugin_abstime	= abs_pos;
	// notify the caller if a callback is present
	if( this.m_callback ){
		var	arg = { new_time: this.track_reltime()};
		this.m_callback("new_time", arg);
	}
	
	/*************** relaunch event_timeout	*******************************/
	this.m_event_timeout= setTimeout(function(thisObj){thisObj.event_timeout_cb(); }
							, this.m_event_period, this);
}



