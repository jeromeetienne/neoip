/*! \file
    \brief Definition of the apps_detect_wikidbg_t

\par Brief Description
neoip.apps_detect_wikidbg_t provides various wikidbg keyword in order to debug
the neoip.apps_detect_t.

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
neoip.apps_detect_wikidbg_t	= function(){}

// create apps_detect_wikidbg_t as an object with only static functions
neoip.apps_detect_wikidbg	= new neoip.apps_detect_wikidbg_t();


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			wikidbg
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief main callback for wikidbg
 */
neoip.apps_detect_wikidbg_t.prototype.main_cb	= function(cb_obj, keyword, dom_elem)
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
	}else{
		throw "unknown keyword " + keyword;
	}	
}

/** \brief wikidbg callback for keyword "page"
 */
neoip.apps_detect_wikidbg_t.prototype._page_cb	= function(apps_detect, keyword, root_elem)
{
	// create the table_elem
	var table_elem	= root_elem.table();
	// fillup the table
	for(var suffix_name in neoip_apps_detect_arr ){
		var str;
		// determine the text according to its presence or not
		if( neoip.apps_present(suffix_name) ){
			str	 = " <font color='green'>HAS been found</font> -";
			str	+= " version " + neoip.apps_version(suffix_name);
			str	+= " with outter_uri of " + neoip.outter_uri(suffix_name);
		}else{
			str	 = "<font color='red'>HAS NOT</font> been found";
			str	+= " running on your computer.";
		}
		// add the result into the table
		table_elem.clone().tr().td()	.bold("neoip-" + suffix_name)	
				.up(2).td()	.span().inner_html(": " + str);
	}
}





