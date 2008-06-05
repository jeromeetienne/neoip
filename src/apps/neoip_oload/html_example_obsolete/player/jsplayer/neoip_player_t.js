/*! \file
    \brief Definition of the player_t

\par Brief Description
neoip.player_t is able to player neoip.playlist_t and use various neoip.subplayer_t

\par TODO
- the resync feature is sketched but not tested
- apparently resync is triggered a LOT with the subplayer_vlc_t when it is isnt on screen
  - likely a vlc plugin bug related to having focus or not
- to rename all the outter_uri into webdetect_uri
  - in the player_t stuff and elsewhere
  - outter_uri is only for nested_uri in neoip-oload

*/

// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the player_t
 */
neoip.player_t = function(p_subplayer, p_callback)
{
	// copy the parameter
	this.m_subplayer	= p_subplayer;
	this.m_callback		= p_callback;
	// set the player_t backpointer in the m_subplayer
	this.m_subplayer.callback(neoip.subplayer_cb_t(this.subplayer_cb, this));
	// zero some fields
	this.m_playlist		= null;
	this.m_outter_uri_arr	= new Array();
	this.m_prefetcher_arr	= new Array();
	
	this.m_curtrack_timeout	= null;	
	this.m_is_playing	= false;

	this.m_prefetch_timeout	= null;	

	// some value for the resync feature
	// - TODO make those values tunable
	//   - by player_t or by playlist ?
	// - TODO experiment to determine what are good defaults
	this.m_resync_period	= 1.0;
	this.m_resync_threshold	= 30*1000;
	this.m_resync_timeout	= null;
}

/** \brief Destructor of player_t
 */
