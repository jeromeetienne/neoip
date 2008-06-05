/*! \file
    \brief Definition of the prefetcher_t

\par Brief Description
neoip.prefetcher_t handles the prefetching of the track_t for player_t

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
neoip.prefetcher_t = function(m_track_pos, m_player)
{
	// copy the parameter
	this.m_track_pos	= m_track_pos;
	this.m_player		= m_player;
	// log to debug
	console.info("m_track_pos trackidx=" + m_track_pos.trackidx + " track_reltime="
						+ m_track_pos.track_reltime);
	// get the playlist_track for the trackidx
	var playlist_track	= m_player.m_playlist.track_at(m_track_pos.trackidx);
	// prefetching is only valid when going thru neoip apps
	if( playlist_track.is_static() && !m_player.has_oload() )	return;
	if( playlist_track.is_stream() && !m_player.has_casto() )	return;
	// get the "flv_mdata" extension of this track
	var flv_mdata_info	= playlist_track.extension().flv_mdata_info;
	if( !flv_mdata_info )	flv_mdata_info = { "type": "internal"	};

	// get the outter_uri toward which sending the create/delete_prefetch xmlrpc
	var outter_uri;
	if( playlist_track.is_static())	outter_uri = m_player.m_outter_uri_arr.oload;
	if( playlist_track.is_stream())	outter_uri = m_player.m_outter_uri_arr.casto;

	// set the uri toward which sending the rpc
	this.m_rpc_uri	= outter_uri + "/neoip_uri_prefetch_jsrest.js";

	// compute the absolute time within the track
	var	track_abstime	= playlist_track.start_time() + m_track_pos.track_reltime;

	// create_prefetch specific for the mdata
	this.m_mdata_uri	= playlist_track.flv_mdata_uri({ "httpo_maxrate": "0k"	});
	// launch to debug
	console.info("mdata_uri=" + this.m_mdata_uri);
	// launch the create_prefetch for this.m_mdata_uri - with no callback
	new neoip.xdomrpc_t(this.m_rpc_uri, null, "create_prefetch", this.m_mdata_uri);

	// determine if it is possible to get the exact kframe byte position
	var	has_kframe_find	= m_player.m_subplayer.flv_kframe_find != null;
	// If subplayer has_kframe_find, and track_abstime > 0, do flv_kframe_find
	if( has_kframe_find && track_abstime > 0){
		m_player.m_subplayer.flv_kframe_find(track_abstime, flv_mdata_info.type
					, playlist_track.flv_mdata_uri(), m_track_pos.trackidx);
		// return now - create_prefetch on this.m_track_uri will be done in callback 
		return;
	}

	// get the cooked_uri for this playlist_track, with prefetching as true
	this.m_track_uri	= this._cpu_track_uri();

	// log to debug
	console.info("track_uri=" + this.m_track_uri);

	// launch the create_prefetch for this.m_track_uri - with no callback
	new neoip.xdomrpc_t(this.m_rpc_uri, null, "create_prefetch", this.m_track_uri);
}

/** \brief destructor
 */
neoip.prefetcher_t.prototype.destructor	= function()
{
	// log to debug
	//console.info("mdata_uri="+ this.m_mdata_uri + " track_uri=" + this.m_track_uri);
	// launch the delete_prefetch - with no callback
	if( this.m_track_uri )	new neoip.xdomrpc_t(this.m_rpc_uri, null, "delete_prefetch", this.m_track_uri);
	if( this.m_mdata_uri )	new neoip.xdomrpc_t(this.m_rpc_uri, null, "delete_prefetch", this.m_mdata_uri);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			compute the uri
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the track_uri for the track_t, adding all specific outter_var
 */
neoip.prefetcher_t.prototype._cpu_track_uri	= function(outter_var)
{
	// get the playlist_track for the trackidx
	var playlist_track	= this.m_player.m_playlist.track_at(this.m_track_pos.trackidx);	
	// build the outter_var for playlist_track.cooked_uri
	var	tmp_var		= { "httpo_maxrate" : "0k" };
	for(var key in outter_var )	tmp_var[key] = outter_var[key];
	// call playlist_track.cooked_uri
	return	playlist_track.cooked_uri(tmp_var);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			flv_kframe_find_cb
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Callback for the subplayer flv_kframe_find
 * 
 * - called by player_t which receive it from subplayer_cb
 */
neoip.prefetcher_t.prototype.flv_kframe_find_cb	= function(arg)
{
	// log to debug
	console.info("enter");	console.dir(arg);
	// sanity check - this.m_track_uri MUST NOT be set
	console.assert( !this.m_track_uri );
	// If the operation "succeed", use its result
	if( arg.event_type == "succeed" ){
		// compute the this.m_track_uri - using the byte_offset	
		this.m_track_uri = this._cpu_track_uri({"subfile_byteoffset" : arg.result.byte_offset});
	}else{
		// compute the this.m_track_uri - fallback on the begining of the file	
		this.m_track_uri = this._cpu_track_uri();
	}
	// log to debug
	console.info("this.m_track_uri=" + this.m_track_uri);
	// launch the create_prefetch for this.m_track_uri - with no callback
	new neoip.xdomrpc_t(this.m_rpc_uri, null, "create_prefetch", this.m_track_uri);
}


