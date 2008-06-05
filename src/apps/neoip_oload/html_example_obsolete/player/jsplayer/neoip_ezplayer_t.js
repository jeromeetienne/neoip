/*! \file
    \brief Definition of the ezplayer_t

\par Brief Description
A bunch of function on top of neoip.player_t to really simplify the usage of it

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
neoip.ezplayer_t	= function()
{
	this.m_subplayer_html_id	= "subplayer_plugin_html_id";
	// determine the type of subplayer to init. "vlc"|"asplayer" are the valid one
	this.m_subplayer_type		= "asplayer";
	// state variable to know the fullpage_state "normal"|"maximized"
	this.m_fullpage_state		= "normal";
	// set the default 'play_post_playlist' value
	this.m_play_post_playlist	= false;
	// set the default 'autobuffer' value
	// - aka start buffering as soon as created
	this.m_autobuffer		= false;

	// initilize the playlist_uri
	this.m_playlist_uri	= "../playlist.jspf/sample_static.playlist.jspf";
//	this.m_playlist_uri	= "../playlist.jspf/sample_stream.playlist.jspf";
//	this.m_playlist_uri	= "../playlist.jspf/sample_stream_static.playlist.jspf";
//	this.m_playlist_uri	= "../playlist.jspf/ntv002.playlist.jspf";
//	this.m_playlist_uri	= "../playlist.jspf/auto_bliptv.playlist.jspf";
//	this.m_playlist_uri	= "../playlist.jspf/bliptv_at_random.playlist.jspf";
//	this.m_playlist_uri	= "../playlist.jspf/youtube_featured_at_random.playlist.jspf";
//	this.m_playlist_uri	= "../playlist.jspf/youtube_tag_at_random.playlist.jspf";
//	this.m_playlist_uri	= "../playlist.jspf/youporn_at_random.playlist.jspf";
}

/** \brief Destructor
 */
