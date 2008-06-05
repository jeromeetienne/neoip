/*! \file
    \brief Definition of the ezplayer_embedui_t

\par Brief Description
handle all the embedui on top of neoip.ezplayer_t

- TODO make this its own object
  - required by regularity rules
  - have pointer on m_ezplayer in ctor
  - have a .is_supported public function
  - all public function must return immediatly if is_not_supported();

\par Possible UI improvement
- if the mouse does not more for a while, hide it
- if a ui element is visible but no action occurs on it for a while, hide it
- when in picinpic, click anywhere on the video would toogle fullscreen
- when not playing and a playlist is selected, go into playing

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
neoip.ezplayer_embedui_t	= function(p_ezplayer)
{
	// copy the parameter
	this.m_ezplayer	= p_ezplayer;
	
	// get directly the plugin
	var plugin	= this._get_plugin()
	

	// build the "embedui_id_root_stage"
	var embedui_opt	= {	"embedui_class"	: "root_stage",
				"embedui_id"	: "embedui_id_root_stage",
				"userptr": {
					"embedui_id"	: "embedui_id_root_stage",
					},
				"element_opt"	: {
					}
				};
	plugin.embedui_create(embedui_opt);	


	// init the plugin sound from the values saved in the cookie
	this._sound_init_from_cookie();
	// build the "embedui_id_volume"
	var embedui_opt	= {	"embedui_class"	: "button_volume",
				"embedui_id"	: "embedui_id_volume",
				"userptr": {
					"embedui_id"	: "embedui_id_volume",
					},
				"element_opt" : {
					"sound_vol"	: plugin.get_sound_vol(),
					"sound_mute"	: plugin.get_sound_mute()
					},	
			 	"base_sprite" : {
			 		"element_x"	: 0.0,
			 		"element_y"	: 0.0,
			 		"element_w"	: 0.1,
			 		"element_h"	: 0.0,
					"anchor_x"	: 0.5, 
					"anchor_y"	: 0.5,
					"display_type"	: "mouse_over" 
					},
				};
	plugin.embedui_create(embedui_opt);
	
	// build the "embedui_id_winsizer"
	var embedui_opt	= {	"embedui_class"	: "button_bitmap",
				"embedui_id"	: "embedui_id_winsizer",
				"userptr": {
					"embedui_id"	: "embedui_id_winsizer",
					},
				"element_opt" : {
					"type"		: "vector",
					"location"	: plugin.get_fullscreen() 
								? "win_normalizer"
								: "win_maximizer"
					},
			 	"base_sprite" : {
			 		"element_x"	: 1.0,
			 		"element_y"	: 0.0,
			 		"element_w"	: 0.08,
			 		"element_h"	: 0.0,
					"anchor_x"	: 0.0, 
					"anchor_y"	: 0.0,
					"display_type"	: "mouse_over"
					},
				};
	plugin.embedui_create(embedui_opt);
	
	// build the "embedui_id_playlist_toggle"
	var embedui_opt	= {	"embedui_class"	: "button_bitmap",
				"embedui_id"	: "embedui_id_playlist_toggle",
				"userptr": {
					"embedui_id"	: "embedui_id_playlist_toggle",
					},
				"element_opt" : {
					"type"		: "embedded",
					"location"	: "embed_pic_globe"
					},
			 	"base_sprite" : {
			 		"element_x"	: 0.0,
			 		"element_y"	: 0.5,
			 		"element_w"	: 0.08,
			 		"element_h"	: 0.0,
					"display_type"	: "mouse_over"
					},
				};
	plugin.embedui_create(embedui_opt);
	
	// create the "embedui_id_track_title"
	var embedui_opt	= {	"embedui_class"	: "text_caption",
				"embedui_id"	: "embedui_id_track_title",
				"userptr": {
					"embedui_id"	: "embedui_id_track_title",
					},
				"element_opt" : {
					"text"		: " ",
					"font_size"	: 100/1024
					},
			 	"base_sprite" : {
			 		"element_x"	: 0.5,
			 		"element_y"	: 0.8,
			 		"element_w"	: 1.0,
			 		"element_h"	: 0.0,
					"anchor_x"	: 0.5, 
					"anchor_y"	: 0.5,
					"display_type"	: "timeout",
					"timeout_delay"	: 4*1000,
					"mouse_action"	: false
					},
				};
	plugin.embedui_create(embedui_opt);
	
	// create the "embedui_id_track_title"
	var embedui_opt	= {	"embedui_class"	: "text_caption",
				"embedui_id"	: "embedui_id_status_line",
				"userptr": {
					"embedui_id"	: "embedui_id_status_line",
					},
				"element_opt" : {
					"text"		: " ",
					"font_size"	: 100/1024,
					"font_color"	: 0xFF0000
					},
			 	"base_sprite" : {
			 		"element_x"	: 0.5,
			 		"element_y"	: 0.2,
			 		"element_w"	: 1.0,
			 		"element_h"	: 0.0,
					"anchor_x"	: 0.5, 
					"anchor_y"	: 0.5,
					"display_type"	: "timeout",
					"timeout_delay"	: 4*1000,
					"mouse_action"	: false
					},
				};
	plugin.embedui_create(embedui_opt);

	// reinitialize play/stop buttons 
	this._reinit_playstop_buttons();
}

/** \brief Destructor
 */
