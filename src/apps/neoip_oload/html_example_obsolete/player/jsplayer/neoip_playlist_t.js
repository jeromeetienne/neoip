// 
// this script implement a playlist_t for the player_t
// 
// - TODO it has 2 format jspf and a json custom (obsolete to remove)


// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			playlist_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the playlist_t
 */
neoip.playlist_t = function(playlist_str)
{
	// if playlist_str is provided parse it now	
	if( playlist_str )	this.from_jspf(playlist_str);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse the playlist_t from a string contains 'jspf format
 * 
 * - jspf is the json version of xspf
 * - http://wiki.xiph.org/index.php/JSPF_Draft
 */
neoip.playlist_t.prototype.from_jspf = function(jspf_str)
{
	// TODO to do eval on json as if if were javascript cause some security issue
	// - IIF the json string is untrusted tho. aka not coming from the same site
	// - some json only parser are available in http://json.org
	// - an additionnal dependancy
	var jspf_data		= eval('(' + jspf_str + ')');
	var jspf_playlist	= jspf_data.playlist;
	//console.info("loaded jspf playlist");
	//console.dir(jspf_playlist);
	
	// get data about the playlist itself
	this.m_base_date	= Date.parse(jspf_playlist.date);
	this.m_identifier	= jspf_playlist.identifier;
	this.m_meta		= {};
	this.m_extension	= {};

	// put some default in the this.m_meta
	this.m_meta['may_loop']		= true;
	this.m_meta['trackidx_beg']	= 0;
	
	// parse all the meta of the jspf_track
	for(var key in jspf_playlist.meta)	this.m_meta[key]	= jspf_playlist.meta[key];
	// parse all the known extension
	for(var key in jspf_playlist.extension)	this.m_extension[key]	= jspf_playlist.extension[key];

	// parse all the track_arr
	this.m_track_arr	= new Array();
	this.m_total_duration	= 0.0;
	for(var i = 0; i < jspf_playlist.track.length; i++){
		// create a new playlist_track
		var track	= new this.track_t(this, i + this.trackidx_beg()
					, this.m_total_duration, jspf_playlist.track[i]);
		// put it in the m_track_arr
		this.m_track_arr.push(track);
		// update the total_duration
		this.m_total_duration	+= this.m_track_arr[i].duration();
	}

	// sanity check - the resulting object MUST be valid
	//console.assert(this.is_valid());
	this.check();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the current track_pos
 */
neoip.playlist_t.prototype.current_track_pos = function(present_date)
{
	// if no present date is specified, get the current one
	// - this allow to get the 'present_date' from an external source
	//   which is important when the operating system date is incorrect
	if( !present_date )	present_date	= new Date();
	// compute the age of the playlist
	var playlist_age	= present_date - this.m_base_date;
	// compute the delay within the playlist
	var track_reltime	= playlist_age % this.m_total_duration;
	// find the track for the current time 
	for(var i = 0; i < this.m_track_arr.length; i++){
		// if this track is the current one, leave the loop
		if( this.m_track_arr[i].m_duration > track_reltime )	break;	
		// update the track_reltime
		track_reltime -= this.m_track_arr[i].m_duration;
	}
	console.info("i=" + i);
	console.info("trackidx_beg()=" + this.trackidx_beg());
	
	// sanity check - i MUST be less than this.m_track_arr.length
	console.assert( i < this.m_track_arr.length );
	// return the result
	return {"trackidx"	: i + this.trackidx_beg(),
		"track_reltime"	: track_reltime
		 }
}

/** \brief Return the current track_pos
 */
neoip.playlist_t.prototype.current_track_pos_wseek = function(present_date)
{
	// get the current_track_pos
	var track_pos	= this.current_track_pos(present_date);
	// if this track_t.may_not_startinmid, zero the track_reltime
	var track	= this.track_at(track_pos.trackidx);
	if( track.may_not_startinmid() )	track_pos.track_reltime	= 0;
	// return the result
	return track_pos;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			check functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Check that the playlist is valid
 * 
 * - if the playlist is not valid an exception will be thrown
 */
neoip.playlist_t.prototype.check = function()
{
	// check the type of each fields
	if( !typeof(this.m_base_date) == "number" )	throw("base_date is NOT Number");
	if( !(this.m_track_arr  instanceof Array) )	throw("track_arr is NOT Array");

	// check each track of the track_arr
	for(var i = 0; i < this.m_track_arr.length; i++){
		// check the type of this track
		if( !(this.m_track_arr[i]  instanceof this.track_t) )
			throw("track " + i + " is not an playlist_t.track_t");
		// check this track_t
		this.m_track_arr[i].check();
	}
}

/** \brief Return true if the playlist_t is valid, false otherwise
 */
neoip.playlist_t.prototype.is_valid	= function()
{
	try 	{	this.check();	return true;	}
	catch(e){	return false;			}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if both playlist_t are of the same instance, false otherwise
 */
neoip.playlist_t.prototype.same_instance_as	= function(other_playlist)
{
	// test all cases
	if( this.identifier()		!= other_playlist.identifier() )	return false;
	if( this.instance_nonce()	!= other_playlist.instance_nonce() )	return false;
	// if all tests passed, both playlist_t are considered of the same instance
	return true;
}

/** \brief Maintain a outter_uri_arr in the playlist_t
 */
neoip.playlist_t.prototype.set_outter_uri_arr	= function(outter_uri_arr)
{
	// sanity check - outter_uri_arr MUST not be null
	console.assert( outter_uri_arr );
	// copy the value
	this.m_outter_uri_arr	= outter_uri_arr;
}

neoip.playlist_t.prototype.contain_trackidx	= function(trackidx)
{
	if( trackidx	<  this.trackidx_beg() )	return false;
	if( trackidx	>= this.trackidx_end() )	return false;
	return true;
}

/** \brief Return the track at trackidx (with a handling for this.m_trackidx_beg)
 */
neoip.playlist_t.prototype.track_at		= function(trackidx)
{
	// TODO to remove - only display to debug
	if( !this.contain_trackidx(trackidx) ){
		console.info("trackidx=" + trackidx);
		console.info("trackidx_beg=" + this.trackidx_beg());
		console.info("trackidx_end=" + this.trackidx_end());
		console.trace();
		console.assert(false);
	}
	// sanity check - this trackidx MUST be present
	console.assert( this.contain_trackidx(trackidx) );
	// return the track_t for this trackidx
	return this.m_track_arr[trackidx - this.trackidx_beg()];
}

// declare all the methods to read the variables
neoip.playlist_t.prototype.total_duration	= function(){ return this.m_total_duration;	}

neoip.playlist_t.prototype.base_date		= function(){ return this.m_base_date;		}
neoip.playlist_t.prototype.identifier		= function(){ return this.m_identifier;		}

neoip.playlist_t.prototype.meta			= function(){ return this.m_meta;		}
neoip.playlist_t.prototype.extension		= function(){ return this.m_extension;		}

neoip.playlist_t.prototype.may_loop		= function(){ return this.m_meta['may_loop'];		}
neoip.playlist_t.prototype.may_not_loop		= function(){ return !this.may_loop();			}
neoip.playlist_t.prototype.reload_delay		= function(){ return this.m_meta['reload_delay'];	}
neoip.playlist_t.prototype.instance_nonce	= function(){ return this.m_meta['instance_nonce'];	}
neoip.playlist_t.prototype.trackidx_beg		= function(){ return this.m_meta['trackidx_beg'];	}

neoip.playlist_t.prototype.trackidx_end		= function(){ return this.trackidx_beg()
								+ this.m_track_arr.length;	}
neoip.playlist_t.prototype.nb_track		= function(){ return this.m_track_arr.length;	}