neoip.ezplayer_t.prototype.destructor	= function()
{
	// delete the oload apps_detect_t
	if( this.m_oload_detect ){
		this.m_oload_detect.destructor();
		this.m_oload_detect	= null;
	}
	// delete the casto apps_detect_t
	if( this.m_casto_detect ){
		this.m_casto_detect.destructor();
		this.m_casto_detect	= null;
	}
	// delete the neoip.player_t if needed
	if( this.m_player ){
		this.m_player.destructor();
		this.m_player		= null;
	}
	// delete the neoip.sub_player_*_t if needed
	if( this.m_subplayer ){
		this.m_subplayer.destructor();
		this.m_subplayer	= null;
	}
	// delete the neoip.player_t if needed
	if( this.m_embedui ){
		this.m_embedui.destructor();
		this.m_embedui		= null;
	}
	// delete the neoip.playlist_loader_t if needed
	if( this.m_playlist_loader ){
		this.m_playlist_loader.destructor();
		this.m_playlist_loader= null;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Allow to set the 'play_post_playlist' feature 
 * 
 * - MUST be done before start()
 */
neoip.ezplayer_t.prototype.play_post_playlist = function(value)
{
	// sanity check - the value MUST be a valid one
	console.assert( value == true || value == false );
	// update the local value
	this.m_play_post_playlist	= value;
	// sanity check - the autobuffer and play_post_playlist feature are mutually exclusive
	console.assert( this.m_autobuffer == false || this.m_play_post_playlist == false ); 
}

/** \brief Allow to set the 'autobuffer' feature 
 * 
 * - MUST be done before start()
 */
neoip.ezplayer_t.prototype.autobuffer = function(value)
{
	// sanity check - the value MUST be a valid one
	console.assert( value == true || value == false );
	// update the local value
	this.m_autobuffer	= value;
	// sanity check - the autobuffer and play_post_playlist feature are mutually exclusive
	console.assert( this.m_autobuffer == false || this.m_play_post_playlist == false ); 
}

/** \brief Allow to set the 'fullpage_state' 
 * 
 * - MUST be done before start() ?
 */
neoip.ezplayer_t.prototype.fullpage_state = function(value)
{
	// sanity check - the value MUST be a valid one
	console.assert( value == "normal" || value == "maximized" );
	// update the local value
	this.m_fullpage_state	= value;
}

/** \brief Allow to set the 'playlist_arr' - used to build the playlist in ezplayer_embedui_t
 */
neoip.ezplayer_t.prototype.load_playlist_arr = function(playlist_arr_uri)
{
	// setup the playlist_arr for ezplayer_t 
 	// - NOTE to do eval on json as if if were javascript cause some security issue
	var playlist_arr_str	= neoip.core.download_file_insync(playlist_arr_uri);
	this.m_playlist_arr	= eval('(' + playlist_arr_str + ')');
	
	// change the playlist
	// - if there is a "ezplayer_playlist_uri" cookie, use this one
	// - else pick the first of the this.m_playlist_arr
	var prev_playlist_uri	= neoip.core.cookie_read("ezplayer_playlist_uri");
	// TODO if prev_playlist_uri != null, check if it is contained in this.m_playlist_arr
	if( prev_playlist_uri )	ezplayer.change_playlist(prev_playlist_uri);
	else			ezplayer.change_playlist(this.m_playlist_arr[0]['playlist_uri']);
}

/** \brief start the operation
 */
neoip.ezplayer_t.prototype.start = function()
{
	// register the event_listener to be warned on load
	var cb_fct	= neoip.basic_cb_t(this._window_onload_cb, this)
	neoip.core.dom_event_listener(window,"load", cb_fct);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			window_onload_cb
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Callback called when the window is fully loaded
 */
neoip.ezplayer_t.prototype._window_onload_cb	= function()
{
	// build the neoip.subplayer_t  - has it been tested
	if( this.m_subplayer_type == "vlc" ){
		var subplayer_fct_str	= "playlist.isPlaying";
		this.m_subplayer	= new neoip.subplayer_vlc_t(this.m_subplayer_html_id);
	}else if( this.m_subplayer_type == "asplayer" ){
		var subplayer_fct_str	= "track_count";
		this.m_subplayer	= new neoip.subplayer_asplayer_t(this.m_subplayer_html_id);
	}else{	console.assert(false);	}

	// determine the objembed size depending on this.m_fullpage_state
	// - TODO how does this fit in the embedui stuff?
	if( this.m_fullpage_state == "normal" ){
		var objembed_w	= "320";
		var objembed_h	= "240";
	}else if( this.m_fullpage_state == "maximized" ){
		var objembed_w	= "100%";
		var objembed_h	= "100%";
	}else{	console.assert(false);	}	
	// build the objembed for this.m_subplayer
	this.m_subplayer.build_objembed('neoip_player_container_id', objembed_w, objembed_h);

	// start waiting for the browser to initialize the plugin
	// - TODO put that in neoip.core ?
	// - TODO this leave a dangling action which cant be canceled...
	//   - make is an object of its own
	objembed_wait4init(this.m_subplayer_html_id, subplayer_fct_str, 100, 5000
				, neoip.basic_cb_t(this._objembed_wait4init_cb, this));

	// start detecting the neoip-apps
	this._apps_detect_start();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			objembed_wait4init_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called by objembed_wait4init when the subplayer is loaded
 */
neoip.ezplayer_t.prototype._objembed_wait4init_cb	= function()
{
	// log to debug
	//console.info("enter");
	
	// setup the player
	this.m_player	= new neoip.player_t(this.m_subplayer
				, neoip.player_cb_t(this._neoip_player_cb, this));
	
	// build the playlist_loader_t
	this.m_playlist_loader = new neoip.playlist_loader_t(this.m_playlist_uri
			, neoip.playlist_loader_cb_t(this._neoip_playlist_loader_cb, this));

	// call _player_post_init now
	this._player_post_init()
	
	// if plugin support embedui, create this.m_embedui
	var plugin	= document.getElementById(this.m_subplayer_html_id)
	if( typeof(plugin.embedui_create) == "function" )
		this.m_embedui	= new neoip.ezplayer_embedui_t(this);
}

neoip.ezplayer_t.prototype._player_post_init	= function()
{
	// if this.m_player is not yet initialized return now
	if( !this.m_player )	return;
	
	// initialize the player outter_uri if some neoip-apps have already been detected
	for(var apps_suffix in {"oload": null, "casto": null}){
		var outter_uri	= null;
		// if this neoip-apps is present, get its outter_uri
		if( neoip.apps_present(apps_suffix) )	outter_uri = neoip.outter_uri(apps_suffix);
		// update this.m_player with this outter_uri for this apps_suffix
		this.m_player.set_outter_uri(apps_suffix, outter_uri);
	}

	// sanity check - the autobuffer and play_post_playlist feature are mutually exclusive
	console.assert( this.m_autobuffer == false || this.m_play_post_playlist == false ); 

	// if this.m_autobuffer is enabled, notify this.m_player
	// - TODO neoip.player_t._prefetch_initial is a VERY bad name for it. change it	
	if( this.m_autobuffer )		this.m_player._prefetch_initial();
	
	// if there is still a apps_detect_t running, return now
	if( this.m_oload_detect )		return;
	if( this.m_casto_detect )		return;
	// if there is no playlist, return now
	if( this.m_player.playlist() == null )	return;
	// if this.m_play_post_playlist is disabled, return now
	if( this.m_play_post_playlist == false)	return;
	
	// set this.m_play_post_playlist to false
	this.m_play_post_playlist	= false;

	// update the cookie 'ezplayer_playlist_uri'
	neoip.core.cookie_write("ezplayer_playlist_uri", this.m_playlist_uri, 30);
	
	// NOTE: here m_play_post_playlist is enabled and this.m_player is ready to start_playing

	// start playing
	this.playing_start();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			neoip.playlist_loader_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief neoip.playlist_loader_t callback
 */
neoip.ezplayer_t.prototype._neoip_playlist_loader_cb = function(notified_obj, userptr
							, event_type, arg)
{
	// log to debug
	//console.info("enter event_type=" + event_type);
	//console.dir(arg);
	
	// if this.m_player is not yet set, do nothing
	if( !this.m_player )	return;
	
	// sanity check - arg['playlist'] MUST exist
	console.assert( arg['playlist'] );
	// set the playlist in this.m_player
	this.m_player.playlist( arg['playlist'] );
	
	// if this.m_play_post_playlist is enable, see if it is possible to run it now
	if( this.m_play_post_playlist )	this._player_post_init();
	// if _embedui_supported, fwd neoip.playlist_loader_t event, update embedui accordingly
	if( this.m_embedui )		this.m_embedui.playlist_loader_cb(event_type, arg);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			neoip_player_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief neoip.player_t callback
 */
neoip.ezplayer_t.prototype._neoip_player_cb = function(notified_obj, userptr
							, event_type, arg)
{
	// log to debug
	//console.info("enter_type=" + event_type);	console.dir(arg);
	
	// forward the event_type to the proper handler
	if( event_type == "changed_state" || event_type == "new_time" ){
		neoip.player_wikidbg.main_cb(this.m_player, "admin", 'player_info_container_id');
		
		// if _embedui_supported, fwd neoip.player_t event, update embedui accordingly
		if( this.m_embedui )	this.m_embedui.neoip_player_cb(event_type, arg);
	}else if( event_type == "error" ){
		if( this.m_player.is_playing() )	this.playing_stop();
		// if _embedui_supported, fwd neoip.player_t event, update embedui accordingly
		if( this.m_embedui )	this.m_embedui.neoip_player_cb(event_type, arg);
	}else if( event_type == "embedui_event" ){
		this.m_embedui.embedui_event_cb(arg['event_type'], arg['arg']);
	}
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			apps_detect_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the apps_detect_t object
 */
neoip.ezplayer_t.prototype._apps_detect_start	= function()
{
	// start probing neoip-apps
	var cb_fct		= neoip.apps_detect_cb_t(this._apps_detect_cb, this);
	// TODO put that in a array with suffix as key
	this.m_oload_detect	= new neoip.apps_detect_t("oload", 4550, 4559, cb_fct);
	this.m_casto_detect	= new neoip.apps_detect_t("casto", 4560, 4569, cb_fct);						
}

/** \brief Callback for all the neoip.apps_detect_t of this page
 */
neoip.ezplayer_t.prototype._apps_detect_cb	= function(apps_detect, userptr, result_str)
{
	var apps_suffix	= apps_detect.suffix_name();
	// log the result
	//console.info("enter suffix_name=" + apps_detect.suffix_name() + " result_str=" + result_str);

	// delete the apps_detect
	if( apps_suffix == "oload" ){
		this.m_oload_detect.destructor();
		this.m_oload_detect	= null;
	}else if( apps_suffix == "casto" ){
		this.m_casto_detect.destructor();
		this.m_casto_detect	= null;
	}else{	console.assert(false);	}
	
	// display the result via wikidbg
	neoip.apps_detect_wikidbg.main_cb(null, "page", 'apps_detect_container_id');

	// call _player_post_init now
	this._player_post_init()
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			public function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief To change the playlist
 */
neoip.ezplayer_t.prototype.change_playlist	= function(p_playlist_uri)
{
	// delete the current playlist_loader if needed
	if( this.m_playlist_loader ){
		this.m_playlist_loader.destructor();
		this.m_playlist_loader	= null;
	}
	// update the local playlist_uri
	this.m_playlist_uri	= p_playlist_uri;
	// if this.m_player is not yet initialized, return now
	if( !this.m_player )	return; 
	// init the playlist_loader for this playlist_uri
	this.m_playlist_loader = new neoip.playlist_loader_t(this.m_playlist_uri
			, neoip.playlist_loader_cb_t(this._neoip_playlist_loader_cb, this));
}

/** \brief To start playing
 */
neoip.ezplayer_t.prototype.playing_start	= function()
{
	// ask the player_t to start playing
	this.m_player.playing_start();
	// notify the embedui if supported
	if( this.m_embedui )	this.m_embedui.playing_start();
}

/** \brief To stop playing
 */
neoip.ezplayer_t.prototype.playing_stop	= function()
{
	// ask the player_t to stop playing
	this.m_player.playing_stop();
	// notify the embedui if supported
	if( this.m_embedui )	this.m_embedui.playing_stop();
}
