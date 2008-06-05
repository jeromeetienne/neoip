/*! \file
    \brief Definition of the neoip.playlist_loader_t

- TODO the xmlHttpRequest is currently insync
  - this is an remain of experimentation
  - this is useless and harmfull
  - make it async

\par Brief Description
This object implement the mechanism to periodically reload playlist. this 
is part of the 'infinitly long playlist'.

\par Note Implementation
- caching workaround
  - it seems that the result of XMLHttpRequest got cached by firefox
  - so add a fake uri variable with a random number in it to workaround the cache

*/

// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			playlist_loader_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor 
 */
neoip.playlist_loader_t	= function(p_playlist_uri, p_callback) 
{
	// copy the parameter
	this.m_playlist_uri	= p_playlist_uri;
	this.m_callback		= p_callback;
	
	// start the initial m_reload_timeout
	this.m_reload_timeout	= setTimeout(function(thisObj){thisObj._reload_timeout_cb(); }
						, 0, this);
}

/** \brief Destructor
 */
neoip.playlist_loader_t.prototype.destructor	= function()
{
	// delete the timeout if needed
	if( this.m_reload_timeout ){
		clearTimeout(this.m_reload_timeout);
		this.m_reload_timeout	= null;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Load the playlist from the m_playlist_uri
 */
neoip.playlist_loader_t.prototype._load_playlist_now	= function()
{
	// log to debug
	console.info("load playlist " + this.m_playlist_uri);
	// delete the timeout if needed
	if( this.m_reload_timeout ){
		clearTimeout(this.m_reload_timeout);
		this.m_reload_timeout	= null;
	}	
	// load the data
	var playlist_str	= this._file_get_insync(this.m_playlist_uri);
	if( playlist_str == null ){
		var retry_delay	= 10*1000;	// TODO make this tunable
		console.info("unable to load playlist at " + this.m_playlist_uri + ". will retry in " + retry_delay + "-sec");
		this.m_reload_timeout	= setTimeout(function(thisObj){thisObj._reload_timeout_cb(); }
								, retry_delay, this);
		return;
	}

	// build the playlist_t
	var playlist	= new neoip.playlist_t(playlist_str);
	// update the playlist in the player_t
	this._notify_callback("new_playlist", { "playlist" : playlist });			
	// init the reload timeout if needed
	if( playlist.reload_delay() ){
		this.m_reload_timeout	= setTimeout(function(thisObj){thisObj._reload_timeout_cb(); }
						, playlist.reload_delay(), this);
		return;
	}
}


/** \brief Load the content of a url synchronously
 */
neoip.playlist_loader_t.prototype._file_get_insync	= function(url)
{
	var xmlhttp	= new XMLHttpRequest();
	// do a open insync
	// NOTE: use the classical workaround to workaround browser caching it
	var nocache_url	= url;
	nocache_url	+= nocache_url.indexOf('?') == -1 ? "?" : "&";
	nocache_url	+= "nocache_workaround="+Math.floor(Math.random()*999999)
	xmlhttp.open("GET", nocache_url, false);
	// start the request
	xmlhttp.send(null);
	// log to debug
	//console.info("response=" + xmlhttp.responseText);
	//console.info("status=" + xmlhttp.status);
	// if request failed, return a null object 
	if( xmlhttp.status != 200 )	return null;
	// else return the content of the file as text
	return xmlhttp.responseText;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief The timeout callback
 */
neoip.playlist_loader_t.prototype._reload_timeout_cb	= function()
{
	// simply forward to load_playlist_now
	this._load_playlist_now();
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Main callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief The flash plugin event callback
 */
neoip.playlist_loader_t.prototype._notify_callback	= function(event_type, arg)
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
//			playlist_loader_cb_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor for a playlist_loader_cb_t
 *
 * - see http://www.dustindiaz.com/javascript-curry/ for principle description 
 */
neoip.playlist_loader_cb_t	= function(fct, p_scope, userptr) 
{
	var	scope	= p_scope || window;
	return	function(event_type, arg) {
			fct.call(scope, this, userptr, event_type, arg);
		};
}


