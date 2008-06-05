/**
 * - look at http://www.javascriptsearch.com/scripts/User_Information/plug_ins.html
 *   - to get to know if the vlc plugin is currently installed or not
 */


// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};

/** \brief Return true if local browser has VLC plugin installed - false otherwise
 */
neoip.vlc_plugin_present	= function() {
	var	nb_plugin	= navigator.plugins.length;
	// go thru every installed plugin
	for(var i=0; i < nb_plugin; i++) {
		if( navigator.plugins[i].name == "VLC Multimedia Plugin")
			return true;
	}
	// if no plugin name matches, return false
	return false;
}


/** \brief Build a vlc plugin object 
 *
 * - similar to the usual SWFObject
 * - NOTE: if vlc plugin is not available, it will throw an exception
 */
neoip.vlc_object = function(object_id, width, height){
	// copy the parameter
	this.width	= width;
	this.height	= height;
	this.object_id	= object_id;
	
	// if vlc plugin is not present, throw an exception
	if( neoip.vlc_plugin_present() == false )
		throw("VLC Multimedia Plugin Not available on your browser.")
}

/** \brief Write the html result into the dest_id
 */
neoip.vlc_object.prototype.write = function(dest_id)
{
	var	html_str = "";
	// build the html object string
	html_str	+= '<object type="application/x-vlc-plugin"'
	html_str	+= 	' pluginspage="http://www.videolan.org/"'
	html_str	+= 	' version="VideoLAN.VLCPlugin.2"'
	html_str	+= ' width="'	+ this.width		+ '"';
	html_str	+= ' height="'	+ this.height		+ '"';
	html_str	+= ' id="'	+ this.object_id	+ '"';
	html_str	+= '>'
	html_str	+= '</object>'
	// log to debug
	//console.log(html_str)
	// write the html object string to the dest_id
	document.getElementById(dest_id).innerHTML = html_str;
}