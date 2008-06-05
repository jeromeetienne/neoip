/*! \file
    \brief Definition of the neoip.wikidbg_t

\par Brief Description
neoip.wikidbg_t implements a bunch of tool to help the debugging of javascript

*/

// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			wikidb misc
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Delete all the subelement from the root_elem
 */
neoip.wikidbg_subelem_delete	= function(root_elem)
{
	// loop until there is no more childNodes in the root_elem
	while( root_elem.childNodes.length ){
		// delete the first child
		var childnode	= root_elem.childNodes[0];
		root_elem.removeChild(childnode);
	}
}

/** \brief Delete all the subelement from the root_elem
 */
neoip.wikidbg_dump_object	= function(root_elem, obj)
{
	// handle the 'simple type' immediatly
	if( typeof(obj) != "object"){
		var text_elem	= document.createTextNode("[" + typeof(obj) + "] - " + obj);
		root_elem.appendChild(text_elem);
		return;
	}

	// handle the special case of null
	if( obj == null ){
		var text_elem	= document.createTextNode("null");
		root_elem.appendChild(text_elem);
		return;
	}

	// add the object type
	var text_elem	= document.createTextNode("[" + typeof(obj) + "]");
	root_elem.appendChild(text_elem);
	// create the ul which gonna hold this object
	var ul_elem	= document.createElement('ul');
	// handle each property of this object
	for(var key in obj){
		// skip this key if this is a "function"
		if( typeof(obj[key]) == "function" )	continue;
		// create the 'li' for this key 
		var li_elem	= document.createElement('li');
		li_elem.appendChild(document.createTextNode(key + ": "));
		// reccusive call this this obj
		neoip.wikidbg_dump_object(li_elem, obj[key]);
		// attach this 'li' to the 'ul'
		ul_elem.appendChild(li_elem);
	}
	// add the list to the root_elem		
	root_elem.appendChild(ul_elem);
}


/** \brief Delete all the subelement from the root_elem
 */
neoip.wikidbg_periodic	= function(period, wikidbg_obj, obj, keyword, root_elem)
{
	// log to debug
	//console.info("Enter wikidbg_periodic");
	// call the actual wikidbg function
	wikidbg_obj.main_cb(obj, keyword, root_elem)
	
	// create the function closure to the timeout callback
	var timeout_cb_fct  = function() { 
			neoip.wikidbg_periodic(period, wikidbg_obj, obj, keyword, root_elem);
		}	
	// init the timeout for the next call after period-ms expired
	setTimeout(timeout_cb_fct, period);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			wikidbg_elem_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
neoip.wikidbg_elem_t = function(dom_elem)
{
	this.m_dom_elem	= dom_elem
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			private function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief append a child element from a dom_element
 */
neoip.wikidbg_elem_t.prototype._append_child_elem	= function(new_elem)
{
	// do the appendChild to the current dom_elem
	this.m_dom_elem.appendChild(new_elem);
	// update the current dom_elem to be the new one
	this.m_dom_elem	= new_elem;
	// return the object itself
	return this; 
}

/** \brief Create a dom elem with the tag_str and then append it as child
 */
neoip.wikidbg_elem_t.prototype._append_child_create	= function(tag_str)
{
	// create the new dom element
	var new_elem	= document.createElement(tag_str);
	return this._append_child_elem(new_elem);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			public function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

neoip.wikidbg_elem_t.prototype.ul	= function(){ return this._append_child_create("ul");	}
neoip.wikidbg_elem_t.prototype.ol	= function(){ return this._append_child_create("ol");	}
neoip.wikidbg_elem_t.prototype.li	= function(){ return this._append_child_create("li");	}
neoip.wikidbg_elem_t.prototype.table	= function(){ return this._append_child_create("table");}
neoip.wikidbg_elem_t.prototype.tr	= function(){ return this._append_child_create("tr");	}
neoip.wikidbg_elem_t.prototype.td	= function(){ return this._append_child_create("td");	}
neoip.wikidbg_elem_t.prototype.span	= function(){ return this._append_child_create("span");	}

neoip.wikidbg_elem_t.prototype.text	= function(text_str)
{
	// create the new dom element
	var new_elem	= document.createTextNode(text_str);
	return this._append_child_elem(new_elem);
}

neoip.wikidbg_elem_t.prototype.inner_html	= function(value)
{
	this.m_dom_elem.innerHTML	= value;
}

neoip.wikidbg_elem_t.prototype.bold	= function(text_str)
{
	var tmp		= this._append_child_create("b");
	// if text_str is not specified, return now
	if(typeof(text_str) == "undefined")	return tmp;
	// if text_str is specified, add it and return
	return tmp.text(text_str).up();	
}

/** \brief Goes up in the node hierarchy nb_time (if not specified, go once)
 */
neoip.wikidbg_elem_t.prototype.up	= function(nb_time)
{
	// if nb_time is not specified, default to 1 
	if(typeof(nb_time) == "undefined")	nb_time = 1;
	// goto the parent nb_time
	for(var i = 0; i < nb_time; i++)	this.m_dom_elem	= this.m_dom_elem.parentNode;
	// return the object itself
	return this; 
}

/** \brief Close the wikidbg_elem_t object
 */
neoip.wikidbg_elem_t.prototype.clone	= function()
{
	return neoip.core.object_clone(this);
}
