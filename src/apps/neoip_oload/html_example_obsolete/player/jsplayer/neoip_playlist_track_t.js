// 
// this script implement a playlist_t for the player_t
// 
// - TODO it has 2 format jspf and a json custom (obsolete to remove)


// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief the track_t for each track of the playlist_t
 */
neoip.playlist_t.prototype.track_t = function(p_playlist, p_trackidx, p_base_reltime
							, track_data)
{
	// sanity check - track_data MUST be non null
	console.assert(track_data);
	// copy the parameter
	this.m_playlist		= p_playlist;
	this.m_trackidx		= p_trackidx;
	this.m_base_reltime	= p_base_reltime;
	
	// parse the track_data
	if( track_data )	this._from_jspf(track_data);
}

/** \brief Parse the track_t from track element of a jspf playlist
 */
neoip.playlist_t.prototype.track_t.prototype._from_jspf = function(jspf_track)
{
	this.m_location		= jspf_track.location;
	this.m_duration		= jspf_track.duration;
	this.m_meta		= {};
	this.m_extension	= {};

	// convert the jspf_track.title 
	this.m_title		= "unspecified title"; 
	if( jspf_track.title )	this.m_title = jspf_track.title;

	// put some default in the this.m_meta
	this.m_meta['start_time']	= 0;
	this.m_meta['content_type']	= "static";
	this.m_meta['prefetch_delay']	= Math.min(30*1000, this.m_duration);
	
	// parse all the meta of the jspf_track
	for(var key in jspf_track.meta)		this.m_meta[key]	= jspf_track.meta[key];
	// parse all the known extension
	for(var key in jspf_track.extension)	this.m_extension[key]	= jspf_track.extension[key];
}

/** \brief Check that the playlist is valid
 * 
 * - if the playlist is not valid an exception will be thrown
 */
neoip.playlist_t.prototype.track_t.prototype.check = function()
{
	// check the type of each fields
	if( !typeof(this.m_location) == "string" )	throw("m_location is NOT a string");
	if( !typeof(this.m_duration) == "number" )	throw("m_duration is NOT number");
	if( typeof(this.m_meta['start_time']) != "number" )	throw("m_meta start_time is NOT number");
	if( typeof(this.m_meta['content_type']) != "string" )	throw("m_meta content_type is NOT string");
	if( typeof(this.m_meta['prefetch_delay']) != "number")	throw("m_prefetch_delay is NOT number");
// TODO unclear what it is... is it an object?
//	if( !(this.m_extension instanceof Array) )	throw("m_extension is NOT an Array");


	// if track_t.is_stream, then this.m_meta['start_time'] MUST be 0
	if( this.is_stream() )	console.assert(this.m_meta['start_time'] == 0);
	
	// the prefetch_delay MUST be <= than the duration
	// - TODO what is the reason behind this ? seem weird to me
	console.assert( this.prefetch_delay() <= this.duration() );
}

// declare all the methods to read the variables
neoip.playlist_t.prototype.track_t.prototype.playlist	= function(){ return this.m_playlist;		}
neoip.playlist_t.prototype.track_t.prototype.trackidx	= function(){ return this.m_trackidx;		}
neoip.playlist_t.prototype.track_t.prototype.base_reltime=function(){ return this.m_base_reltime;	}

neoip.playlist_t.prototype.track_t.prototype.location	= function(){ return this.m_location;		}
neoip.playlist_t.prototype.track_t.prototype.title	= function(){ return this.m_title;		}
neoip.playlist_t.prototype.track_t.prototype.duration	= function(){ return this.m_duration;		}

neoip.playlist_t.prototype.track_t.prototype.meta	= function(){ return this.m_meta;		}
neoip.playlist_t.prototype.track_t.prototype.extension	= function(){ return this.m_extension;		}

neoip.playlist_t.prototype.track_t.prototype.start_time	= function(){ return this.m_meta['start_time'];	}
neoip.playlist_t.prototype.track_t.prototype.content_type=function(){ return this.m_meta['content_type'];}

neoip.playlist_t.prototype.track_t.prototype.is_static	= function(){ return this.content_type() == "static";	}
neoip.playlist_t.prototype.track_t.prototype.is_stream	= function(){ return this.content_type() == "stream";	}

/** \brief Return the static_filelen from this.extension['oload'] if present, null otherwise
 */