neoip.ezplayer_embedui_t.prototype.destructor	= function()
{
	// get directly the plugin
	var plugin	= this._get_plugin()

	// delete all the elemui from the plugin
	plugin.embedui_delete("embedui_id_root_stage");
	plugin.embedui_delete("embedui_id_volume");
	plugin.embedui_delete("embedui_id_winsizer");
	plugin.embedui_delete("embedui_id_playlist_toggle");
	plugin.embedui_delete("embedui_id_track_title");
	plugin.embedui_delete("embedui_id_status_line");
	// delete all the 'moving' embedui element
	this._embedui_delete_playlist_select();
	this._embedui_delete_play();
	this._embedui_delete_stop();
	this._embedui_delete_busy();	
}


/** \brief init plugin sound according to the values saved in the cookie
 */
neoip.ezplayer_embedui_t.prototype._sound_init_from_cookie	= function()
{
	// get directly the plugin
	var plugin	= this._get_plugin()
	// set the current plugin.set_sound_mute according to the value saved in cookie
	ezplayer_sound_mute	= neoip.core.cookie_read("ezplayer_sound_mute");
	if( ezplayer_sound_mute )	plugin.set_sound_mute(Number(ezplayer_sound_mute));
	// set the current plugin.set_sound_vol according to the value saved in cookie
	ezplayer_sound_vol	= neoip.core.cookie_read("ezplayer_sound_vol");
	if( ezplayer_sound_vol )	plugin.set_sound_vol(ezplayer_sound_vol);
}
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the plugin element
 */
