/*! \file
    \brief Definition of the neoip.xdomrpc_t

\par Brief Description
This object implement a cross-domain rpc mechanism based on ""remote scripting 
with script tag"

\par About the callback and the notification
- if neoip.xdomrpc_t constructor specify null as callback, no notification is made
  and the object autodelete itself on completion
- if a callback is specified, it MUST be of neoip.xdomrpc_cb_t type

\par About expire_timeout and network issue
- neoip.xdomrpc_t includes a expire_timeout
- this is usefull to detect the network error with the server
  - e.g. if the server is unreachable
- NOTE: any error once the server is reached, will be notified by fault

\par About the number of parameter to send in the callback
- currently only 4 are accepted, but this can be increased at will
- any non-specified parameters wont be sent

*/

// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor of the object
 */
neoip.xdomrpc_t = function(p_rpc_url, p_callback, method_name, arg0, arg1, arg2, arg3, arg4
							, arg5, arg6, arg7, arg8, arg9)
{
	// copy the parameters
	this.m_obj_id		= neoip_xdomrpc_cb_new_obj_id();
	this.m_callback		= p_callback;
	this.m_rpc_url		= p_rpc_url;
	this.m_expire_delay	= 10.0;			// TODO make this tunable
	this.m_expire_timeout	= setTimeout(function(thisObj){thisObj._expire_timeout_cb(); }
						, this.m_expire_delay * 1000, this);
	// register this xdomrpc to the neoip_xdomrpc_cb_arr
	neoip_xdomrpc_cb_doregister(this);
	// build the call_uri
	var	call_uri	= this.m_rpc_url;
	call_uri	+= "?obj_id="		+ this.m_obj_id;
	call_uri	+= "&js_callback="	+ 'neoip_xdomrpc_cb_callback_from_server';
	call_uri	+= "&method_name="	+ escape(method_name);
	if( arg0 != null )	call_uri += "&arg0=" + escape(arg0);
	if( arg1 != null )	call_uri += "&arg1=" + escape(arg1);
	if( arg2 != null )	call_uri += "&arg2=" + escape(arg2);
	if( arg3 != null )	call_uri += "&arg3=" + escape(arg3);
	if( arg4 != null )	call_uri += "&arg4=" + escape(arg4);
	if( arg5 != null )	call_uri += "&arg5=" + escape(arg5);
	if( arg6 != null )	call_uri += "&arg6=" + escape(arg6);
	if( arg7 != null )	call_uri += "&arg7=" + escape(arg7);
	if( arg8 != null )	call_uri += "&arg8=" + escape(arg8);
	if( arg9 != null )	call_uri += "&arg9=" + escape(arg9);
	// log to debug
	//console.info("call_uri=" + call_uri);
	// do a m_zerotimer_init
	// - NOTE: needed because opera evaluate the <script> *IMMEDIATLY*
	//   after insertion. while firefox doesnt it at the next event loop iteration.
	this._call_uri	= call_uri;
	this.m_zerotimer_init	= setTimeout(function(thisObj){thisObj._zerotimer_init_cb(); }
						, 0 * 1000, this);
}

/** \brief destructor of the object
 */
