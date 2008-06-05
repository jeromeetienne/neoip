/*

OBSOLETE!!!!
- this is obsoleted by my own flash player
- this has been coded when i wasnt able to code flash



 * \brief this script implement a subplayer_flowplayer_t
 *
 * for doc about the flowplayer flash:
 * - http://flowplayer.org/
 *
 * \par TODO - make the track_reltime working in play()
 * - the start lighttpd trick is not working
 * - likely a silly bug in the flowplayer configuration
 * - i got the source of it and i am able to compile it
 * - but currently i dont understand anything about flash and even less flowplayer
 *   source... so i cant fix
 * - moreover the subplayer_jwplayer_t seems to work well
 *
 * \par TODO
 * - it doesnt contains the subplayer_cb_t thing
 */

// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};


/** \brief constructor of the playlist_t
 */
neoip.subplayer_flowplayer_t = function(p_plugin_id)
{
	// copy the parameters
	this.m_plugin_id	= p_plugin_id;
	// zero some fields
	this.m_trackidx		= null;
	this.m_start_time	= null;
}

/** \brief Build the player
 */
neoip.subplayer_flowplayer_t.prototype.build	= function(p_container_id, p_width, p_height)
{
	var	swf_obj = new SWFObject("../obsolete/flowplayer/FlowPlayer.swf", this.m_plugin_id
					, p_width.toString(), p_height.toString()
					, "7", "#000000", true);
	// need this next line for local testing, it's optional if your swf is on the same domain as your html page
	swf_obj.addParam("allowScriptAccess", "always");
	swf_obj.addParam("flashVars", "config={configInject: true}");
      	swf_obj.write(p_container_id);
}

/** \brief Play the neoip.playlist_t.track_t with a delay_within
 */
neoip.subplayer_flowplayer_t.prototype.play	= function(p_url, p_trackidx, p_start_time, p_track_reltime)
{
	// copy the parameters
	this.m_trackidx		= p_trackidx;
	this.m_start_time	= p_start_time;
	
	var	plugin	= document.getElementById(this.m_plugin_id)
	// TODO this conf in in play/stop... centralize it
	var 	conf = {
			startingBufferLength	: 0,
			loop			: false,
			showPlayList		: false,
			hideControls		: true,
			showPlayListButtons	: false,
			streamingServer		: 'lighttpd'
		};
	conf.playList = [ {
			 url			: p_url
			 } ];
	// TODO the start: doesnt work.... 
	// - apparently this is the start: variable which is ignored
	// - if the controls button is visible, i can seek thru it with the lighttpd ?start=
	conf.autoBuffering = true;
	conf.autoPlay = true;
	plugin.setConfig(conf);
}

/** \brief Stop playing
 */
neoip.subplayer_flowplayer_t.prototype.stop	= function()
{
	var	plugin	= document.getElementById(this.m_plugin_id)
	var 	conf = {
			bufferLength: 0,
			loop: false,
			showPlayList: false,
			hideControls: true,
			showPlayListButtons: false
		};
	conf.playList		= [];
	conf.autoBuffering	= false;
	conf.autoPlay		= false;
	plugin.setConfig(conf);
}

/** \brief Return the current state
 */
neoip.subplayer_flowplayer_t.prototype.current_state	= function()
{
	// if m_trackidx IS NOT set, return "stopped"
	if( this.m_trackidx == null )	return "stopped";
	
	// TODO this function is a stub
	// - flowplayer return much better stuff... with callback
	
	// if plugin is playing, return unknown
	if( plugin.getIsPlaying() )	return "playing";
	// return unknown
	return "unknown";
}

/** \brief Return the current track_pos for the subplayer
 */
neoip.subplayer_flowplayer_t.prototype.current_track_pos	= function()
{
	var	plugin	= document.getElementById(this.m_plugin_id)
	// if plugin is NOT playing, return null
	// - TODO not sure it is required
	if( !plugin.getIsPlaying() )	return null;
	
	// get the abs_pos
	// - TODO in fact i dunno if it is the abs_pos returned. but i cant test
	//   as the start doesnt work for now
	var	abs_pos		= plugin.getTime()
	// convert the abs_pos into a track_reltime
	var	track_reltime	= abs_pos - this.m_start_time;
	// return the result
	return {"trackidx"	: this.m_trackidx,
		"track_reltime"	: track_reltime
		}
}

// declare all the methods to read the variables
neoip.subplayer_flowplayer_t.prototype.trackidx		= function(){ return this.m_trackidx;	}
neoip.subplayer_flowplayer_t.prototype.start_time	= function(){ return this.m_start_time;	}
