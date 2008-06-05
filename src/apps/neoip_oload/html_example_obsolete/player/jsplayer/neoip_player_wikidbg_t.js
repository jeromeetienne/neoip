/*! \file
    \brief Definition of the player_wikidbg_t

\par Brief Description
neoip.player_wikidbg_t provides various wikidbg keyword in order to debug
the neoip.player_t.

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
neoip.player_wikidbg_t	= function(){}

// create player_wikidbg_t as an object with only static functions
neoip.player_wikidbg	= new neoip.player_wikidbg_t();


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			wikidbg
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief main callback for wikidbg
 */
neoip.player_wikidbg_t.prototype.main_cb	= function(cb_obj, keyword, dom_elem)
{
	// if dom_elem is a string, then it is the html_id for the actual dom element
	if( typeof(dom_elem) == "string")	dom_elem = document.getElementById(dom_elem);
	// delete the all subelem of dom_elem
	neoip.wikidbg_subelem_delete(dom_elem);
	// create a neoip.wikidbg_elem_t to ease the building
	var root_elem	= new neoip.wikidbg_elem_t(dom_elem);
	
	// handle the wikidbg according to the keyword
	if( keyword == "page" ){
		return this._page_cb		(cb_obj, keyword, root_elem);
	}else if( keyword == "prefetch" ){
		return this._prefetch_cb	(cb_obj, keyword, root_elem);
	}else if( keyword == "admin" ){
		return this._admin_cb		(cb_obj, keyword, root_elem);
	}else{
		throw "unknown keyword " + keyword;
	}	
}

/** \brief wikidbg callback for keyword "page"
 */
neoip.player_wikidbg_t.prototype._page_cb	= function(player, keyword, root_elem)
{
	// create the table_elem
	var table_elem	= root_elem.table();
	// fillup the table
	table_elem.clone().tr().td()	.bold("Title")	
			.up(2).td()	.text(": " + "wow super title");
	table_elem.clone().tr().td()	.bold("State")	
			.up(2).td()	.text(": " + "wow super state");
}

/** \brief wikidbg callback for keyword "prefetch"
 */
neoip.player_wikidbg_t.prototype._prefetch_cb	= function(player, keyword, root_elem)
{
	// create the table_elem
	var ol_elem	= root_elem.ol();
	
	// loop over all the pending prefetcher_t
	for(var trackidx in player.m_prefetcher_arr){
		var prefetcher	= player.m_prefetcher_arr[trackidx];

		// fillup the table
		var table_elem	= ol_elem.clone().li().table();
		// fillup the table
		table_elem.clone().tr().td()	.bold("track idx")	
				.up(2).td()	.text(": " + trackidx);
		table_elem.clone().tr().td()	.bold("track pos")	
				.up(2).td()	.text(": idx:"	+ prefetcher.m_track_pos.trackidx)
				.up(1)		.text("  reltime:" + prefetcher.m_track_pos.track_reltime);
		table_elem.clone().tr().td()	.bold("mdata_uri")	
				.up(2).td()	.text(": " + prefetcher.m_mdata_uri);
		table_elem.clone().tr().td()	.bold("track_uri")	
				.up(2).td()	.text(": " + prefetcher.m_track_uri);
	}
}

/** \brief wikidbg callback for keyword "admin"
 */
neoip.player_wikidbg_t.prototype._admin_cb	= function(player, keyword, root_elem)
{
	// get some info from the player
	var	trackidx	= player.practical_trackidx();
	var	track_reltime	= player.practical_reltime();
	var	track_state	= player.practical_state();
	var	playlist_track	= player.playlist().track_at(trackidx);
	
	// determine the title
	var	title	= "undefined";
	if( playlist_track.title() )	title	= playlist_track.title();

	// compute the position string
	var	pos_str	= '';
	if( track_reltime ){
		pos_str += (track_reltime/1000).toFixed(2) + "-sec";
		pos_str += " - ";
		pos_str += (100.0 * track_reltime/playlist_track.duration()).toFixed(0) + "%";
		pos_str += " of total ";
	}else{
		pos_str += "position unknown but total "
	}
	pos_str +=( playlist_track.duration()/1000).toFixed(2) + "-sec";

	// create the table_elem
	var table_elem	= root_elem.table();
	// fillup the table
	table_elem.clone().tr().td().bold("Title")	.up(2).td().text(": " + title);
	table_elem.clone().tr().td().bold("State")	.up(2).td().text(": " + track_state);
	table_elem.clone().tr().td().bold("Position")	.up(2).td().text(": " + pos_str);
}