neoip.playlist_t.prototype.track_t.prototype.static_filelen	= function()
{
	// if this track_t has no "oload" extension, return null
	if( this.m_extension['oload'] == null )				return null;
	// if this.m_extension['oload'] has no "static_filelen", return null
	if( this.m_extension['oload']['static_filelen'] == null )	return null;
	// else return the value
	return this.m_extension['oload']['static_filelen'];
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			seekability
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if this track is may be started in the middle, false otherwise
 */
neoip.playlist_t.prototype.track_t.prototype.may_startinmid	= function()
{
	var outter_uri_var	= this.m_playlist.m_outter_uri_arr;
	// a track_t.is_stream can always start in the middle
	if( this.is_stream() )	return true;
	// NOTE: from here the track_t.is_static
	console.assert( this.is_static() );
	
	// TODO to code
	// - ok the data is now available in the track_t
	// - how to pass the oload presence here ?
	// - and the subplayer_caps
	
	/* note: a track may not start in the middle if:
	 * - track with vlc and vlc is unable to seek thru it
	 * - player_t is flash and flv has no kframe_index
	 * - player_t is flash and the http server does not support rangereq_byuri
	 *   - depend on the presence of oload (as oload support rangereq_byuri)
	 *   - depend on the http server pointed by 'location' if oload is not present
	 *   - imply to get playlist_t to be aware of the player type and the oload presence
	 *     - maybe to have various precomputation everytime those data changes
	 */

	// NOTE: this assumes to be an flv
	// - how does this work in case of vlc ?
	
	// determine if the track_t is accessed directly or thru neoip
	// NOTE: it assume that if neoip-oload is present it is used.
	var direct_location	= outter_uri_var.oload ? false : true;
	
	// if it is accessed directly to the location, and location_rangereq_byuri_varname
	// is not specified, this track may not start in the middle
	if( direct_location && !this.meta()['location_rangereq_byuri_varname'] )
		return false;

	// if flv_mdata_info['kframe_index_present'] is not 1, this track may not startinmid
	var flv_mdata_info	= this.extension()['flv_mdata_info'];
	if( !flv_mdata_info || flv_mdata_info['kframe_index_present'] != 1 )
		return false;

	// if all previous tests passed, it is considered to be startable in the middle
	return true;
}

/** \brief Just an alias on top of track_t.may_startinmid
 */
neoip.playlist_t.prototype.track_t.prototype.may_not_startinmid	= function(){ return !this.may_startinmid();	}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function about prefetch
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

neoip.playlist_t.prototype.track_t.prototype.prefetch_delay	= function(){ return this.m_meta['prefetch_delay'];	}
neoip.playlist_t.prototype.track_t.prototype.prefetchable	= function(){ return this.prefetch_delay() > 0;		}

/** \brief Return true if this track_t is prefetchable at this base_time
 */
neoip.playlist_t.prototype.track_t.prototype.prefetchable_at	= function(base_time)
{
	// sanity check - ensure the base_time is in an acceptable 
	console.assert( base_time >= 0 && base_time < this.m_playlist.total_duration() );

	// compute the range in which this track_t is considered prefetchable
	var	range_beg	= this.m_base_reltime - this.prefetch_delay();
	var	range_end	= this.m_base_reltime;
	
	// if range_beg may warparound and playlist.may_not_loop, clamp range_beg to 0
	if( range_beg < 0 && this.m_playlist.may_not_loop() )
		range_beg	= 0;

	// special case if the range goes 'before' the begining of the playlist_t
	// - aka if there is a warparound in the range
	if( range_beg < 0 ){
		// update the range_beg
		range_beg	+= this.m_playlist.total_duration();
		// perform the test - taking into account the  
		if( base_time >= range_beg )	return true;
		if( base_time <= range_end )	return true;
		return false;
	}
	
	// if base_time is outside the range, return false
	if( base_time < range_beg )	return false;
	if( base_time > range_end )	return false;
	// if all previous tests passed, this track_t is considered prefetchable
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** Return the outter_uri for the content_type of this track_t
 * 
 * - or null if there is no outter_uri
 */
neoip.playlist_t.prototype.track_t.prototype.outter_uri = function()
{
	var outter_uri_arr	= this.m_playlist.m_outter_uri_arr;
	// if this track_t is static, return outter_uri for neoip-oload
	if( this.is_static() )	return outter_uri_arr.oload;
	// if this track_t is stream, return outter_uri for neoip-casto
	if( this.is_stream() )	return outter_uri_arr.casto;
	// NOTE: this point SHOULD NEVER be reached
	console.assert( 0 );
	return null;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			cooked_uri stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** Return a string of the cooked_uri for this track 
 */
neoip.playlist_t.prototype.track_t.prototype.cooked_uri = function(outter_var)
{
	var outter_uri_arr	= this.m_playlist.m_outter_uri_arr;
	// handle it according to its content_type
	if( this.is_static() ){
		// if neoip-oload is not present, return the raw track_t.location() 
		if( !outter_uri_arr.oload )	return this.location();
		// if neoip-oload is present, return the cooked_uri
		return this._cooked_uri_static(outter_uri_arr.oload, outter_var);
	}else if( this.is_stream() ){
		// if neoip-casto is not present, return the raw track_t.location() 
		if( !outter_uri_arr.casto )	return this.location();
		// if neoip-casto is present, return the cooked_uri
		return this._cooked_uri_stream(outter_uri_arr.casto, outter_var);
	}
	// NOTE: this point SHOULD NEVER be reached
	console.assert( 0 );	
	return null;
}

/** \brief Return the location cooked to be used for static playlist_track
 *
 * - TODO to handle the extension['oload']['required'] == true
 */
neoip.playlist_t.prototype.track_t.prototype._cooked_uri_static= function(outter_uri, outter_var)
{
	// sanity check - track_t MUST be static
	console.assert( this.is_static() );

	// build the nested_uri
	var	nested_uri	= new neoip.nested_uri_builder_t();
	nested_uri.outter_uri	(outter_uri);
	nested_uri.inner_uri	(this.location());
	// merge this.extension().oload and outter_var array into tmp_var
	var	tmp_var		= {};
	for(var key in this.extension().oload )	tmp_var[key] = this.extension().oload[key];
	for(var key in outter_var )		tmp_var[key] = outter_var[key];
	// TODO experiment with 'cache_buster' 
	//tmp_var['cache_buster']	= Math.floor(Math.random()*999999);
	// set all the outter_var in the nested_uri
	nested_uri.set_var_arr(tmp_var);
	// return the result
	return nested_uri.to_string();
}

/** \brief Return the location cooked to be used for stream playlist_track
 *
 * - TODO to handle the extension['casto']['required'] == true
 *
 * \param track the playlist_t.track_t to build an location for
 * \param prefetching if true, build an location for prefetching
 */
neoip.playlist_t.prototype.track_t.prototype._cooked_uri_stream= function(outter_uri, outter_var)
{
	// sanity check - track MUST be stream
	console.assert( this.is_stream() );
	// build the cooked_uri
	var	cooked_uri	= outter_uri + "/" + this.extension().casto.cast_name;
	// merge this.extension().casto and outter_var array into tmp_var
	var	tmp_var		= {};
	for(var key in this.extension().casto)	tmp_var[key] = this.extension().casto[key];
	for(var key in outter_var)		tmp_var[key] = outter_var[key];
	// append all tmp_var at the end of the cooked_uri
	for(var key in tmp_var ){
		// if this is "cast_name", goto the next - it always has been given in path
		if( key == "cast_name" )	continue;
		// add the variable separator
		cooked_uri	+= cooked_uri.indexOf('?') == -1 ? "?" : "&";
		// add the key+value
		cooked_uri	+= escape(key) + "=" + escape(tmp_var[key]);
	}
	// log to debug
	console.info("cookie_uri=" + cooked_uri);
	// return the result
	return cooked_uri;
}


/** \brief Return the mdata_uri for the track_t, adding all specific outter_var
 * 
 * - WARNING: to use ONLY to get flv_mdata_t .. not the content
 */
neoip.playlist_t.prototype.track_t.prototype.flv_mdata_uri	= function(outter_var)
{
	var	result_uri;
	// get the "flv_mdata" extension of this track
	var flv_mdata_info	= this.extension()['flv_mdata_info'];
	if( !flv_mdata_info )	flv_mdata_info = { "type": "internal"	};
	
	if( flv_mdata_info['type'] == "external" ){
		// build the nested_uri
		var	nested_uri	= new neoip.nested_uri_builder_t();
		nested_uri.outter_uri	(this.outter_uri());
		nested_uri.set_var_arr	(outter_var);
		nested_uri.inner_uri	(flv_mdata_info['extmdata_uri']);
		// set tmp_uri with the just built nested_uri
		result_uri	= nested_uri.to_string();
	}else if( flv_mdata_info['type'] == "internal" ){
		// copy outter_var into tmp_var
		var	tmp_var		= {};
		for(var key in outter_var )	tmp_var[key] = outter_var[key];
		// if flv_mdata_info.intmdata_len is specified, update the outter_var
		// - NOTE: only an optimisation to avoid downloading more than necessary
		if( flv_mdata_info['intmdata_len'] )
			tmp_var['read_ahead']	= flv_mdata_info['intmdata_len'];
		// TODO experiment with 'cache_buster' 
		//tmp_var['cache_buster']	= Math.floor(Math.random()*999999);
		// get the cooked_uri for this playlist_track
		result_uri	= this.cooked_uri(tmp_var);
	}
	// return the just-built result_uri
	return	result_uri;
}
