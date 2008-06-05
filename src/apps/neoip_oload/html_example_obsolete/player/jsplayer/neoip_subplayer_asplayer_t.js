/*! \file
    \brief Definition of the subplayer_asplayer_t

\par Brief Description
This is a subplayer using my own flash object.

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
neoip.subplayer_asplayer_t = function(p_plugin_id)
{
	// copy the parameters
	this.m_plugin_htmlid	= p_plugin_id;
	// zero some fields
	this.m_callback		= null;
	this.m_trackidx		= null;
	this.m_start_time	= null;
	this.m_current_state	= null;
	this.m_plugin_abstime	= null;
	// register this subplayer_asplayer_t to the neoip_subplayer_asplayer_cb_arr
	neoip_subplayer_asplayer_cb_doreg(this);
}

/** \brief destructor of the object
 */
neoip.subplayer_asplayer_t.prototype.destructor = function()
{
	// unregister this subplayer_asplayer_t to the neoip_subplayer_asplayer_cb_arr
	neoip_subplayer_asplayer_cb_unreg(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief get the callback
 */
neoip.subplayer_asplayer_t.prototype.callback = function(p_callback)
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
neoip.subplayer_asplayer_t.prototype.current_state	= function()
{
	return this.m_current_state;
}

/** \brief Return the current track_pos for the subplayer
 */
neoip.subplayer_asplayer_t.prototype.current_track_pos	= function()
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
neoip.subplayer_asplayer_t.prototype.track_reltime	= function()
{
	// if this.m_reltime_begdate is not null, report the track_reltime according to it
	if( this.m_reltime_begdate )	return (new Date()).getTime() - this.m_reltime_begdate;

	// NOTE: if this.m_reltime_begdate is null, report according to the plugin value

	// if no absolute_time has been set, return null
	if( this.m_plugin_abstime == null )	return null;
	// return the time relative this the begining of this track
	return this.m_plugin_abstime - this.m_start_time;
}

// declare all the methods to read the variables
neoip.subplayer_asplayer_t.prototype.trackidx	= function(){ return this.m_trackidx;	}
neoip.subplayer_asplayer_t.prototype.start_time	= function(){ return this.m_start_time;	}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build the player
 */
neoip.subplayer_asplayer_t.prototype.build_objembed = function(p_container_id, p_width, p_height)
{
	var so			= new SWFObject("neoip_asplayer.swf", this.m_plugin_htmlid
					, p_width.toString(), p_height.toString()
					, "7", "#000000", true);
	// put the parameter to the flash plugin itself
	so.addParam("allowScriptAccess"	, "always");


	var allowfullscreen	= true;
	so.addParam("allowfullscreen"	, allowfullscreen ? "true" : "false");
	// mirror addParam "allowfullscreen" because unable to get addParam from as
	so.addVariable("allowfullscreen", allowfullscreen ? "true" : "false");  
	// put the variable for the flash program
	so.addVariable("jscallback_str"	, "neoip_subplayer_asplayer_plugin_cb");
	so.addVariable("jscallback_key"	, neoip_subplayer_asplayer_cb_build_key(this));
	// write the result in the html
      	so.write(p_container_id);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			function from player_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Play the neoip.playlist_t.track_t with a delay_within
 */
neoip.subplayer_asplayer_t.prototype.play	= function(p_track_pos, p_player)
{
	var playlist_track	= p_player.playlist().track_at(p_track_pos.trackidx);
	var plugin_track_arg	= {};
	var cooked_uri_arg	= {};
	// copy the parameters
	this.m_trackidx		= p_track_pos.trackidx;
	this.m_start_time	= playlist_track.start_time();

	// declare the variable to contain the time to seek in
	var seek_time	= null;
	
	// if the playlist_track.is_stream, then this.m_reltime_begdate is present, else is null
	// - this is used to determine the track_reltime. 
	// - in stream, the plugin track_reltime is not valid
	if( playlist_track.is_stream() ){
		seek_time		= null;
		this.m_reltime_begdate	= (new Date()).getTime() - p_track_pos.track_reltime;
	}else if( playlist_track.is_static() ){
		seek_time		= this.m_start_time + p_track_pos.track_reltime;
		this.m_reltime_begdate	= null;
	}else{
		console.assert(false);
	}


if(1){	/*************** to enable or disable net_ratelim in flash	*******/
	// setup flash net_ratelim - if playlist_track.is_static and p_player.has_oload
	// TODO apparently this trigger a bug where no data get delivered by neoip-oload
	// - it is shown by web player trying to reconnect in the middle of the track 
	if( playlist_track.is_static() && p_player.has_oload() ){
		// - net_ratelim_t in neoip_asplayer.swf will issue the first xmlrpc
		//   set_httpo_maxrate immediatly after starting the NetStream.
		// - by the time for this set_httpo_maxrate to reach neoip-oload,
		//   neoip-oload will already have started downloading at full speed
		// - in order to limit this waste of bandwidth, an initial httpo_maxrate
		//   is set to a minimal value after a given threshold. 
		//   - 1k and not 0k because a bug in flash-player which is unable to close
		//     connection which doesnt send anything. details in neoip_net_ratelim.as 
		// - determination of the threshold
		//   - if ['oload']['static_filelen'] is not present, default to "500k"
		//   - it it is present, compute an average rate for the track
		//     and put the threshold at 5.0*1000 msec
		//   - this algo works IIF the duration of playlist_track is for the 
		//     whole file. aka IIF start_time is 0
		cooked_uri_arg['httpo_maxrate']		= "1k";
		if( playlist_track.static_filelen() != null && playlist_track.start_time()==0){
			var buffer_msec	= 5.0*1000;	// TODO this one should be tunable
			var filelen	= playlist_track.static_filelen();
			var duration	= playlist_track.duration();
			var threshold	= buffer_msec * (filelen / duration);
			cooked_uri_arg['httpo_maxrate_thres']	= Math.round(threshold);
		}else{
			cooked_uri_arg['httpo_maxrate_thres']	= "500k";
		}
		
		// add a "httpo_full_id" outter var in the cooked_uri for the track_uri
		cooked_uri_arg['httpo_full_id']= neoip.core.build_nonce_str(16);
		// tell the plugin to handle this track with net_ratelim
		plugin_track_arg['net_ratelim']= {
					"key"	: cooked_uri_arg['httpo_full_id'],
					"uri"	: p_player.m_outter_uri_arr['oload'] };
	}
}

	// get the track_uri
	var track_uri			= playlist_track.cooked_uri(cooked_uri_arg);
	// compute the track_arg to pass to the plugin
	plugin_track_arg['content_url']	= track_uri;	


	console.info("====================================");
	console.info("track_uri=" + track_uri + "seek_time=" + seek_time);
	console.dir(p_track_pos);
	console.info("====================================");


	if( seek_time > 0 ){
		// sanity check - if seek_time, playlist_track MUST be may_startinmid()
		console.assert( playlist_track.may_startinmid() );
		// sanity check - if track_abstime is > 0, playlist_track MUST be static
		console.assert( playlist_track.is_static() );
		
		// start updating track_arg to get the proper start_time
		plugin_track_arg['start_time']		= seek_time/1000;
		plugin_track_arg['rangereq_type']	= "flv";
		// determine the rangereq_flv_var according to the presence of neoip-oload
		if( p_player.has_oload() ){
			var flv_mdata_info	= playlist_track.extension()['flv_mdata_info'];
			// if it goes thru neoip-oload, use 'pos' by default
			plugin_track_arg['rangereq_flv_var']	= "pos";
			plugin_track_arg['flv_mdata_uri']	= playlist_track.flv_mdata_uri();
			if( flv_mdata_info ){
				plugin_track_arg['flv_mdata_type']	= flv_mdata_info['type'];
			}else{
				plugin_track_arg['flv_mdata_type']	= "internal";
			}
		}else{
			// if it goes directly to the location, use the proper meta
			// - TODO this name is way too long
			var varname	= playlist_track.meta()['location_rangereq_byuri_varname'];
			// sanity check - the varname MUST be specified
			console.assert( varname );
			// set the value into track_arg
			plugin_track_arg['rangereq_flv_var']	= varname;
		}
	}
	
	console.info("startplaying with: ");
	console.dir(plugin_track_arg);
	var plugin	= document.getElementById(this.m_plugin_htmlid)
	// sanity check - the plugin playlist MUST be empty
	console.assert( plugin.track_count() == 0 );	
	// add this track to the plugin
	plugin.track_add(plugin_track_arg, 0);
	// set the pluging as not playlist_loop
	// - it is never supposed to loop on its playlist
	plugin.playlist_loop(false);
	// start playing
	plugin.playing_start();
}

/** \brief Stop playing
 */
neoip.subplayer_asplayer_t.prototype.stop	= function()
{
	var	plugin	= document.getElementById(this.m_plugin_htmlid)
	
	// stop playing
	plugin.playing_stop();
	// remove the current track from the plugin playlist
	plugin.track_del(0);
	// zero some fields
	this.m_trackidx		= null;
	this.m_start_time	= null;
	this.m_current_state	= null;
	this.m_plugin_abstime	= null;
	this.m_reltime_begdate	= null;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			plugin_cb
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief The flash plugin event callback
 */
neoip.subplayer_asplayer_t.prototype.plugin_cb	= function(event_type, arg)
{
	// log to debug
	//console.info("event_type=" + event_type + " arg=%o", arg);

	// parse the event_type 
	if( event_type == "state_change" ){
		var	old_state	= this.m_current_state;
		var	new_state	= arg.new_state;
		// if the new state is different than m_current_state, update it
		// - TODO unsure what to do when the track are switching from one another
		if( new_state != this.m_current_state ){
			// update the m_current_state
			this.m_current_state	= new_state;
			// notify the caller if a callback is present
			var cb_arg	= { old_state: old_state, new_state: new_state };
			this.notify_callback("changed_state", cb_arg);
		}
	}else if( event_type == "current_time"){
		// update the m_plugin_abstime
		this.m_plugin_abstime	= arg.time*1000;
		// notify the caller if a callback is present
		var cb_arg	= { new_time: this.track_reltime()};
		this.notify_callback("new_time", cb_arg);
	}else{	// simply forward the event
		this.notify_callback(event_type, arg);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			flv_kframe_find	service
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief find a flv kframe byte offset
 * 
 * - NOTE: this function is specific to this flash player
 */
neoip.subplayer_asplayer_t.prototype.flv_kframe_find
				= function(wished_delay, mdata_type, src_uri, userptr)
{
	var	plugin	= document.getElementById(this.m_plugin_htmlid)
	// forward the function
	plugin.flv_kframe_find(wished_delay/1000, mdata_type, src_uri, userptr);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Main callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief The flash plugin event callback
 */
neoip.subplayer_asplayer_t.prototype.notify_callback	= function(event_type, arg)
{
	// log to debug
	//console.info("event_type=" + event_type + " arg=" + arg);
	
	// if no callback is defined, do nothing
	if( this.m_callback == null )	return;
	// forward the event to the callback
	this.m_callback(event_type, arg);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Global callback for subplayer flash_t
// - this global is dirty but this is a part of the flash plugin
// - It include a callback registration for each instance of subplayer_asplayer_t
//   to register and thus receives the event for itself.
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

var neoip_subplayer_asplayer_cb_arr	= new Array();

function neoip_subplayer_asplayer_cb_build_key(subplayer)
{
	return "tag_" + subplayer.m_plugin_htmlid;
}
function neoip_subplayer_asplayer_cb_doreg(subplayer)
{
	var key	= neoip_subplayer_asplayer_cb_build_key(subplayer);
	// sanity check - this key MUST NOT already exists
	console.assert( !neoip_subplayer_asplayer_cb_arr[key] );
	// register this subplayer into the neoip_subplayer_asplayer_cb_arr
	neoip_subplayer_asplayer_cb_arr[key]	= subplayer;
}

function neoip_subplayer_asplayer_cb_unreg(subplayer)
{
	var key	= neoip_subplayer_asplayer_cb_build_key(subplayer);
	// sanity check - this key MUST already exists
	console.assert( neoip_subplayer_asplayer_cb_arr[key] );
	// delete it from the array - aka make it null
	// - TODO to use a splice instead
	delete neoip_subplayer_asplayer_cb_arr[key];
}

function neoip_subplayer_asplayer_plugin_cb(key, event_type, arg)
{
	// log to debug
	//console.info("key="+ key + " event_type=" + event_type + " arg=%o", arg);
	// get the subplayer_t matching this key
	var subplayer	= neoip_subplayer_asplayer_cb_arr[key];
	// sanity check - the subplayer MUST exist
	console.assert( subplayer );
	// notify the subplayer of the event
	subplayer.plugin_cb(event_type, arg);
};