neoip.xdomrpc_t.prototype.destructor = function()
{
	// delete the m_zerotimer_init if needed
	if( this.m_zerotimer_init )	clearTimeout( this.m_zerotimer_init );
	// delete the m_expire_timeout if needed
	if( this.m_expire_timeout )	clearTimeout( this.m_expire_timeout );
	// unregister this xdomrpc to the neoip_xdomrpc_cb_arr
	neoip_xdomrpc_cb_unregister(this);
	// delete the iframe for this xdomrpc
	var div_elem	= document.getElementById('neoip.xdomrpc_temp_div');
	var htmlid_root	= "neoip_xdomrpc_script_" + this.m_obj_id;
	try {	div_elem.removeChild(document.getElementById(htmlid_root + "_pre"));
	}catch(e){}
	try {	div_elem.removeChild(document.getElementById(htmlid_root + "_call"));
	}catch(e){}
	try {	div_elem.removeChild(document.getElementById(htmlid_root + "_post"));
	}catch(e){}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			m_zerotimer_launch callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief The _zerotimer_init_cb
 */
neoip.xdomrpc_t.prototype._zerotimer_init_cb	= function()
{
	var	call_uri	= this._call_uri;
	// log to debug
	
	// delete the m_zerotimer_init
	clearTimeout( this.m_zerotimer_init );
	this.m_zerotimer_init	= null;

	// Get the div_elem in which to store the iframe
	var	div_elem	= document.getElementById('neoip.xdomrpc_temp_div');
	// if div_elem doest not exist, create it now
	if( !div_elem ){
		div_elem	= document.createElement('div');
		div_elem.setAttribute('id','neoip.xdomrpc_temp_div');
		document.body.appendChild(div_elem);
	}
	
	var htmlid_root	= "neoip_xdomrpc_script_" + this.m_obj_id;
	var reply_var	= "neoip_xdomrpc_script_reply_var_" + this.m_obj_id;
	// build an script element PRE-call to detect if the call <script> failed
	var intern_elem	= document.createElement('script');
	intern_elem.setAttribute('id', htmlid_root + "_pre");
	intern_elem.appendChild(document.createTextNode(reply_var + "=null;"));
	div_elem.appendChild(intern_elem);
	// build an script element pointing to the url passed in parameter
	var intern_elem	= document.createElement('script');
	intern_elem.setAttribute('src', call_uri);
	intern_elem.setAttribute('id', htmlid_root + "_call");
	div_elem.appendChild(intern_elem);
	// build an script element POST-call to detect if the call <script> failed
	var intern_elem	= document.createElement('script');
	var text_data	= "neoip_xdomrpc_cb_callback_from_server(" + this.m_obj_id 
						+ ", " + reply_var + ");\n"
			+ "delete " + reply_var + ";\n"
	intern_elem.setAttribute('id', htmlid_root + "_post");
	intern_elem.appendChild(document.createTextNode(text_data));
	// NOTE: this should be the LAST thing of the function as it will notify the caller
	// - to appendChild of a <script> on opera cause its IMMEDIATE evaluation
	div_elem.appendChild(intern_elem);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			m_expire_timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief The _expire_timeout_cb
 */
neoip.xdomrpc_t.prototype._expire_timeout_cb	= function()
{
	// log to debug
	//console.info("enter");
	// stop the expire_timeout
	clearInterval( this.m_expire_timeout );
	this.m_expire_timeout	= null;
	// do the normal callback_cb mechanism with a special error
	var	fault	= { code: -1, string: "expired after " + this.m_expire_delay};
	this.callback_cb(this.m_obj_id, fault, null);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			local callback from the global one
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief callback to receive the result from the server
 */
neoip.xdomrpc_t.prototype.callback_cb	= function(obj_id, fault, returned_val)
{
	// sanity check - the obj_id MUST matches
	console.assert( this.m_obj_id == obj_id );
	// log to debug
	//console.info("fault=" + fault + " returned_value=" + returned_val);
	// to notify the caller if there is a callback
	if( this.m_callback ){
		this.m_callback(fault, returned_val);
	}else{
		// if there is no callback, autodelete
		this.destructor();
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			xdomrpc_cb_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor for a xdomrpc_cb_t
 *
 * - see http://www.dustindiaz.com/javascript-curry/ for principle description 
 */
neoip.xdomrpc_cb_t	= function(fct, p_scope, userptr) 
{
	var	scope	= p_scope || window;
	return	function(fault, returned_val) {
			fct.call(scope, this, userptr, fault, returned_val);
		};
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Global callback for xdomrpc
// - TODO i could put all those function in a namespace
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

var	neoip_xdomrpc_cb_arr	= new Array();

/** \brief Return a free obj_id
 */
neoip_xdomrpc_cb_new_obj_id	= function(obj)
{
	var	obj_id;
	// draw a new random obj_id until a free one is found
	do {
		obj_id	= Math.floor(Math.random() * 65536);
	}while( neoip_xdomrpc_cb_arr[obj_id] );
	// return the obj_id
	return obj_id;
}

/** \brief Register a neoip.xdomrpc_t
 */
neoip_xdomrpc_cb_doregister	= function(obj)
{
	// log to debug
	//console.info("doregister obj_id=" + obj.m_obj_id);
	// sanity check - this obj.m_obj_id MUST NOT already exists
	console.assert( !neoip_xdomrpc_cb_arr[obj.m_obj_id] );
	// register this obj into the global array
	neoip_xdomrpc_cb_arr[obj.m_obj_id]	= obj;
}

/** \brief UnRegister a neoip.xdomrpc_t
 */
neoip_xdomrpc_cb_unregister	= function(obj)
{
	// log to debug
	//console.info("unregister obj_id=" + obj.m_obj_id);
	// sanity check - this obj.m_obj_id MUST already exists
	console.assert( neoip_xdomrpc_cb_arr[obj.m_obj_id] );
	// delete it from the array
	// TODO delete is bad!!!! it should be a splice.. but splice doesnt seems to work
	// - likely an issue with another bug
	//neoip_xdomrpc_cb_arr.splice(obj.m_obj_id, 1);
	delete neoip_xdomrpc_cb_arr[obj.m_obj_id];
	// sanity check - this obj.m_obj_id MUST NOT already exists
	console.assert( !neoip_xdomrpc_cb_arr[obj.m_obj_id] );
}

/** \brief Global callback called from the script
 */
neoip_xdomrpc_cb_callback_from_server	= function(obj_id, reply)
{
	// log to debug
	//console.info("obj_id=" + obj_id + " reply=" + reply);
	// get the subplayer for this pid
	var	obj	= neoip_xdomrpc_cb_arr[obj_id];
	// sanity check - this key MUST already exists
	console.assert( obj );
	// if reply is still null, this means the server has not been reached
	if( reply == null ){
		obj.callback_cb(obj_id, { code: -1, string: "Server Unreachable"}, null);
		return;	
	}
	// log to debug
	//console.info("fault=" + reply.fault + " returned_val=" + reply.returned_val);
	// forward it to this obj
	obj.callback_cb(obj_id, reply.fault, reply.returned_val);
};