neoip.player_t.prototype.destructor = function()
{
	// TODO to code
	
	// if player_t is_playing(), stop it
	if( this.is_playing() )	this.playing_stop();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			curtrack stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start a curtrack
 */
neoip.player_t.prototype._curtrack_start = function(track_pos)
{
	// alias on the playlist_track
	var playlist_track	= this.m_playlist.track_at([track_pos.trackidx]);

	// log to debug
	console.dir(track_pos);

	// if the playlist_track is_static, provide the start_time
	this.m_subplayer.play(track_pos, this);

	// launch the timeout to expire at the end of this track
	var delay_remain	= playlist_track.duration() - track_pos.track_reltime;
	console.assert( delay_remain >= 0 );
	console.info("delay_remain=" + delay_remain);
	this.m_curtrack_timeout	= setTimeout(function(thisObj){thisObj._curtrack_timeout_cb(); }
							, delay_remain, this);
}

/** \brief Stop a curtrack
 */
neoip.player_t.prototype._curtrack_stop = function()
{
	// log to debug
	console.info("enter trackidx=" + this.m_subplayer.trackidx());

	// stop the m_curtrack_timeout
	console.assert( this.m_curtrack_timeout );
	clearTimeout( this.m_curtrack_timeout );
	this.m_curtrack_timeout	= null;

	// stop the prefetch
	// - NOTE: may happen if subplayer never reached to 'playing' state on track
	//   and thus its player_t.prefetcher_t never got stopped.
	this._prefetch_try_stop(this.m_subplayer.trackidx());

	// notify the subplayer to play this one now
	this.m_subplayer.stop();
}


/** \brief callback for the m_curtrack_timeout
 */
neoip.player_t.prototype._curtrack_timeout_cb	= function()
{
	var	next_trackidx	= this.m_subplayer.trackidx() + 1;
	// sanity check - the player_t MUST be is_playing()
	console.assert( this.is_playing() );
	// stop the curtrack
	this._curtrack_stop();

	// if the next_trackidx cause the playlist_t to loop
	if( next_trackidx >= this.m_playlist.trackidx_end() ){
		// if playlist_t can't loop stop completly
		if( this.m_playlist.may_loop() == false ){
			console.info("next_trackidx=" + next_trackidx);
			console.info("trackidx_end=" + this.m_playlist.trackidx_end());
			console.info("trackidx_beg=" + this.m_playlist.trackidx_beg());
			console.assert(0); // likely other things to do
			return;
		}
		// sanity check - if m_track_firstidx MUST be equal to this.m_playlist.trackidx_end()
		console.assert( next_trackidx == this.m_playlist.trackidx_end() );
		// warparound the trackidx to the begining of the playlist
		next_trackidx	= this.m_playlist.trackidx_beg();
	}

	// log to debug
	console.info("next_trackidx=" + next_trackidx);
	// get the track_pos for the next track
	// - NOTE: track_reltime: is explicitly set to 0 to ensure no seek at the begining
	var	track_pos	= { trackidx: next_trackidx, track_reltime: 0 };
	// start the curtrack
	this._curtrack_start(track_pos);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			prefetch stuff
// - TODO put that in its own file
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Initial prefetch
 *
 * - TODO this function has a poor nameing
 *   - should it be private/public ?
 * - Start prefetch the playlist_t.track_t to play 
 * - this function is intended to be used before the player_t started playing
 * - USE CASE: when the browser read the page, it start prefetching immediatly
 *   so when the user actually asks to play, the track has been prefetched 
 *   reducing the initialization latency and so improving the 
 */
neoip.player_t.prototype._prefetch_initial	= function()
{
	// get the current position from the playlist_t - the theorical one
	var	curtrack_pos	= this.m_playlist.current_track_pos();
	// sanity check - the player_t MUST NOT be is_playing()
	console.assert( !this.is_playing() );

	// TODO what is curtrack_pos.track_reltime is non 0...
	// start a prefetch on this one
	this._prefetch_try_start(curtrack_pos);
}

/** \brief init all the prefetch
 *
 * - TODO to handle the m_prefetch_timeout
 */
neoip.player_t.prototype._prefetch_reinit_full	= function()
{
	// take the time from the playlist_t
	// - NOTE: this is directly in relation when the next track will start playing
	// - so it is tiedly related to how the inter track transition is done
	// - currently this is done solely by the playlist_t time
	var curtrack_pos	= this.m_playlist.current_track_pos();

	// log to debug
	console.info("ENTER prefetch_reinit_full: curtrack_pos.trackidx=" + curtrack_pos.trackidx
					+ " track_reltime=" + curtrack_pos.track_reltime);

	// stop the prefetch_timeout if needed
	if( this.m_prefetch_timeout ){
		clearTimeout( this.m_prefetch_timeout );
		this.m_prefetch_timeout	= null;
	}

	// alias on the playlist_t.track_t
	var	curtrack	= this.m_playlist.track_at(curtrack_pos.trackidx);
	// compute the present/now moment relative to the playlist_t.base_date()	
	var	cur_base_time	= curtrack.base_reltime() + curtrack_pos.track_reltime;

	// to get the delay before the next prefetch
	var	timeout_delay	= Number.MAX_VALUE;

	// go thru each playlist_t.track_t of this.m_playlist
	for(var i = this.m_playlist.trackidx_beg(); i != this.m_playlist.trackidx_end(); i++){
		var	playlist_track	= this.m_playlist.track_at(i);
// log to debug
//console.info("playlist_track:" + " trackidx="	+ playlist_track.trackidx() 	);
//console.info("playlist_track:" + " duration="	+ playlist_track.duration() 	);
//console.info("playlist_track:" + " title="	+ playlist_track.title()	);

		// if this playlist_track is NOT prefetchable(), goto the next
		if( !playlist_track.prefetchable() )	continue;

		// if this playlist_track is prefetchable now, start a prefetch for it
		// - it is up to _prefetch_start to detect already started prefetch
		if( playlist_track.prefetchable_at(cur_base_time) ){
//console.info("Start prefetching this track");
			this._prefetch_try_start({trackidx: playlist_track.trackidx()
						, track_reltime: 0});
			continue;
		}

		// NOTE: at this point, playlist_track is not to be prefetched now

		// compute the amount of time between now and this playlist_track start playing
		var start_base_time	= playlist_track.base_reltime() - playlist_track.prefetch_delay();
//console.info("start_base_time=" + start_base_time);
		// handle special case if start_base_time is before current time
		if( start_base_time < cur_base_time ){
			// if playlist.may_not_loop, goto the next -it wont be prefeteched ever
			if( this.m_playlist.may_not_loop() )	continue;
			// update start_base_time to reflext the time
//console.info("total_duration=" + this.m_playlist.total_duration());
			start_base_time	+= this.m_playlist.total_duration();
		}
		// compute the delay before triggering the next prefetch
//console.info("timeout_delay=" + timeout_delay);
//console.info("start_base_time=" + start_base_time);
//console.info("cur_base_time=" + cur_base_time);
		timeout_delay	= Math.min(timeout_delay, start_base_time - cur_base_time);
		console.assert( timeout_delay >= 0 );
//console.info("timeout_delay=" + timeout_delay);
		
		// NOTE: it is NOT up to this function to stop any prefetch
	}
	
	// launch the m_prefetch_timeout to expire when the next prefetch it to be started
	if( timeout_delay != Number.MAX_VALUE ){
		// log to debug
		console.info("start prefetch_timeout to expire in " + timeout_delay);
		// sanity check - timeout_delay MUST be > 0
		console.assert( timeout_delay > 0 );
		// launch the timeout itself
		this.m_prefetch_timeout	= setTimeout(function(thisObj){thisObj._prefetch_reinit_full(); }
							, timeout_delay, this);
	}
	console.info("LEAVE prefetch_reinit_full");
}

/** \brief Start prefetching this trackidx in the current playlist_t
 */
neoip.player_t.prototype._prefetch_try_start = function(track_pos)
{
	var trackidx	= track_pos.trackidx;
	// if already in prefetching, do nothing
	if( this.m_prefetcher_arr[trackidx] )	return;
	// create a prefetcher_t for this track_pos
	var prefetcher	= new neoip.prefetcher_t(track_pos, this);
	// put the prefetcher in the m_prefetcher_arr at the trackidx position
	this.m_prefetcher_arr[trackidx]	= prefetcher;
}

/** \brief Stop prefetching this trackidx in the current playlist_t
 */
neoip.player_t.prototype._prefetch_try_stop = function(trackidx)
{
	// it no prefetcher for this trackidx, do nothing
	if( !this.m_prefetcher_arr[trackidx] )	return;
	// create an alias on this prefetcher_t
	var prefetcher	= this.m_prefetcher_arr[trackidx];
	// delete it from the array
	this.m_prefetcher_arr.splice(trackidx, 1);
	// destruct the prefetcher
	prefetcher.destructor();
	prefetcher	= null;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			resync stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Resync the subplayer if needed
 */
neoip.player_t.prototype._resync_if_needed = function()
{
	// restart the resync_timeout
	this.m_resync_timeout	= setTimeout(function(thisObj){thisObj._resync_if_needed(); }
							, this.m_resync_period, this);
		
	// get the track_pos for the playlist and for the subplayer
	var	playlist_track_pos	= this.m_playlist.current_track_pos();
	var	subplayer_track_pos	= this.m_subplayer.current_track_pos();

	// if the subplayer is unable to provide an track_pos, do nothing and return now
	// - TODO it is unclear why it would be unable to provide an track_pos
	//   - but the knowledge on the vlc plugin is very unknown
	if( subplayer_track_pos == null )	return;
		
	// alias the playlist_t.track_t
	var	playlist_track	= this.m_playlist.track_at(playlist_track_pos.trackidx);
	var	subplayer_track	= this.m_playlist.track_at(subplayer_track_pos.trackidx);
	
	// compute the base_time of each track_pos
	var	playlist_base_time	= playlist_track.base_reltime() + playlist_track_pos.track_reltime;
	var	subplayer_base_time	= subplayer_track.base_reltime() + subplayer_track_pos.track_reltime;

// TODO what if the track is stream ? no resync here
	
	// TODO BUG BUG BUG
	// - if the subplayer practical time is at the end of the playlist 
	// - but the theorical time is now at the begining of the playlist
	// - the jitter will be close to the whole duration of the playlist....
	// - causing unneeded resync action
	// - TODO find a formula to include this
	
	// TODO BUG BUG
	// - another bug is in subplayer_jwplayer_t which report playing the 
	//   begining of the track while it is supposed to seek at a start_time
	//   in the middle of the movie
	
	// compute the delta_sync - abs of difference between playlist/subplayer base_time
	var	delta_sync	= Math.abs(playlist_base_time - subplayer_base_time);
	// log to debug
	//console.info("delta_sync=" + delta_sync.toFixed(2) + "-sec")
	
	// if the delta_sync is less than m_resyn_threshold, do nothing
	if( delta_sync < this.m_resync_threshold )	return;
	
	// log to debig
	console.error("NEEDS RESYNC! delta_sync=" + (delta_sync/1000).toFixed(2) + "-sec");
	console.info("playlist track pos: trackidx="+ playlist_track_pos.trackidx
				+ " track_reltime=" + playlist_track_pos.track_reltime);
	console.info("subplayer track pos: trackidx="+ subplayer_track_pos.trackidx
				+ " track_reltime=" + subplayer_track_pos.track_reltime);

// TODO disable resync - just during debug - to remove
return;
	// relaunch a _play_current
	this._curtrack_stop();
	// launch a _curtrack_start
	this._curtrack_start(playlist_track_pos);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			subplayer_cb
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief The subplayer_cb called every time the subplayer has event to notify
 */
neoip.player_t.prototype.subplayer_cb	= function(notifier_obj, userptr, event_type, arg)
{
	// log to debug
	//console.info("event_type=" + event_type + " arg=%o", arg);

	// if m_subplayer just became "playing", _prefetch_try_stop for this trackidx
	if( event_type == "changed_state" && arg.new_state == "playing" )
		this._prefetch_try_stop(this.m_subplayer.trackidx());

// TODO to reenable - currently the stuff it too bugged to be trusted	
//	// if m_subplayer just became "playing", do a _prefetch_reinit_full
	if( event_type == "changed_state" && arg.new_state == "playing" )
		this._prefetch_reinit_full();

	// if a event_type == "flv_kframe_find_cb" notify a result back to javascript
	if( event_type == "flv_kframe_find_cb" ){
		var trackidx	= arg.userptr;	// userptr is set to the trackidx
		var prefetcher	= this.m_prefetcher_arr[trackidx];
		// if the triggering prefetcher is still here, forward the result
		if( prefetcher)	prefetcher.flv_kframe_find_cb(arg);
	}

	
	// forward the event to the player_cb_t
	if( this.m_callback )	this.m_callback(event_type, arg);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			public function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the playlist
 */
neoip.player_t.prototype.playlist = function(new_playlist)
{
	// if there are no parameter, return the current playlist
	if( !new_playlist )	return this.m_playlist;

	// backup the current this.m_playlist
	// - NOTE: old_playlist may be null
	var old_playlist	= this.m_playlist

	/*************** determine if player_t is restartable	***************/
	var restartable		= false;
	// if both playlist_t are not of the same instance, then it should be restarted
	if( old_playlist && old_playlist.same_instance_as(new_playlist) == false ){
		restartable	= true;
	}
	// if the subplayer.trackidx() is not in new_playlist, then 
	if( new_playlist.contain_trackidx(this.m_subplayer.trackidx()) == false ){
		restartable	= true;
	}
	// if the player_t is not playing, it is not restartable
	if( this.is_not_playing() )	restartable	= false;
	
	/*************** do restart the player_t	***********************/
	// stop playing with the old_playlist - if restartable
	// NOTE: this allow to get proper stopping notification in the callback
	if( restartable )	this.playing_stop();
	// set the new_playlist as the current one
	this.m_playlist		= new_playlist;
	this.m_playlist.set_outter_uri_arr(this.m_outter_uri_arr);
	// restart playing with the new current playlist  - if restartable
	if( restartable )	this.playing_start();
	
	// return the playlist
	return this.m_playlist;
}

/** \brief Return the practical trackidx within the subplayer
 *
 * - WARNING: the player_t MUST be playing
 */
neoip.player_t.prototype.practical_trackidx	= function()
{
	// sanity check - player_t MUST be in is_playing()
// TODO to check - apparently i use them in player_cb_t and there is a race in which it is not
//	console.assert( this.is_playing() );
	// return the m_subplayer.trackidx
	return this.m_subplayer.trackidx();
}

/** \brief Return the practical track_reltime for the subplayer
 *
 * - WARNING: if subplayer is unable to return the track_reltime, it may return null
 * - WARNING: the player_t MUST be playing
 */
neoip.player_t.prototype.practical_reltime	= function()
{
	// sanity check - player_t MUST be in is_playing()
// TODO to check - apparently i use them in player_cb_t and there is a race in which it is not
//	console.assert( this.is_playing() );
	// get the track_reltime from the subplayer_t - the practical one
	return this.m_subplayer.track_reltime();
}

/** \brief Return the current state from the subplayer
 *
 * - WARNING: if subplayer is unable to return the current_state, it may return null
 * - WARNING: the player_t MUST be playing
 */
neoip.player_t.prototype.practical_state		= function()
{
	// sanity check - player_t MUST be in is_playing()
// TODO to check - apparently i use them in player_cb_t and there is a race in which it is not
//	console.assert( this.is_playing() );
	// get the track_reltime from the subplayer_t - the practical one
	return this.m_subplayer.current_state();
}

/** \brief Set the oload_outter_uri
 */
neoip.player_t.prototype.set_outter_uri	= function(p_apps_suffix, p_outter_uri)
{
	// log to debug
	//console.info("apps_suffix="+ p_apps_suffix + " outter_uri=" + p_outter_uri);
	// copy the value
	this.m_outter_uri_arr[p_apps_suffix]	= p_outter_uri;
	// notify the playlist_t of the change
	if( this.m_playlist )	this.m_playlist.set_outter_uri_arr(this.m_outter_uri_arr);
}

neoip.player_t.prototype.has_oload	= function(){ return this.m_outter_uri_arr.oload != null;	}
neoip.player_t.prototype.has_casto	= function(){ return this.m_outter_uri_arr.casto != null;	}

/** \brief Return true if the player is currently playing, false otherwise
 */
neoip.player_t.prototype.is_playing	= function()	{ return this.m_is_playing;	}
neoip.player_t.prototype.is_not_playing	= function()	{ return !this.is_playing();	}


/** \brief Start playing
 */
neoip.player_t.prototype.playing_start = function()
{
	// sanity check - the playlist MUST be set
	console.assert(this.m_playlist);
	// if it is already playing, do nothing and return now
	if( this.is_playing() )	return;
	
	// set this.m_is_playing to true
	this.m_is_playing	= true;
	
	// get the theorical track_pos
	var track_pos	= this.m_playlist.current_track_pos_wseek();
	// launch a _curtrack_start
	this._curtrack_start(track_pos);

	// start the resync_timeout
if( 0 ){	// TODO to reenable later
	this.m_resync_timeout	= setTimeout(function(thisObj){thisObj._resync_if_needed(); }
							, this.m_resync_period * 1000, this);
}
}

/** \brief Stop Playing
 */
neoip.player_t.prototype.playing_stop = function()
{
	// stop all the prefetcher_t
	// - NOTE: done before testing is_playing, as prefetching may happen when not playing
	for(trackidx in this.m_prefetcher_arr)	this._prefetch_try_stop(trackidx)

	// if it is already NOT playing, do nothing and return now
	if( !this.is_playing() )	return;

	// set this.m_is_playing to false
	this.m_is_playing	= false;
	
	// stop the curtrack
	this._curtrack_stop();

	// stop the prefetch_timeout if needed
	if( this.m_prefetch_timeout ){
		clearTimeout( this.m_prefetch_timeout );
		this.m_prefetch_timeout	= null;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			player_cb_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor for a player_ctrl_cb_t
 *
 * - see http://www.dustindiaz.com/javascript-curry/ for principle description 
 */
neoip.player_cb_t	= function(fct, p_scope, userptr) 
{
	var	scope	= p_scope || window;
	return	function(event_type, arg) {
			fct.call(scope, this, userptr, event_type, arg);
		};
}

