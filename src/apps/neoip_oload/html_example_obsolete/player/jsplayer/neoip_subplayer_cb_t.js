/*! \file
    \brief Definition of the neoip.subplayer_t

\par Brief Description
This object implement the callback object for the neoip.subplayer_t
- if a callback is specified, it MUST be of neoip.subplayer_cb_t type

*/

// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			subplayer_cb_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor for a subplayer_cb_t
 *
 * - see http://www.dustindiaz.com/javascript-curry/ for principle description 
 */
neoip.subplayer_cb_t	= function(fct, p_scope, userptr) 
{
	var	scope	= p_scope || window;
	return	function(event_type, arg) {
			fct.call(scope, this, userptr, event_type, arg);
		};
}