neoip.ezplayer_embedui_t.prototype._get_plugin	= function()
{
	return document.getElementById(this.m_ezplayer.m_subplayer_html_id);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			embedui service callbacks
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Callback for any embedui event (forwarded from the neoip.ezplayer_t)
 */
neoip.ezplayer_embedui_t.prototype.embedui_event_cb	= function(event_type, arg)
{
	var userptr	= arg['userptr'];
	var embedui_id	= userptr['embedui_id'];
	// log to debug
	//console.info("embedui_id=" + embedui_id);	console.dir(arg);
	
	// forward to the proper callback depending on embedui_id
	if( embedui_id == "embedui_id_root_stage" ){
		return this._embedui_root_stage_cb		(event_type, arg);
	}else if( embedui_id == "embedui_id_volume" ){
		return this._embedui_button_vol_cb		(event_type, arg);
	}else if( embedui_id == "embedui_id_winsizer" ){
		return this._embedui_button_winsizer_cb		(event_type, arg);
	}else if( embedui_id == "embedui_id_play" && event_type == "click" ){
		return this.m_ezplayer.playing_start();
	}else if( embedui_id == "embedui_id_stop" && event_type == "click" ){
		return this.m_ezplayer.playing_stop();
	}else if( embedui_id == "embedui_id_playlist_toggle" ){
		return this._embedui_playlist_toggle_cb		(event_type, arg);
	}else if( embedui_id == "embedui_id_playlist_select" ){
		return this._embedui_playlist_select_cb		(event_type, arg);
	}
}

/** \brief Callback for any embedui event from the "button_vol_id"
 */
neoip.ezplayer_embedui_t.prototype._embedui_button_vol_cb	= function(event_type, arg)
{
	var embedui_id	= "embedui_id_volume";
	var plugin	= this._get_plugin();

	// if event_type == "click", toggle the mute status
	if( event_type == "click" ){
		// toggle the mute in the plugin
		plugin.set_sound_mute( !plugin.get_sound_mute() );
		
		// update the cookie 'ezplayer_sound_mute'
		neoip.core.cookie_write("ezplayer_sound_mute", Number(plugin.get_sound_mute()), 30);
		// change the ui_element
		plugin.embedui_update(embedui_id, { "action": "element_update_opt",
					"arg":	{ "sound_mute"	: plugin.get_sound_mute() } });
	}
	// if event_type == "mouseWheel", inc/dec the sound_vol
	if( event_type == "mouseWheel" ){
		var delta	= arg['wheel_delta'] * 0.1;
		var new_vol	= plugin.get_sound_vol() + delta;
		// clamp new_vol between 0 and 1
		new_vol		= Math.max(new_vol, 0);
		new_vol		= Math.min(new_vol, 1);
		// set the new_vol in the plugin
		plugin.set_sound_vol( new_vol );
		// update the cookie 'ezplayer_sound_vol'
		neoip.core.cookie_write("ezplayer_sound_vol", plugin.get_sound_vol(), 30);
		// change the ui_element
		plugin.embedui_update(embedui_id, { "action": "element_update_opt",
					"arg":	{ "sound_vol"	: plugin.get_sound_vol() } });			
	}
}

/** \brief Callback for any embedui event from the "embedui_id_winsizer"
 */
neoip.ezplayer_embedui_t.prototype._embedui_button_winsizer_cb	= function(event_type, arg)
{
	var embedui_id		= "embedui_id_winsizer";
	var plugin		= this._get_plugin()
	var may_fullscreen	= plugin.may_fullscreen();


	if( event_type == "click" && may_fullscreen ){
		// toggle the hw-fullscreen in the plugin
		plugin.set_fullscreen( !plugin.get_fullscreen() );
	}else if( event_type == "mouseWheel" || event_type == "click" ){
		if( plugin.getAttribute("width") == "100%" ){
			plugin.setAttribute("width", "320");
			plugin.setAttribute("height", "240");
		}else{
			plugin.setAttribute("width", "100%");
			plugin.setAttribute("height", "100%");
		}
	}

	// make the embedui invisible
	plugin.embedui_update(embedui_id, { "action": "base_reset_state",
				"arg":	{ "new_state": "invisible" } });


	// determine the image to display
	var is_fullscreen	= plugin.get_fullscreen();
	var is_maximized	= plugin.getAttribute("width") == "100%";
	var button_normalizer	= may_fullscreen ? is_fullscreen : is_maximized;
	// update the ui
	plugin.embedui_update(embedui_id, { "action": "element_update_opt",
				"arg":	{ "location" : button_normalizer 
						? "win_normalizer"
						: "win_maximizer"} });
}


/** \brief Callback for any embedui event from the "embedui_id_root_stage"
 */
neoip.ezplayer_embedui_t.prototype._embedui_root_stage_cb	= function(event_type, arg)
{
	var embedui_id	= "embedui_id_root_stage";
	var plugin	= this._get_plugin()

	// log to debug
	//console.info("enter");	console.dir(arg);

	// if "doubleClick" and plugin.may_fullscreen(), toggle fullscreen
	if( event_type == "doubleClick" && plugin.may_fullscreen() ){
		// toggle the hw-fullscreen in the plugin
		plugin.set_fullscreen( !plugin.get_fullscreen() );
		// update the "embedui_id_winsizer"
		plugin.embedui_update("embedui_id_winsizer", { "action": "element_update_opt",
				"arg":	{ "location" : plugin.get_fullscreen() 
						? "win_normalizer"
						: "win_maximizer"} });
	}

	// hide/show the mouse on changed_state
	if( event_type == "changed_state" ){
		var new_state	= arg['new_state'];
		if( new_state == "idle_detect" ){
			plugin.embedui_update(embedui_id, { "action": "element_update_opt",
					"arg":	{ "mouse_visibility" : "show"	}});
		}else{
			plugin.embedui_update(embedui_id, { "action": "element_update_opt",
					"arg":	{ "mouse_visibility" : "hide"	}});
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			playlist handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Callback for any embedui event from the "embedui_id_playlist_toggle"
 */
neoip.ezplayer_embedui_t.prototype._embedui_playlist_toggle_cb	= function(event_type, arg)
{
	var embedui_id	= "embedui_id_playlist_select";
	var plugin	= this._get_plugin()
	// log to debug
	//console.info("enter");

	// if the event_type is not "click", do nothing
	if( event_type != "click" )	return;

	// toggle the playlist_select element
	if( !plugin.embedui_exist(embedui_id) )	this._embedui_create_playlist_select();
	else					this._embedui_delete_playlist_select();
	// reinitialize play/stop buttons 
	this._reinit_playstop_buttons();
}

/** \brief Callback for any embedui event from the "embedui_id_playlist_select"
 */
neoip.ezplayer_embedui_t.prototype._embedui_playlist_select_cb	= function(event_type, arg)
{
	var selected_idx	= arg['selected_idx'];
	var item_userptr	= arg['item_userptr'];
	// log to debug
	console.info("enter selected_idx=" + selected_idx);
	console.dir(arg);
	// delete the playlist_select
	this._embedui_delete_playlist_select();
	// reinitialize play/stop buttons according to this.m_ezplayer.m_player.is_playing() 
	this._reinit_playstop_buttons();

	// change the playlist
	this.m_ezplayer.play_post_playlist(true);
	this.m_ezplayer.change_playlist(item_userptr['playlist_uri']);
	// TODO if not playing, it should start playing
	// - but to start playing now, would not play the new playlist
	// - but the current one
}

/** \brief create the embedui_id_play button
 */
neoip.ezplayer_embedui_t.prototype._embedui_create_playlist_select	= function()
{
	var plugin	= this._get_plugin()
	var embedui_id	= "embedui_id_playlist_select";
	// if this embedui_id alreadt exist, return now
	if( plugin.embedui_exist(embedui_id) == true )	return;
	
	// determine embedui_opt for this element
	var embedui_opt	= {	"embedui_class"	: "select_list",
				"embedui_id"	: "embedui_id_playlist_select",
				"userptr": {
					"embedui_id"	: "embedui_id_playlist_select",
					},
				"element_opt"	: {
					"selected_idx"	: 0,
					"item_w"	: 500	/1024,
					"item_h"	: 70	/ 768,
					"box_t"		: 7	/1024,
					"margin_w"	: 10	/1024,
					"margin_h"	: 0	/ 768,
					"font_size"	: 60	/1024,
					"item_arr"	: [ "FILLED DYNAMICALLY JUST AFTER" ]
					}
				};
				
	// build the item_arr containing the whole this.m_ezplayer.m_playlist_arr
	var item_arr	= []
	for(var i = 0; i < this.m_ezplayer.m_playlist_arr.length; i++){
		var	playlist_item	= this.m_ezplayer.m_playlist_arr[i];
		item_arr.push(	{	"display_text" 	: playlist_item['playlist_title'],
					"item_userptr"	: {
						"playlist_uri"	: playlist_item['playlist_uri']
					}
				});
	}
	// update the item_arr into embedui_opt['element_opt']
	embedui_opt['element_opt']['item_arr']	= item_arr;
	
	// update the selected_idx into embedui_opt['element_opt']
	for(var i = 0; i < this.m_ezplayer.m_playlist_arr.length; i++){
		var	playlist_item	= this.m_ezplayer.m_playlist_arr[i];
		// if this playlist_item['playlist_uri'] matches current playlist_uri, select it 
		if( playlist_item['playlist_uri'] == this.m_ezplayer.m_playlist_uri ){
			embedui_opt['element_opt']['selected_idx']	= i;
			break;
		}
	}
		
	// actually ask the plugin to create the element
	plugin.embedui_create(embedui_opt);
}

/** \brief delete the embedui_id_play button
 */
neoip.ezplayer_embedui_t.prototype._embedui_delete_playlist_select	= function()
{
	var plugin	= this._get_plugin()
	var embedui_id	= "embedui_id_playlist_select";
	// if this embedui_id doesnt exist, return now
	if( plugin.embedui_exist(embedui_id) == false )	return;
	// delete the embedui
	plugin.embedui_delete(embedui_id);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			handle event from neoip.player_cb_t 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Callback for any neoip.player_cb_t event - forwarded to update the embedui
 */
neoip.ezplayer_embedui_t.prototype.neoip_player_cb	= function(event_type, arg)
{
	// get directly the plugin
	var plugin	= this._get_plugin();
	// log to debug
	//console.info("event_type=" + event_type);console.dir(arg);

	// when entering in connection, display the track_title
	// - TODO display it "always" when "connecting" or "kframefinding" 
	//   and "timeout"+"disapearing" when playing
	if( event_type == "changed_state" && arg['new_state'] == "playing" ){
		var trackidx		= this.m_ezplayer.m_player.practical_trackidx();	
		var playlist_track	= this.m_ezplayer.m_player.playlist().track_at(trackidx);
		var track_title		= playlist_track.title();
		var embedui_id		= "embedui_id_track_title";
		// make the embedui invisible
		plugin.embedui_update(embedui_id, { "action": "base_reset_state",
						"arg":	{ "new_state": "invisible" } });
		// change the text
		plugin.embedui_update(embedui_id, { "action": "element_update_opt",
						"arg":	{ "text": track_title } });
		// make the embedui appearing
		plugin.embedui_update(embedui_id, { "action": "base_reset_state",
						"arg":	{ "new_state": "appearing" } });
	}else if( event_type == "error" ){
		var embedui_id		= "embedui_id_status_line";
		var text		= arg['reason'].replace(".", " ", "g");
		// make the embedui invisible
		plugin.embedui_update(embedui_id, { "action": "base_reset_state",
						"arg":	{ "new_state": "invisible" } });
		// change the text
		plugin.embedui_update(embedui_id, { "action": "element_update_opt",
						"arg":	{ "text": text } });
		// make the embedui appearing
		plugin.embedui_update(embedui_id, { "action": "base_reset_state",
						"arg":	{ "new_state": "appearing" } });
	}
	// reinit_playstop_buttons if event_type is "changed_state"
	if( event_type == "changed_state" )	this._reinit_playstop_buttons();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			handle event from neoip.playlist_loader_cb_t 
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Callback for any neoip.playlist_loader_cb_t event (forwarded from neoip.ezplayer_t)
 */
neoip.ezplayer_embedui_t.prototype.playlist_loader_cb	= function(event_type, arg)
{
	// log to debug
	//console.info("enter event_type=" + event_type);
	//console.dir(arg);
	
	// reinit_playstop_buttons
	this._reinit_playstop_buttons();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			embedui service play/stop stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle a playing_start() (forwarded from neoip.ezplayer_t)
 */
neoip.ezplayer_embedui_t.prototype.playing_start	= function()
{
	// reinitialize play/stop buttons according to this.m_ezplayer.m_player.is_playing() 
	this._reinit_playstop_buttons();
}

/** \brief Handle a playing_stop()  (forwarded from neoip.ezplayer_t)
 */
neoip.ezplayer_embedui_t.prototype.playing_stop	= function()
{
	// reinitialize play/stop buttons according to this.m_ezplayer.m_player.is_playing() 
	this._reinit_playstop_buttons();

	// make the embedui_id_track_title invisible - just in case it is still visible
	var plugin	= this._get_plugin()
	plugin.embedui_update("embedui_id_track_title", { "action": "base_reset_state",
						"arg":	{ "new_state": "invisible" } });
}

/** \brief reinitialize play/stop buttons according to this.m_ezplayer.m_player.is_playing();
 */
neoip.ezplayer_embedui_t.prototype._reinit_playstop_buttons	= function()
{
	var plugin	= this._get_plugin()

	// if "embedui_id_playlist_select" exists, hide play/stop/busy as they are all in middle
	if( plugin.embedui_exist("embedui_id_playlist_select") ){
		this._embedui_delete_play();
		this._embedui_delete_stop();
		this._embedui_delete_busy();
		return;
	}

	// handle the busy button
	var track_state	= this.m_ezplayer.m_player.practical_state();
	if( track_state == "kframefinding" || track_state == "connecting" ){
		this._embedui_create_busy();
	}else{
		this._embedui_delete_busy();
	}

	// display "play" or "stop" depending on this.m_ezplayer.m_player.is_playing()
	if( this.m_ezplayer.m_player.is_playing() ){
		// if m_player.is_playing, display "stop" and not "play"
		this._embedui_delete_play();
		this._embedui_create_stop();
	}else{
		// if m_player.is_playing, display "play" and not "stop"
		this._embedui_delete_stop();
		this._embedui_create_play();
	}
}

/** \brief create the embedui_id_play button
 */
neoip.ezplayer_embedui_t.prototype._embedui_create_play	= function()
{
	var plugin	= this._get_plugin()
	var embedui_id	= "embedui_id_play";
	// if this embedui_id alreadt exist, return now
	if( plugin.embedui_exist(embedui_id) == true )	return;
	
	// determine embedui_opt for this element
	var embedui_opt	= {	"embedui_class"	: "button_bitmap",
				"embedui_id"	: embedui_id,
				"userptr": {
					"embedui_id"	: embedui_id,
					},
				"element_opt" : {
					"type"		: "vector",
					"location"	: "play"
					},
			 	"base_sprite" : {
			 		"element_x"	: 0.5,
			 		"element_y"	: 0.5,
			 		"element_w"	: 0.1,
			 		"element_h"	: 0.0,
					"anchor_x"	: 0.5, 
					"anchor_y"	: 0.5,
					"display_type"	: "always"
					},
				};
	// actually ask the plugin to create the element
	plugin.embedui_create(embedui_opt);
}

/** \brief delete the embedui_id_play button
 */
neoip.ezplayer_embedui_t.prototype._embedui_delete_play	= function()
{
	var plugin	= this._get_plugin()
	var embedui_id	= "embedui_id_play";
	// if this embedui_id doesnt exist, return now
	if( plugin.embedui_exist(embedui_id) == false )	return;
	// delete the embedui
	plugin.embedui_delete(embedui_id);
}

/** \brief create the embedui_id_stop button
 */
neoip.ezplayer_embedui_t.prototype._embedui_create_stop	= function()
{
	var plugin	= this._get_plugin()
	var embedui_id	= "embedui_id_stop";
	// if this embedui_id alreadt exist, return now
	if( plugin.embedui_exist(embedui_id) == true )	return;

	// determine embedui_opt for this element
	var embedui_opt	= {	"embedui_class"	: "button_bitmap",
				"embedui_id"	: embedui_id,
				"userptr": {
					"embedui_id"	: embedui_id,
					},
				"element_opt" : {
					"type"		: "vector",
					"location"	: "stop"
					},
			 	"base_sprite" : {
			 		"element_x"	: 0.5,
			 		"element_y"	: 0.5,
			 		"element_w"	: 0.1,
			 		"element_h"	: 0.0,
					"anchor_x"	: 0.5, 
					"anchor_y"	: 0.5,
					"display_type"	: "mouse_over"
					},
				};
	// actually ask the plugin to create the element
	plugin.embedui_create(embedui_opt);
}

/** \brief delete the embedui_id_stop button
 */
neoip.ezplayer_embedui_t.prototype._embedui_delete_stop	= function()
{
	var plugin	= this._get_plugin()
	var embedui_id	= "embedui_id_stop";
	// if this embedui_id doesnt exist, return now
	if( plugin.embedui_exist(embedui_id) == false )	return;
	// delete the embedui
	plugin.embedui_delete(embedui_id);
}


/** \brief create the embedui_id_busy button
 */
neoip.ezplayer_embedui_t.prototype._embedui_create_busy	= function()
{
	var plugin	= this._get_plugin()
	var embedui_id	= "embedui_id_busy";
	// if this embedui_id alreadt exist, return now
	if( plugin.embedui_exist(embedui_id) == true )	return;
	
	// determine embedui_opt for this element
	var embedui_opt	= {	"embedui_class"	: "button_busy",
				"embedui_id"	: embedui_id,
				"userptr": {
					"embedui_id"	: embedui_id,
					},
			 	"base_sprite" : {
			 		"element_x"	: 0.5,
			 		"element_y"	: 0.5,
			 		"element_w"	: 0.1,
			 		"element_h"	: 0.0,
					"anchor_x"	: 0.5, 
					"anchor_y"	: 0.5,
					"display_type"	: "always",
					},
				};
	// actually ask the plugin to create the element
	plugin.embedui_create(embedui_opt);
}

/** \brief delete the embedui_id_busy button
 */
neoip.ezplayer_embedui_t.prototype._embedui_delete_busy	= function()
{
	var plugin	= this._get_plugin()
	var embedui_id	= "embedui_id_busy";
	// if this embedui_id doesnt exist, return now
	if( plugin.embedui_exist(embedui_id) == false )	return;
	// delete the embedui
	plugin.embedui_delete(embedui_id);
}
