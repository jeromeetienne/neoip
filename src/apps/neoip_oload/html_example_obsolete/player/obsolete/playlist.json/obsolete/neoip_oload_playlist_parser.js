// 
// this script implement a parser for the neoip-oload playlist
// 

// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};


/** \brief to parse a playlist
 */
neoip.oload_playlist_parser = function(playlist_json)
{
	// TODO make it eval from a text taken elsewhere
	// TODO to do eval on json as if if were javascript cause some security issue
	// - IIF the json string is untrusted tho. aka not coming from the same site
	// - some json only parser are available in http://json.org
	// - an additionnal dependancy
	_playlist	= eval('(' + playlist_json + ')');

	// compute the total_duration of the whole playlist - in second
	var _total_duration	= 0.0;
	for(var i = 0; i < _playlist.item.length; i++){
		_total_duration	+= _playlist.item[i].duration;
	}
	
	/** \brief Convert the item url to use neoip-oload
	 * 
	 * - TODO find a better name and place for this function
	 */
	this.oloadify= function(oload_outter_uri){
		// log to debug
		//console.log("oloadification")
		// go thru every item of the playlist
		for(var i = 0; i < _playlist.item.length; i++){
			var	inner_uri	= _playlist.item[i].url;
			var	outter_var	= _playlist.item[i].outter_var;
			// build the nested_uri
			var	nested_uri	= new neoip.nested_uri_builder();
			nested_uri.outter_uri	(oload_outter_uri);
			nested_uri.inner_uri	(inner_uri);
			// add all the outter_var in a generic fashion
			for(key in outter_var){
				// log to debug
				//console.log("key=" + key + " value=" + outter_var[key]);					
				// call the nested_uri function 
				// - TODO: is this eval() dangerous ?
				// - NOTE: little kludge to handle dupuri to get multiple dupuri in a array
				//   - any outter_var key containing dupuri string is a play dupuri
				//   - typically if there is several dupuri use dupuri_1 dupuri_2 etc...
				if( key.match("dupuri") ){
					eval('nested_uri.dupuri("' + outter_var[key]+ '")');
				}else{
					eval('nested_uri.' + key + '("' + outter_var[key]+ '")');
				}
			}
			// overwrite the item.url with the nested_uri
			_playlist.item[i].url	= nested_uri.to_string();
		}
	}
	
	/** \brief return the current item + position within this item
	 */
	this.current = function(present_date){
		// if no present date is specified, get the current one
		// - this allow to get the 'present_date' from an external source
		//   which is important when the operating system date is incorrect
		if( !present_date )	present_date	= new Date();
		// compute the age of the playlist in second
		playlist_age	= (present_date - Date.parse(_playlist.base_date))/1000;
		// compute the delay within the playlist
		delay_within	= playlist_age % _total_duration;
		// find the item for the current time 
		for(var i = 0; i < _playlist.item.length; i++){
			if( _playlist.item[i].duration > delay_within )	break;	
			delay_within -= _playlist.item[i].duration;
		}
		// return the result
		return {"offset_time"	: delay_within, 
			"item"		: _playlist.item[i] }
	}
}


